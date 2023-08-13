#include "EgBasics.h"


ECS_TAG_DECLARE(EgRead);
ECS_TAG_DECLARE(EgWrite);


void EgBasicsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgBasics);
	ecs_set_name_prefix(world, "Eg");


	ECS_TAG_DEFINE(world, EgRead);
	ECS_TAG_DEFINE(world, EgWrite);


    ecs_add_id(world, EgRead, EcsTraversable);
    ecs_add_id(world, EgWrite, EcsTraversable);

}
