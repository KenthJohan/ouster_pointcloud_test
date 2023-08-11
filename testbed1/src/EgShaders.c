#include "EgShaders.h"
#include "eg_basics.h"
#include <math.h>
#include <stdio.h>



#define FLOG(...) fprintf(__VA_ARGS__)


ECS_COMPONENT_DECLARE(EgShader);


void SystemCompile(ecs_iter_t *it)
{
	const EgShader *r = ecs_field(it, EgShader, 1);
	for (int i = 0; i < it->count; i ++)
	{
	}
}


void EgShadersImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgShaders);
	ecs_set_name_prefix(world, "Eg");

	ECS_COMPONENT_DEFINE(world, EgShader);

	ecs_struct(world, {
	.entity = ecs_id(EgShader),
	.members = {
	{ .name = "id", .type = ecs_id(ecs_i32_t) },
	}
	});


	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemCompile",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgShader), .inout = EcsInOut },
		},
		.callback = SystemCompile
	});

}
