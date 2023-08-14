#pragma once
#include "flecs.h"
typedef struct {
    ecs_i32_t id;
} AppDraw;

extern ECS_COMPONENT_DECLARE(AppDraw);

void AppDrawingImport(ecs_world_t *world);
