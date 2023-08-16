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
#include "EgBasics.h"
#include "EgRendering.h"
#include "EgDrawBuffers.h"
#include "EgQuantities.h"
#include "AppDrawing.h"
#include "AppParticles.h"

#include <stdlib.h> // rand()
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "dbgui/dbgui.h"



ecs_world_t * world;
ecs_u64_t frame_number = 0;


void init(void) {

    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });
    __dbgui_setup(sapp_sample_count());

    world = ecs_init();
    ECS_IMPORT(world, FlecsMonitor);
    ECS_IMPORT(world, EgBasics);
    ECS_IMPORT(world, EgStr);
    ECS_IMPORT(world, EgShaders);
    ECS_IMPORT(world, EgCameras);
    ECS_IMPORT(world, EgFs);
    ECS_IMPORT(world, EgFetcher);
    ECS_IMPORT(world, EgRendering);
    ECS_IMPORT(world, EgDrawBuffers);
    ECS_IMPORT(world, EgQuantities);
    ECS_IMPORT(world, AppDrawing);
    ECS_IMPORT(world, AppParticles);

    //https://www.flecs.dev/explorer/?remote=true
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	ecs_plecs_from_file(world, "./src/config.flecs");
}

void frame(void) {
    frame_number++;
    //printf("%ju\n", frame_number);
    ecs_progress(world, 0);
}

void cleanup(void) {
    ecs_fini(world);
    __dbgui_shutdown();
    sg_shutdown();
}







sapp_desc sokol_main(int argc, char* argv[]) {
    //test();
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
