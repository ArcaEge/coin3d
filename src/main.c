#define SOKOL_IMPL
#define SOKOL_GLES3
#include <emscripten/emscripten.h>
#include <stdio.h>

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"

static void init(void) {
    sg_setup(&(sg_desc){.environment = sglue_environment()});
}

static void frame(void) {
    sg_pass_action pass_action = {0};

    sg_begin_pass(&(sg_pass){.action = pass_action, .swapchain = sglue_swapchain()});
    sg_end_pass();
    sg_commit();

}

static void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    printf("Hello from Emscripten!\n");

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 1080,
        .height = 720,
        .window_title = "Gravity Simulator",
    };
}