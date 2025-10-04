#define SOKOL_IMPL
#define SOKOL_GLES3
#include <emscripten/emscripten.h>

#include <cstdio>

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_gp.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"
#include "imgui/imgui.h"
#include "sokol/sokol_imgui.h"

// Needs to be defined after
#include "drawing_utils.h"

// Called on every frame of the application.
static void frame(void) {
    // Get current window size.
    int width = sapp_width(), height = sapp_height();
    float ratio = width / (float)height;

    // Begin recording draw commands for a frame buffer of size (width, height).
    sgp_begin(width, height);
    // Set frame buffer drawing region to (0,0,width,height).
    sgp_viewport(0, 0, width, height);
    // Set drawing coordinate space to (left=-ratio, right=ratio, top=1, bottom=-1).
    sgp_project(-ratio, ratio, 1.0f, -1.0f);

    // Clear the frame buffer.
    sgp_set_color(0.1f, 0.1f, 0.1f, 1.0f);
    sgp_clear();

    // Draw an animated rectangle that rotates and changes its colors.
    float time = sapp_frame_count() * sapp_frame_duration();
    float r = sinf(time) * 0.5 + 0.5, g = cosf(time) * 0.5 + 0.5;
    sgp_set_color(r, g, 0.8f, 1.0f);
    // sgp_draw_filled_rect(-0.5f, -0.5f, 1.0f, 1.0f);
    draw_filled_circle(0, 0, 0.2f, 64);
    sgp_draw_point(0, 0);
    // sgp_rotate_at(time, 0.0f, 0.0f);

    // Begin a render pass.
    sg_pass pass = {.swapchain = sglue_swapchain()};
    sg_begin_pass(&pass);
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
    // Finish a draw command queue, clearing it.
    sgp_end();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
}

// Called when the application is initializing.
static void init(void) {
    // Initialize Sokol GFX.
    sg_desc sgdesc{};
    sgdesc.environment = sglue_environment();
    sgdesc.logger.func = slog_func;

    sg_setup(&sgdesc);
    if (!sg_isvalid()) {
        fprintf(stderr, "Failed to create Sokol GFX context!\n");
        exit(-1);
    }

    // Initialize Sokol GP, adjust the size of command buffers for your own use.
    sgp_desc sgpdesc = {0};
    sgp_setup(&sgpdesc);
    if (!sgp_is_valid()) {
        fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
        exit(-1);
    }
}

// Called when the application is shutting down.
static void cleanup(void) {
    // Cleanup Sokol GP and Sokol GFX resources.
    sgp_shutdown();
    sg_shutdown();
}

// Implement application main through Sokol APP.
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Did it this way instead of intialising directly because dEpReCaTiOn wArNiNg -_-
    sapp_desc desc{};

    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.cleanup_cb = cleanup;
    desc.window_title = "Rectangle (Sokol GP)";
    desc.logger.func = slog_func;
    desc.html5_canvas_resize = true;
    desc.high_dpi = true;
    desc.width = 1280;
    desc.height = 720;

    return desc;
}