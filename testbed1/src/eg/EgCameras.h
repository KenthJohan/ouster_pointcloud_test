#pragma once
#include "flecs.h"

typedef union
{
    float e[16];
} eg_mat4_t;

typedef struct {
	eg_mat4_t mvp;
    ecs_f32_t ry;
    ecs_f32_t w;
    ecs_f32_t h;
} EgCamera;

extern ECS_COMPONENT_DECLARE(EgCamera);


void EgCamerasImport(ecs_world_t *world);
