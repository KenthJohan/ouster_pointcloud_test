#include "EgBasics.h"
#include "EgFetcher.h"
#include "EgFs.h"
#include "EgStr.h"
#include "sokol_fetch.h"

#include "sokol_log.h"

typedef struct 
{
	ecs_world_t * world;
	ecs_entity_t e;
} userdata_t;


static void fetch_callback(const sfetch_response_t* response)
{
    if (response->fetched)
	{
		userdata_t * user_data = (userdata_t *)response->user_data;
		char const * rs = response->buffer.ptr;
		ecs_i32_t size = response->buffer.size;
		ecs_set_pair(user_data->world, user_data->e, EgText, EgFsFile, {rs, size});
    }
	else if (response->failed)
	{

    }
}


void System_Send(ecs_iter_t *it)
{
	EgText *path = ecs_field(it, EgText, 1);
	EgFsSize *file_size = ecs_field(it, EgFsSize, 2);
	for (int i = 0; i < it->count; i ++)
	{
		userdata_t user_data = {it->world, it->entities[i]};
		ecs_i32_t size = file_size[i].size + 1;
		char * memory = ecs_os_calloc(size);
		sfetch_send(&(sfetch_request_t){
			.path = path[i].value,
			.callback = fetch_callback,
			.buffer = (sfetch_range_t){ memory, size },
			.user_data = (sfetch_range_t){ &user_data, sizeof(userdata_t) },
		});
	}
}

void System_Print(ecs_iter_t *it)
{
	//EgText *path = ecs_field(it, EgText, 4);
	for (int i = 0; i < it->count; i ++)
	{
		//printf("text :\n %s\n\n\n", path[i].value);
	}
}

void System_Pump(ecs_iter_t *it)
{
    sfetch_dowork();
}



void EgFetcherImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgFetcher);
	ecs_set_name_prefix(world, "Eg");

    sfetch_setup(&(sfetch_desc_t){
        .max_requests = 4,
        .num_channels = 3,
        .num_lanes = 3,
        .logger.func = slog_func,
    });

	ECS_SYSTEM(world, System_Pump, EcsOnUpdate, 0);

	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "System_Send",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_pair(ecs_id(EgText), EgFsPath) },
			{.id = ecs_id(EgFsSize) },
			{.id = EgFsFile },
			{.id = ecs_pair(ecs_id(EgText), EgFsFile), .oper=EcsNot }, // Adds this
		},
		.callback = System_Send
	});


	ecs_observer(world, {
		.entity = ecs_entity(world, {
		.name = "System_Print",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.filter.terms = {
			{.id = ecs_pair(ecs_id(EgText), EgFsPath) },
			{.id = ecs_id(EgFsSize) },
			{.id = EgFsFile },
			{.id = ecs_pair(ecs_id(EgText), EgFsFile) }, // Adds this
		},
		.events = { EcsOnSet },
		.callback = System_Print,
	});

}
