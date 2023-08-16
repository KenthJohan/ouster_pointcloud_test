#include "AppParticles.h"
#include "EgBasics.h"
#include "EgDrawBuffers.h"
#include "EgQuantities.h"
#include "sokol_gfx.h"
#include <stdio.h>


ECS_COMPONENT_DECLARE(AppParticlesDesc);




void SystemParticleEmit(ecs_iter_t *it)
{
	AppParticlesDesc *particles_pos = ecs_field(it, AppParticlesDesc, 1);
	EgVelocity_V3F32 *particles_vel = ecs_field(it, EgVelocity_V3F32, 2);
	for (int j = 0; j < it->count; j ++)
	{
		AppParticlesDesc * p = particles_pos + j;
		EgVelocity_V3F32 * v = particles_vel + j;
		// emit new particles
		for (int i = 0; i < NUM_PARTICLES_EMITTED_PER_FRAME; i++)
		{
			if (p->cur_num_particles < p->max_num_particles)
			{
				p->pos[p->cur_num_particles] = HMM_Vec3(0.0, 0.0, 0.0);
				p->vel[p->cur_num_particles] = HMM_Vec3(
					((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f,
					((float)(rand() & 0x7FFF) / 0x7FFF) * 0.5f + 2.0f,
					((float)(rand() & 0x7FFF) / 0x7FFF) - 0.5f);
				p->vel[p->cur_num_particles].X *= v->dx;
				p->vel[p->cur_num_particles].Y *= v->dy;
				p->vel[p->cur_num_particles].Z *= v->dz;
				p->cur_num_particles++;
			}
			else
			{
				break;
			}
		}
	}
}


void SystemParticleUpdate(ecs_iter_t *it)
{
	AppParticlesDesc *particles = ecs_field(it, AppParticlesDesc, 1);
	for (int j = 0; j < it->count; j ++)
	{
		AppParticlesDesc * p = particles + j;
		// update particle positions
		for (int i = 0; i < p->cur_num_particles; i++)
		{
			p->vel[i].Y -= 1.0f * it->delta_time;
			p->pos[i].X += p->vel[i].X * it->delta_time;
			p->pos[i].Y += p->vel[i].Y * it->delta_time;
			p->pos[i].Z += p->vel[i].Z * it->delta_time;
			// bounce back from 'ground'
			if (p->pos[i].Y < -2.0f)
			{
				p->pos[i].Y = -1.8f;
				p->vel[i].Y = -p->vel[i].Y;
				p->vel[i].X *= 0.8f;
				p->vel[i].Y *= 0.8f;
				p->vel[i].Z *= 0.8f;
			}
		}
	}
}


void SystemParticleAllocate(ecs_iter_t *it)
{
	printf("SystemParticleAllocate\n");
	AppParticlesDesc *particles = ecs_field(it, AppParticlesDesc, 1);
	for (int j = 0; j < it->count; j ++)
	{
        particles[j].cur_num_particles = 0;
        ecs_i32_t n = ECS_MIN(MAX_PARTICLES, particles[j].max_num_particles);
        particles[j].max_num_particles = n;
        particles[j].pos = ecs_os_calloc(n * sizeof(hmm_vec3));
        particles[j].vel = ecs_os_calloc(n * sizeof(hmm_vec3));
	}
}


void SystemAppendBuffer(ecs_iter_t *it)
{
	while (ecs_query_next(it))
	{
		//printf("SystemAppendBuffer %s\n", ecs_get_name(it->world, ecs_field_src(it, 1)));
		AppParticlesDesc *f1_particles = ecs_field(it, AppParticlesDesc, 1); // EcsSelf
		EgDrawBuffer *f2_buffer = ecs_field(it, EgDrawBuffer, 2); // EcsUp
		ecs_assert(ecs_field_is_self(it, 1) == true, ECS_INVALID_OPERATION, 0);
		ecs_assert(ecs_field_is_self(it, 2) == false, ECS_INVALID_OPERATION, 0);
		ecs_assert(sizeof(hmm_vec3) == f2_buffer->esize, ECS_INVALID_OPERATION, 0);
		f2_buffer->count = 0;
		for (int i = 0; i < it->count; i ++)
		{
			AppParticlesDesc *p = f1_particles + i;
			EgDrawBuffer_append(f2_buffer, p->pos, p->cur_num_particles);
		}
	}
}


void AppParticlesImport(ecs_world_t *world)
{
	ECS_MODULE(world, AppParticles);
	ecs_set_name_prefix(world, "App");
	ECS_IMPORT(world, EgBasics);
	ECS_IMPORT(world, EgDrawBuffers);
	ECS_IMPORT(world, EgQuantities);

	ECS_COMPONENT_DEFINE(world, AppParticlesDesc);

	ecs_struct(world, {
	.entity = ecs_id(AppParticlesDesc),
	.members = {
	{ .name = "max_num_particles", .type = ecs_id(ecs_i32_t) },
	{ .name = "cur_num_particles", .type = ecs_id(ecs_i32_t) },
	{ .name = "pos", .type = ecs_id(ecs_uptr_t) },
	{ .name = "vel", .type = ecs_id(ecs_uptr_t) },
	}
	});

    ECS_SYSTEM(world, SystemParticleEmit, EcsOnUpdate, AppParticlesDesc, EgVelocity_V3F32);
    ECS_SYSTEM(world, SystemParticleUpdate, EcsOnUpdate, AppParticlesDesc);
    ECS_OBSERVER(world, SystemParticleAllocate, EcsOnSet, AppParticlesDesc);



	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemAppendBuffer",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(AppParticlesDesc), .inout = EcsInOut },
			{.id = ecs_id(EgDrawBuffer), .inout = EcsInOut, .src.flags = EcsUp },
		},
		.run = SystemAppendBuffer
	});



	/*
	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemAppendBuffer",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(AppParticlesDesc), .inout = EcsInOut, .src.flags = EcsUp | EcsCascade, .src.trav = ecs_id(EgUse) },
		},
		.callback = SystemAppendBuffer
	});
	*/
}
