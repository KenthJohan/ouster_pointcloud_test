#pragma once
#include "flecs.h"
#include "HandmadeMath.h"


#define MAX_PARTICLES (512 * 1024)
#define NUM_PARTICLES_EMITTED_PER_FRAME (10)

typedef struct {
    ecs_f32_t speed;
    ecs_i32_t max_num_particles;
    ecs_i32_t cur_num_particles;
    hmm_vec3 * pos;
    hmm_vec3 * vel;
} AppParticlesDesc;


extern ECS_COMPONENT_DECLARE(AppParticlesDesc);

void AppParticlesImport(ecs_world_t *world);
