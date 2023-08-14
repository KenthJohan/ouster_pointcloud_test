#include "EgCameras.h"
#include "HandmadeMath.h"



ECS_COMPONENT_DECLARE(EgCamera);


void SystemMvp(ecs_iter_t *it)
{
	EgCamera *c = ecs_field(it, EgCamera, 1);
	for (int i = 0; i < it->count; i ++)
	{
        hmm_mat4 proj = HMM_Perspective(60.0f, (c[i].w / c[i].h), 0.01f, 50.0f);
        hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 12.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
        hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
        c[i].ry += 60.0f * it->delta_time;
        hmm_mat4 mvp = HMM_MultiplyMat4(view_proj, HMM_Rotate(c[i].ry, HMM_Vec3(0.0f, 1.0f, 0.0f)));
        ecs_os_memcpy_t(&c[i].mvp, &mvp, eg_mat4_t);
	}
}





void EgCamerasImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgCameras);
	ecs_set_name_prefix(world, "Eg");

	ECS_COMPONENT_DEFINE(world, EgCamera);

	ecs_struct(world, {
	.entity = ecs_id(EgCamera),
	.members = {
	{ .name = "mvp", .type = ecs_id(ecs_f32_t), .count = (4*4) },
	{ .name = "ry", .type = ecs_id(ecs_f32_t) },
	{ .name = "w", .type = ecs_id(ecs_f32_t) },
	{ .name = "h", .type = ecs_id(ecs_f32_t) },
	}
	});


	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemMvp",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgCamera), .inout = EcsInOut },
		},
		.callback = SystemMvp
	});

}