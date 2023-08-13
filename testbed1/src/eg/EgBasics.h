#pragma once
#include "flecs.h"


extern ECS_TAG_DECLARE(EgRead);
extern ECS_TAG_DECLARE(EgWrite);
extern ECS_TAG_DECLARE(EgUse);

void EgBasicsImport(ecs_world_t *world);