#pragma once
#include "flecs.h"

typedef union
{
    float e[16];
} eg_mat4_t;

typedef struct {
	eg_mat4_t mvp;
    float ry;
    float w;
    float h;
} EgCamera;

extern ECS_COMPONENT_DECLARE(EgCamera);


void EgCamerasImport(ecs_world_t *world);
