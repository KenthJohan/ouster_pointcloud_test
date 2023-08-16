#pragma once
#include "flecs.h"
#include "sokol_gfx.h"

typedef struct {
    ecs_i32_t cap;
    ecs_i32_t esize;
    ecs_i32_t count;
    sg_buffer buffer;
    sg_buffer buffer_verts;
    sg_buffer buffer_index;
} EgDrawBuffer;

int EgDrawBuffer_append(EgDrawBuffer * buffer, void * data, ecs_i32_t count);

extern ECS_COMPONENT_DECLARE(EgDrawBuffer);

void EgDrawBuffersImport(ecs_world_t *world);
