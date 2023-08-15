#pragma once
#include "flecs.h"
#include "sokol_gfx.h"

typedef struct {
    ecs_i32_t size;
    ecs_i32_t num_instances;
    sg_buffer buffer;
    sg_buffer buffer_verts;
    sg_buffer buffer_index;
} EgDrawBuffer;

extern ECS_COMPONENT_DECLARE(EgDrawBuffer);

void EgDrawBuffersImport(ecs_world_t *world);
