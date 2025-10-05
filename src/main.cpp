#include "defs.h"
#include "drawing_utils.h"

// #include "imgui/imgui.h"
// #include "sokol/sokol_imgui.h"

// TODO: Implement index buffers

sg_pipeline pipeline;
sg_buffer vertex_buffer{};

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

    // Create vertex buffer
    sg_buffer_usage vertex_buf_usage{};
    vertex_buf_usage.stream_update = true;

    sg_buffer_desc vertex_buf_desc{};
    vertex_buf_desc.size = sizeof(triangles);
    vertex_buf_desc.usage = vertex_buf_usage;

    vertex_buffer = sg_make_buffer(&vertex_buf_desc);

    // Create shader
    sg_shader_desc shader_desc{};
    shader_desc.attrs[0].glsl_name = "position";
    shader_desc.attrs[1].glsl_name = "color0";
    shader_desc.vertex_func.source = R"(#version 300 es
    precision mediump float;

    in vec3 position;
    in vec4 color0;
    out vec4 fragment_color;

    void main() {
        fragment_color = color0;
        gl_Position = vec4(position, 1.0);
    }
    )";
    shader_desc.vertex_func.entry = "main";
    shader_desc.fragment_func.source = R"(#version 300 es
    precision mediump float;

    in vec4 fragment_color;
    out vec4 color;

    void main() {
        color = fragment_color;
    }
    )";
    shader_desc.fragment_func.entry = "main";

    sg_pipeline_desc pipeline_desc{
        .shader = sg_make_shader(&shader_desc),
        .layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3,
        .layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4,
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLES,   // SG_PRIMITIVETYPE_TRIANGLES, use SG_PRIMITIVETYPE_LINE_STRIP for wireframeish rendering
    };
    pipeline = sg_make_pipeline(&pipeline_desc);
}

// Called on every frame of the application.
static void frame(void) {
    // Get current window size.
    int width = sapp_width(), height = sapp_height();
    float ratio = width / (float)height;

    // triangles[0][0].x = -0.1f;
    // triangles[0][0].y = 0.0f;
    // triangles[0][0].colour.r = 0.0f;
    // triangles[0][0].colour.g = 1.0f;
    // triangles[0][0].colour.b = 1.0f;
    // triangles[0][0].colour.a = 0.0f;

    // triangles[0][1].x = 0.5f;
    // triangles[0][1].y = 0.0f;
    // triangles[0][1].colour.r = 0.0f;
    // triangles[0][1].colour.g = 0.0f;
    // triangles[0][1].colour.b = 1.0f;
    // triangles[0][1].colour.a = 0.0f;

    // triangles[0][2].x = 0.0f;
    // triangles[0][2].y = 0.5f;
    // triangles[0][2].colour.r = 0.0f;
    // triangles[0][2].colour.g = 1.0f;
    // triangles[0][2].colour.b = 1.0f;
    // triangles[0][2].colour.a = 0.0f;
    colour_t colour1{};
    colour1.r = 0.0f;
    colour1.g = 1.0f;
    colour1.b = 1.0f;
    colour1.a = 1.0f;

    colour_t colour2{};
    colour2.r = 0.0f;
    colour2.g = 0.0f;
    colour2.b = 1.0f;
    colour2.a = 1.0f;

    draw_filled_circle(0, 0, 0.5f, colour1, colour2, 128);

    sg_range range{};
    range.ptr = triangles;
    range.size = sizeof(triangles);

    sg_update_buffer(vertex_buffer, range);

    // Begin pass
    sg_pass_action pass_action{};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.colors[0].clear_value = {0.1f, 0.1f, 0.1f, 1.0f};

    sg_pass pass{};
    pass.action = pass_action;
    pass.swapchain = sglue_swapchain();

    sg_begin_pass(&pass);
    sg_apply_pipeline(pipeline);

    sg_bindings bindings{};
    *bindings.vertex_buffers = vertex_buffer;
    sg_apply_bindings(&bindings);

    sg_draw(0, 3 * triangles_used, 1);

    // // Begin recording draw commands for a frame buffer of size (width, height).
    // sgp_begin(width, height);
    // // Set frame buffer drawing region to (0,0,width,height).
    // sgp_viewport(0, 0, width, height);
    // // Set drawing coordinate space to (left=-ratio, right=ratio, top=1, bottom=-1).
    // sgp_project(-ratio, ratio, 1.0f, -1.0f);

    // // Clear the frame buffer.
    // sgp_set_color(0.1f, 0.1f, 0.1f, 1.0f);
    // sgp_clear();

    // // Draw an animated rectangle that rotates and changes its colors.
    // float time = sapp_frame_count() * sapp_frame_duration();
    // float r = sinf(time) * 0.5 + 0.5, g = cosf(time) * 0.5 + 0.5;
    // sgp_set_color(r, g, 0.8f, 1.0f);
    // // sgp_draw_filled_rect(-0.5f, -0.5f, 1.0f, 1.0f);
    // sgp_draw_point(0, 0);
    // // sgp_rotate_at(time, 0.0f, 0.0f);

    // // Begin a render pass.
    // sg_pass pass = {.swapchain = sglue_swapchain()};
    // sg_begin_pass(&pass);
    // // Dispatch all draw commands to Sokol GFX.
    // sgp_flush();
    // // Finish a draw command queue, clearing it.
    // sgp_end();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
}

// Called when the application is shutting down.
static void cleanup(void) {
    sg_shutdown();
}

// Implement application main through Sokol APP.
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    sapp_desc desc{};

    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.cleanup_cb = cleanup;
    desc.window_title = "3D coin";
    desc.logger.func = slog_func;
    desc.html5_canvas_resize = true;
    desc.high_dpi = true;
    desc.width = 720;
    desc.height = 720;

    return desc;
}