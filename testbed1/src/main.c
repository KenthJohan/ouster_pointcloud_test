// sudo apt install libxcursor-dev
// sudo apt install libxinerama-dev
// sudo apt-get install libxi-dev
//------------------------------------------------------------------------------
//  instancing.c
//  Demonstrate simple hardware-instancing using a static geometry buffer
//  and a dynamic instance-data buffer.
//------------------------------------------------------------------------------

#include <testbed1.h>
#include <flecs.h>
#include <stdio.h>
#include <unistd.h>
#include "EgStr.h"
#include "EgShaders.h"
#include "EgCameras.h"
#include "EgFs.h"
#include "EgFetcher.h"

#include <stdlib.h> // rand()
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_fetch.h"
#include "HandmadeMath.h"
#include "dbgui/dbgui.h"
//#include "instancing-sapp.glsl.h"

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
    sg_pipeline pip;
    sg_bindings bind;
    float ry;
    int cur_num_particles;
    hmm_vec3 pos[MAX_PARTICLES];
    hmm_vec3 vel[MAX_PARTICLES];
    char * file_buffer_shader_vs;
    char * file_buffer_shader_fs;
    sg_shader shd;
    ecs_world_t * world;
} state;




void SystemDraw(ecs_iter_t *it)
{
	EgCamera *c = ecs_field(it, EgCamera, 1);
	for (int i = 0; i < it->count; i ++)
	{
        vs_params_t vs_params;
        ecs_os_memcpy_t(&vs_params.mvp, &c[i].mvp, eg_mat4_t);
        sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
        if(state.pip.id > 0)
        {
            sg_apply_pipeline(state.pip);
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





static void fetch_callback(const sfetch_response_t* response) {
    if (response->fetched) {
        //response->data.ptr;
        //(int)response->data.size;
        if((state.pip.id == 0) && state.file_buffer_shader_vs[0] && state.file_buffer_shader_fs[0])
        {
            printf("::::::::::::::file_buffer_shader_vs::::::::::::::\n %s\n\n", state.file_buffer_shader_vs);
            printf("::::::::::::::file_buffer_shader_fs::::::::::::::\n %s\n\n", state.file_buffer_shader_fs);
            
            sg_shader_desc desc = {0};
            desc.attrs[0].name = "pos";
            desc.attrs[1].name = "color0";
            desc.attrs[2].name = "inst_pos";
            desc.vs.source = state.file_buffer_shader_vs;
            desc.vs.entry = "main";
            desc.vs.uniform_blocks[0].size = 64;
            desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
            desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
            desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
            desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
            desc.fs.source = state.file_buffer_shader_fs;
            desc.fs.entry = "main";
            desc.label = "instancing_shader";
            state.shd = sg_make_shader(&desc);

            printf("shd.id %i\n", state.shd.id);
            // a pipeline object
            state.pip = sg_make_pipeline(&(sg_pipeline_desc){
                .layout = {
                    // vertex buffer at slot 1 must step per instance
                    .buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE,
                    .attrs = {
                        [ATTR_vs_pos]      = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=0 },
                        [ATTR_vs_color0]   = { .format=SG_VERTEXFORMAT_FLOAT4, .buffer_index=0 },
                        [ATTR_vs_inst_pos] = { .format=SG_VERTEXFORMAT_FLOAT3, .buffer_index=1 }
                    }
                },
                .shader = state.shd,
                .index_type = SG_INDEXTYPE_UINT16,
                .cull_mode = SG_CULLMODE_BACK,
                .depth = {
                    .compare = SG_COMPAREFUNC_LESS_EQUAL,
                    .write_enabled = true,
                },
                .label = "instancing-pipeline"
            });
        }
    } else if (response->failed) {
        // if loading the file failed, set clear color to red
        state.pass_action = (sg_pass_action) {
            .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 1.0f, 0.0f, 0.0f, 1.0f } }
        };
    }
}

#define FILE_BUFFER_SHADER_SIZE (1024 * 10)

void init(void) {
    state.world = ecs_init();
    ECS_IMPORT(state.world, EgStr);
    ECS_IMPORT(state.world, EgShaders);
    ECS_IMPORT(state.world, EgCameras);
    ECS_IMPORT(state.world, EgFs);
    ECS_IMPORT(state.world, EgFetcher);
    ECS_SYSTEM(state.world, SystemParticleEmit, EcsOnUpdate, EgCamera);
    ECS_SYSTEM(state.world, SystemParticleUpdate, EcsOnUpdate, EgCamera);
    ECS_SYSTEM(state.world, SystemDraw, EcsOnUpdate, EgCamera);

    //https://www.flecs.dev/explorer/?remote=true
	ecs_set(state.world, EcsWorld, EcsRest, {.port = 0});
	ecs_plecs_from_file(state.world, "./src/config.flecs");

    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    __dbgui_setup(sapp_sample_count());

    state.file_buffer_shader_vs = calloc(FILE_BUFFER_SHADER_SIZE, 1);
    state.file_buffer_shader_fs = calloc(FILE_BUFFER_SHADER_SIZE, 1);

    sfetch_setup(&(sfetch_desc_t){
        .max_requests = 2,
        .num_channels = 3,
        .num_lanes = 3,
        .logger.func = slog_func,
    });

    // a pass action for the default render pass
    state.pass_action = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };

    // vertex buffer for static geometry, goes into vertex-buffer-slot 0
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


    sfetch_send(&(sfetch_request_t){
        .path = "./src/shader.vs.glsl",
        .callback = fetch_callback,
        .buffer = (sfetch_range_t){ state.file_buffer_shader_vs, FILE_BUFFER_SHADER_SIZE }
    });


    sfetch_send(&(sfetch_request_t){
        .path = "./src/shader.fs.glsl",
        .callback = fetch_callback,
        .buffer = (sfetch_range_t){ state.file_buffer_shader_fs, FILE_BUFFER_SHADER_SIZE }
    });


    // a shader
    //sg_shader shd = sg_make_shader(instancing_shader_desc(sg_query_backend()));


}

void frame(void) {
    sfetch_dowork();
    ecs_progress(state.world, 0);
}

void cleanup(void) {
    ecs_fini(state.world);
    __dbgui_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    setvbuf(stdout, NULL, _IONBF, 0);
    char buf[100];
    getcwd(buf,sizeof(buf));
    printf("cwd: %s\n", buf);

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = __dbgui_event,
        .width = 800,
        .height = 600,
        .sample_count = 4,
        .window_title = "Instancing (sokol-app)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
