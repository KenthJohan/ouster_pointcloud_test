#pragma once
#include "flecs.h"



typedef struct {
	ecs_i32_t id;
} EgShader;

extern ECS_TAG_DECLARE(EgShaderCompiled);
extern ECS_COMPONENT_DECLARE(EgShader);


void EgShadersImport(ecs_world_t *world);
