#pragma once
#include "flecs.h"


typedef struct {
    ecs_i32_t id;
} EgRenderingPipeline;

ECS_COMPONENT_DECLARE(EgRenderingPipeline);


void EgRenderingImport(ecs_world_t *world);
