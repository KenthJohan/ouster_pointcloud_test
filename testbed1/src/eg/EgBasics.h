#pragma once
#include "flecs.h"

typedef struct {
    ecs_i32_t id;
} EgExternalId;

extern ECS_COMPONENT_DECLARE(EgExternalId);
extern ECS_TAG_DECLARE(EgRead);
extern ECS_TAG_DECLARE(EgWrite);
extern ECS_TAG_DECLARE(EgUse);

void EgBasicsImport(ecs_world_t *world);