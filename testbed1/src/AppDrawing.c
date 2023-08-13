#include "AppDrawing.h"
#include "EgCameras.h"
#include "EgRendering.h"
#include "EgBasics.h"

#include "HandmadeMath.h"
#include "sokol_gfx.h"
#include "sokol_app.h"
#include "dbgui/dbgui.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(AppDraw);

#define MAX_PARTICLES (512 * 1024)
#define NUM_PARTICLES_EMITTED_PER_FRAME (10)

#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_inst_pos (2)
#define SLOT_vs_params (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    hmm_mat4 mvp;
} vs_params_t;
#pragma pack(pop)


static struct {
    sg_pass_action pass_action;
    sg_bindings bind;
    float ry;
    int cur_num_particles;
    hmm_vec3 pos[MAX_PARTICLES];
    hmm_vec3 vel[MAX_PARTICLES];
} state;



void SystemDraw(ecs_iter_t *it)
{
	EgRenderingPipeline *p = ecs_field(it, EgRenderingPipeline, 2);
	EgCamera *c = ecs_field(it, EgCamera, 1);
	for (int i = 0; i < it->count; i ++)
	{
        vs_params_t vs_params;
        ecs_os_memcpy_t(&vs_params.mvp, &c[i].mvp, eg_mat4_t);
        sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
        if(p[i].id > 0)
        {
            sg_apply_pipeline((sg_pipeline){p[i].id});
            sg_apply_bindings(&state.bind);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
            sg_draw(0, 24, state.cur_num_particles);
        }
        else
        {
            printf("Hej!\n");
            state.pass_action.colors[0].clear_value.g += 0.01;
        }
        __dbgui_draw();
        sg_end_pass();
        sg_commit();

        c[i].w = sapp_widthf();
        c[i].h = sapp_heightf();
    }
}


void SystemParticleEmit(ecs_iter_t *it)
{
    // emit new particles
    for (int i = 0; i < NUM_PARTICLES_EMITTED_PER_FRAME; i++) {
        if (state.cur_num_particles < MAX_PARTICLES) {
            state.pos[state.cur_num_particles] = HMM_Vec3(0.0, 0.0, 0.0);
            state.vel[state.cur_num_particles] = HMM_Vec3(
                ((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f,
                ((float)(rand() & 0x7FFF) / 0x7FFF) * 0.5f + 2.0f,
                ((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f);
            state.cur_num_particles++;
        } else {
            break;
        }
    }
}


void SystemParticleUpdate(ecs_iter_t *it)
{
    // update particle positions
    for (int i = 0; i < state.cur_num_particles; i++) {
        state.vel[i].Y -= 1.0f * it->delta_time;
        state.pos[i].X += state.vel[i].X * it->delta_time;
        state.pos[i].Y += state.vel[i].Y * it->delta_time;
        state.pos[i].Z += state.vel[i].Z * it->delta_time;
        // bounce back from 'ground'
        if (state.pos[i].Y < -2.0f) {
            state.pos[i].Y = -1.8f;
            state.vel[i].Y = -state.vel[i].Y;
            state.vel[i].X *= 0.8f; state.vel[i].Y *= 0.8f; state.vel[i].Z *= 0.8f;
        }
    }

    // update instance data
    sg_update_buffer(state.bind.vertex_buffers[1], &(sg_range){
        .ptr = state.pos,
        .size = (size_t)state.cur_num_particles * sizeof(hmm_vec3)
    });
}



void AppDrawingImport(ecs_world_t *world)
{
	ECS_MODULE(world, AppDrawing);
	ecs_set_name_prefix(world, "Eg");
    ECS_IMPORT(world, EgBasics);
    ECS_IMPORT(world, EgCameras);
    ECS_IMPORT(world, EgRendering);


	ECS_COMPONENT_DEFINE(world, AppDraw);







    state.pass_action = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };

    const float r = 0.05f;
    const float vertices[] = {
        // positions            colors
        0.0f,   -r, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
           r, 0.0f, r,          0.0f, 1.0f, 0.0f, 1.0f,
           r, 0.0f, -r,         0.0f, 0.0f, 1.0f, 1.0f,
          -r, 0.0f, -r,         1.0f, 1.0f, 0.0f, 1.0f,
          -r, 0.0f, r,          0.0f, 1.0f, 1.0f, 1.0f,
        0.0f,    r, 0.0f,       1.0f, 0.0f, 1.0f, 1.0f
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices),
        .label = "geometry-vertices"
    });

    // index buffer for static geometry
    const uint16_t indices[] = {
        0, 1, 2,    0, 2, 3,    0, 3, 4,    0, 4, 1,
        5, 1, 2,    5, 2, 3,    5, 3, 4,    5, 4, 1
    };
    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = SG_RANGE(indices),
        .label = "geometry-indices"
    });

    // empty, dynamic instance-data vertex buffer, goes into vertex-buffer-slot 1
    state.bind.vertex_buffers[1] = sg_make_buffer(&(sg_buffer_desc){
        .size = MAX_PARTICLES * sizeof(hmm_vec3),
        .usage = SG_USAGE_STREAM,
        .label = "instance-data"
    });








    ECS_SYSTEM(world, SystemParticleEmit, EcsOnUpdate, EgRenderingPipeline);
    ECS_SYSTEM(world, SystemParticleUpdate, EcsOnUpdate, EgRenderingPipeline);

	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemDraw",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgCamera), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
			{.id = ecs_id(EgRenderingPipeline), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
		},
		.callback = SystemDraw
	});


}
