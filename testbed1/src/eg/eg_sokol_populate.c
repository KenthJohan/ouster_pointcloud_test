#include "eg_sokol_populate.h"
#include "HandmadeMath.h"

/*
void esg_add(esg_drawinfo_t * draw, void * data, int32_t size)
{
    sg_update_buffer(draw->bind.vertex_buffers[1], &(sg_range){
        .ptr = data,
        .size = size
    });

    sg_append_buffer();
}


void esg_draw(esg_drawinfo_t * draw)
{
    sg_begin_default_pass(&draw->pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(draw->pip);
    sg_apply_bindings(&draw->bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
    sg_draw(0, draw->num_elements, draw->num_instances);
    sg_end_pass();
    sg_commit();
}
*/