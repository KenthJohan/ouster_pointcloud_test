#include "EgBasics.h"




ECS_COMPONENT_DECLARE(EgExternalId);

ECS_TAG_DECLARE(EgRead);
ECS_TAG_DECLARE(EgWrite);
ECS_TAG_DECLARE(EgUse);

void EgBasicsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgBasics);
	ecs_set_name_prefix(world, "Eg");


	ECS_COMPONENT_DEFINE(world, EgExternalId);
	ECS_TAG_DEFINE(world, EgRead);
	ECS_TAG_DEFINE(world, EgWrite);
	ECS_TAG_DEFINE(world, EgUse);


    ecs_add_id(world, EgRead, EcsTraversable);
    ecs_add_id(world, EgWrite, EcsTraversable);
    ecs_add_id(world, EgUse, EcsTraversable);

	ecs_struct(world, {
	.entity = ecs_id(EgExternalId),
	.members = {
	{ .name = "id", .type = ecs_id(ecs_i32_t) },
	}
	});

}
