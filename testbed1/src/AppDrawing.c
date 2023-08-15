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





void AppDrawingImport(ecs_world_t *world)
{
	ECS_MODULE(world, AppDrawing);
	ecs_set_name_prefix(world, "Eg");
    ECS_IMPORT(world, EgBasics);
    ECS_IMPORT(world, EgCameras);
    ECS_IMPORT(world, EgRendering);



    /*
	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemParticleCopyGpu",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgCamera), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
			{.id = ecs_id(EgRenderingPipeline), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = ecs_id(EgUse) },
		},
		.callback = SystemParticleCopyGpu
	});
    
	

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
    */



}
