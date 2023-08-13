#include "EgRendering.h"
#include "EgShaders.h"
#include "EgBasics.h"

#include "sokol_gfx.h"


ECS_TAG_DECLARE(EgRenderingPipelineCreate);
ECS_COMPONENT_DECLARE(EgRenderingPipeline);


#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_inst_pos (2)
#define SLOT_vs_params (0)


void SystemCreate(ecs_iter_t *it)
{
	EgShader *shd = ecs_field(it, EgShader, 1);
	for (int i = 0; i < it->count; i ++)
    {
        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
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
        ecs_set(it->world, it->entities[i], EgRenderingPipeline, {pip.id});
    }


}



void EgRenderingImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgRendering);
	ecs_set_name_prefix(world, "Eg");
	ECS_IMPORT(world, EgBasics);


	ECS_COMPONENT_DEFINE(world, EgRenderingPipeline);
	ECS_TAG_DEFINE(world, EgRenderingPipelineCreate);

	ecs_struct(world, {
	.entity = ecs_id(EgRenderingPipeline),
	.members = {
	{ .name = "id", .type = ecs_id(ecs_i32_t) },
	}
	});

	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemCreate",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgShader), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
			{.id = ecs_id(EgRenderingPipeline), .inout = EcsInOut, .oper=EcsNot },
			{.id = EgRenderingPipelineCreate},
		},
		.callback = SystemCreate
	});



}