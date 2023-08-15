#pragma once
#include "flecs.h"



typedef struct {
	ecs_i32_t id;
} EgShadersProgram;

extern ECS_TAG_DECLARE(EgShadersCompile);
extern ECS_COMPONENT_DECLARE(EgShadersProgram);


void EgShadersImport(ecs_world_t *world);
