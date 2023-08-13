#include "EgShaders.h"
#include "eg_basics.h"
#include <math.h>
#include <stdio.h>

#include "sokol_gfx.h"

#include "EgStr.h"
#include "EgFs.h"
#include "EgBasics.h"



#define FLOG(...) fprintf(__VA_ARGS__)


ECS_TAG_DECLARE(EgShaderCompiled);
ECS_COMPONENT_DECLARE(EgShader);


void SystemCompile(ecs_iter_t *it)
{
	EgShader *r = ecs_field(it, EgShader, 1);
	EgText *source_shader_vs = ecs_field(it, EgText, 2);
	EgText *source_shader_fs = ecs_field(it, EgText, 3);
	for (int i = 0; i < it->count; i ++)
	{
		ecs_add(it->world, it->entities[i], EgShaderCompiled);
		printf("source_shader_vs %s\n", source_shader_vs[i].value);
		printf("source_shader_fs %s\n", source_shader_fs[i].value);
		sg_shader_desc desc = {0};
		desc.attrs[0].name = "pos";
		desc.attrs[1].name = "color0";
		desc.attrs[2].name = "inst_pos";
		desc.vs.source = source_shader_vs[i].value;
		desc.vs.entry = "main";
		desc.vs.uniform_blocks[0].size = 64;
		desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
		desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
		desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
		desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
		desc.fs.source = source_shader_fs[i].value;
		desc.fs.entry = "main";
		desc.label = "instancing_shader";
		sg_shader shd = sg_make_shader(&desc);
		r[i].id = shd.id;
	}
}


void EgShadersImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgShaders);
	ecs_set_name_prefix(world, "Eg");
    ECS_IMPORT(world, EgBasics);
    ECS_IMPORT(world, EgFs);
    ECS_IMPORT(world, EgStr);

	ECS_TAG_DEFINE(world, EgShaderCompiled);

	ECS_COMPONENT_DEFINE(world, EgShader);

	ecs_struct(world, {
	.entity = ecs_id(EgShader),
	.members = {
	{ .name = "id", .type = ecs_id(ecs_i32_t) },
	}
	});

	//ECS_SYSTEM(world, SystemCompile, EcsOnUpdate, EgShader, EgText(up(eg.fs.TypeLangGlslVs), eg.fs.File));

	
	ecs_system(world, {
		.entity = ecs_entity(world, {
		.name = "SystemCompile",
		.add = { ecs_dependson(EcsOnUpdate) }
		}),
		.query.filter.terms = {
			{.id = ecs_id(EgShader), .inout = EcsInOut },
			//{.id = ecs_pair(ecs_id(EgText), EgFsFile), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = EgRead },
			{.id = ecs_pair(ecs_id(EgText), EgFsFile), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = EgFsTypeLangGlslVs },
			{.id = ecs_pair(ecs_id(EgText), EgFsFile), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = EgFsTypeLangGlslFs },
			{.id = EgShaderCompiled, .oper=EcsNot }, // Adds this
			//{.id = EgFsFile, .inout = EcsInOut, .src.flags = EcsDown, .src.trav = EgFsTypeLangGlslVs },
			//{.id = EgFsFile, .inout = EcsInOut, .src.flags = EcsUp, .src.trav = EgFsTypeLangGlslFs },
		},
		.callback = SystemCompile
	});
	
	

}
