#pragma once
#include "flecs.h"


typedef struct {
    ecs_i32_t id;
} EgRenderingPipeline;

extern ECS_COMPONENT_DECLARE(EgRenderingPipeline);
extern ECS_TAG_DECLARE(EgRenderingPipelineCreate);

void EgRenderingImport(ecs_world_t *world);
