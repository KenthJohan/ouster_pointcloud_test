#include "EgDrawBuffers.h"
#include "sokol_gfx.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(EgDrawBuffer);


int EgDrawBuffer_append(EgDrawBuffer * buf, void * data, ecs_i32_t count)
{
	buf->count += count;
	if(buf->count > buf->cap)
	{
		buf->count -= count;
		return 0;
	}
	sg_range r = {data, count * buf->esize};
	sg_append_buffer(buf->buffer, &r);
	return count;
}


void EgDrawBuffer_OnSet(ecs_iter_t *it)
{
	printf("EgDrawBuffer_OnSet\n");
	EgDrawBuffer *b = ecs_field(it, EgDrawBuffer, 1);
	for (int i = 0; i < it->count; i ++)
	{
		b[i].buffer = sg_make_buffer(&(sg_buffer_desc){
			.size = b[i].cap * b[i].esize,
			.usage = SG_USAGE_STREAM,
			.label = "instance-data"
		});
		const float r = 0.05f;
		const float vertices[] = {
			// positions            colors
			0.0f,   -r, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
			r, 0.0f, r,          0.0f, 1.0f, 0.0f, 1.0f,
			r, 0.0f, -r,         0.0f, 0.0f, 1.0f, 1.0f,
			-r, 0.0f, -r,         1.0f, 1.0f, 0.0f, 1.0f,
			-r, 0.0f, r,          0.0f, 1.0f, 1.0f, 1.0f,
			0.0f,    r, 0.0f,       1.0f, 0.0f, 1.0f, 1.0f
		};
		b[i].buffer_verts = sg_make_buffer(&(sg_buffer_desc){
			.data = SG_RANGE(vertices),
			.label = "geometry-vertices"
		});
		// index buffer for static geometry
		const uint16_t indices[] = {
			0, 1, 2,    0, 2, 3,    0, 3, 4,    0, 4, 1,
			5, 1, 2,    5, 2, 3,    5, 3, 4,    5, 4, 1
		};
		b[i].buffer_index = sg_make_buffer(&(sg_buffer_desc){
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(indices),
			.label = "geometry-indices"
		});
	}
}




void EgDrawBuffersImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgDrawBuffers);
	ecs_set_name_prefix(world, "Eg");

	ECS_COMPONENT_DEFINE(world, EgDrawBuffer);

	ecs_struct(world, {
	.entity = ecs_id(EgDrawBuffer),
	.members = {
	{ .name = "cap", .type = ecs_id(ecs_i32_t) },
	{ .name = "esize", .type = ecs_id(ecs_i32_t) },
	{ .name = "count", .type = ecs_id(ecs_i32_t) },
	{ .name = "buffer", .type = ecs_id(ecs_i32_t) },
	{ .name = "buffer_verts", .type = ecs_id(ecs_i32_t) },
	{ .name = "buffer_index", .type = ecs_id(ecs_i32_t) },
	}
	});

    ecs_observer(world, {
		.filter.terms = { 
			// Use EcsSelf to prevent matching instances of this
			{.id = ecs_id(EgDrawBuffer), .src.flags = EcsSelf },
		},
        .events = { EcsOnSet },
        .callback = EgDrawBuffer_OnSet,
    });
}