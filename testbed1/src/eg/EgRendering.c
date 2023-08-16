#include "EgRendering.h"
#include "EgShaders.h"
#include "EgBasics.h"
#include "EgCameras.h"
#include "EgDrawBuffers.h"

#include "HandmadeMath.h"
#include "sokol_gfx.h"
#include "sokol_app.h"
#include <stdio.h>


ECS_COMPONENT_DECLARE(EgRenderingPipeline);


#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_inst_pos (2)
#define SLOT_vs_params (0)
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    hmm_mat4 mvp;
} vs_params_t;
#pragma pack(pop)



void SystemCreate(ecs_iter_t *it)
{
	EgRenderingPipeline *pip = ecs_field(it, EgRenderingPipeline, 1);
	EgShadersProgram *shd = ecs_field(it, EgShadersProgram, 2);
	for (int i = 0; i < it->count; i ++)
    {
		sg_pipeline pipeline = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                // vertex buffer at slot 1 must step per instance
                .buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE,
                .attrs = {
                    [ATTR_vs_pos]      = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=0 },
                    [ATTR_vs_color0]   = { .format=SG_VERTEXFORMAT_FLOAT4, .buffer_index=0 },
                    [ATTR_vs_inst_pos] = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=1 }
                }
            },
            .shader = (sg_shader){shd[i].id},
            .index_type = SG_INDEXTYPE_UINT16,
            .cull_mode = SG_CULLMODE_BACK,
            .depth = {
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "instancing-pipeline"
        });
		
		ecs_set(it->world, it->entities[i], EgExternalId, {pipeline.id});
		
		sg_pass_action pass_action = (sg_pass_action) {
        	.colors[0] = {
				.load_action = SG_LOADACTION_CLEAR,
				.clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
        	}
		};
		pip[i].pass_action = pass_action;
    }
}










void SystemDraw(ecs_iter_t *it)
{
    //printf("SystemDraw %s\n", ecs_get_name(it->world, ecs_field_src(it, 3)));
	EgExternalId *extid = ecs_field(it, EgExternalId, 1);
	EgCamera *camera = ecs_field(it, EgCamera, 2);
	EgRenderingPipeline *pipeline = ecs_field(it, EgRenderingPipeline, 3);
	EgDrawBuffer *buf = ecs_field(it, EgDrawBuffer, 4);
    //printf("Hej! %i\n", it->count);
	for (int i = 0; i < it->count; i ++)
	{
		EgExternalId *u = extid + i;
		EgRenderingPipeline *p = pipeline + i;
		EgCamera *c = camera + i;
		EgDrawBuffer *b = buf + i;

        vs_params_t vs_params;
        ecs_os_memcpy_t(&vs_params.mvp, &c->mvp, eg_mat4_t);
        sg_begin_default_pass(&p->pass_action, sapp_width(), sapp_height());
        if(u->id > 0)
        {
            sg_apply_pipeline((sg_pipeline){u->id});
			sg_bindings bind = {0};
			bind.index_buffer = b->buffer_index;
			bind.vertex_buffers[0] = b->buffer_verts;
			bind.vertex_buffers[1] = b->buffer;
            sg_apply_bindings(&bind);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
            sg_draw(0, 24, b->count);
			b->count = 0;
        }
        else
        {
            p->pass_action.colors[0].clear_value.g += 0.01;
        }
        sg_end_pass();
        sg_commit();

        c[i].w = sapp_widthf();
        c[i].h = sapp_heightf();
    }
}




void EgRenderingImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgRendering);
	ecs_set_name_prefix(world, "Eg");
	ECS_IMPORT(world, EgBasics);
	ECS_IMPORT(world, EgDrawBuffers);

	ECS_COMPONENT_DEFINE(world, EgRenderingPipeline);


	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemCreate",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgRenderingPipeline), .inout = EcsInOut },
			{.id = ecs_id(EgShadersProgram), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
			{.id = ecs_id(EgExternalId), .inout = EcsInOut, .oper=EcsNot }, // Adds this
		},
		.callback = SystemCreate
	});

	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemDraw",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgExternalId), .inout = EcsInOut },
			{.id = ecs_id(EgCamera), .inout = EcsInOut },
			{.id = ecs_id(EgRenderingPipeline), .inout = EcsInOut },
			{.id = ecs_id(EgDrawBuffer), .inout = EcsInOut },
		},
		.callback = SystemDraw
	});

}