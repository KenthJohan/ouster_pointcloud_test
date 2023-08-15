#pragma once
#include "flecs.h"
#include "sokol_gfx.h"

typedef struct {
    sg_pass_action pass_action;
} EgRenderingPipeline;


extern ECS_COMPONENT_DECLARE(EgRenderingPipeline);

void EgRenderingImport(ecs_world_t *world);
