#include "defs.h"
#include "drawing_utils.h"

#define VECMATH_GENERICS
#include "vecmath.h"

// using namespace vecmath;

// #include "imgui/imgui.h"
// #include "sokol/sokol_imgui.h"

// TODO: Implement index buffers

sg_pipeline pipeline;
sg_buffer vertex_buffer{};

// From https://github.com/floooh/sokol-samples/blob/master/html5/cube-emsc.c
// Calculates a single vector that maps the object space to screen space
static vecmath::mat44_t compute_mvp(float rx, float ry, float rz, int width, int height) {
    vecmath::mat44_t proj = vecmath::mat44_perspective_fov_rh(vecmath::vm_radians(30.0f), (float)width / (float)height, 0.01f, 10.0f);
    vecmath::mat44_t view = vecmath::mat44_look_at_rh(vecmath::vec3(0.0f, 1.5f, 4.0f), vecmath::vec3(0.0f, 0.0f, 0.0f), vecmath::vec3(0.0f, 1.0f, 0.0f));
    vecmath::mat44_t view_proj = vm_mul(view, proj);

    vecmath::mat44_t rxm = vecmath::mat44_rotation_x(vecmath::vm_radians(rx));
    vecmath::mat44_t rym = vecmath::mat44_rotation_y(vecmath::vm_radians(ry));
    vecmath::mat44_t rzm = vecmath::mat44_rotation_z(vecmath::vm_radians(rz));

    vecmath::mat44_t model = vm_mul(rzm, vm_mul(rym, rxm));  // Z * X * Y
    return vm_mul(model, view_proj);
}

typedef struct {
    vecmath::mat44_t mvp;
} vs_params_t;

static struct {
    float rx, ry, rz;
} state;

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
    sg_shader_desc shader_desc{
        .attrs[0].glsl_name = "position",
        .attrs[1].glsl_name = "color0",
        .vertex_func.source = R"(#version 300 es
            uniform mat4 mvp;
            in vec4 position;
            in vec4 color0;
            out vec4 fragment_color;

            void main() {
                fragment_color = color0;
                gl_Position = mvp * position;
            }
            )",
        .vertex_func.entry = "main",
        .fragment_func.source = R"(#version 300 es
            precision mediump float;

            in vec4 fragment_color;
            out vec4 color;

            void main() {
                color = fragment_color;
            }
            )",
        .fragment_func.entry = "main",
        .uniform_blocks[0] = {
            .stage = SG_SHADERSTAGE_VERTEX,
            .size = sizeof(vs_params_t),
            .glsl_uniforms = {
                [0] = {
                    .glsl_name = "mvp",
                    .type = SG_UNIFORMTYPE_MAT4,
                },
            },
        },
    };

    sg_pipeline_desc pipeline_desc{
        .layout = {
            .buffers[0].stride = sizeof(vertex_t),
            .attrs[0].format = SG_VERTEXFORMAT_FLOAT3,
            .attrs[1].format = SG_VERTEXFORMAT_FLOAT4,
        },
        .shader = sg_make_shader(&shader_desc),
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLES,  // SG_PRIMITIVETYPE_TRIANGLES, use SG_PRIMITIVETYPE_LINE_STRIP for wireframeish rendering
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        // .cull_mode = SG_CULLMODE_BACK,
    };
    pipeline = sg_make_pipeline(&pipeline_desc);
}

// Called on every frame of the application.
static void frame(void) {
    // Get current window size.
    int width = sapp_width(), height = sapp_height();
    float ratio = width / (float)height;

    triangles_used = 0;

    // Drawing stuff goes here

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

    colour_t red{
        .r = 1.0f,
        .g = 0.2f,
        .b = 0.0f,
        .a = 1.0f,
    };

    colour_t orange{
        .r = 1.0f,
        .g = 0.5f,
        .b = 0.0f,
        .a = 1.0f,
    };

    colour_t yellow{
        .r = 1.0f,
        .g = 1.0f,
        .b = 0.0f,
        .a = 1.0f,
    };

    colour_t orange_yellow{
        .r = 1.0f,
        .g = 0.75f,
        .b = 0.0f,
        .a = 1.0f,
    };

    colour_t orange_yellow2{
        .r = 1.0f,
        .g = 0.9f,
        .b = 0.0f,
        .a = 1.0f,
    };

    colour_t orange_yellow3{
        .r = 1.0f,
        .g = 0.65f,
        .b = 0.0f,
        .a = 1.0f,
    };

    // triangle_t triangle1;
    // triangle1[0].x = 0.5f;
    // triangle1[0].y = 0.5f;
    // triangle1[0].z = -0.5f;
    // triangle1[0].colour = colour2;
    // triangle1[1].x = -0.3f;
    // triangle1[1].y = -0.3f;
    // triangle1[1].z = -0.5f;
    // triangle1[1].colour = colour1;
    // triangle1[2].x = 0.5f;
    // triangle1[2].y = -0.3f;
    // triangle1[2].z = 0.0f;
    // triangle1[2].colour = colour2;
    // draw_triangle(triangle1);

    // triangle_t triangle2;
    // triangle2[0].x = -0.3f;
    // triangle2[0].y = 0.5f;
    // triangle2[0].z = 0.5f;
    // triangle2[0].colour = colour3;
    // triangle2[1].x = 0.5f;
    // triangle2[1].y = -0.3f;
    // triangle2[1].z = 1.0f;
    // triangle2[1].colour = colour4;
    // triangle2[2].x = -0.3f;
    // triangle2[2].y = -0.3f;
    // triangle2[2].z = 0.5f;
    // triangle2[2].colour = colour3;
    // draw_triangle(triangle2);
    
    draw_cylinder(0, 0, 0.0f, 0.5f, 0.18f, orange_yellow2, orange_yellow, orange_yellow2, orange_yellow3, 96);

    sg_range range{};
    range.ptr = triangles;
    range.size = sizeof(triangles);

    sg_update_buffer(vertex_buffer, range);

    state.ry += 1.0f;
    const vs_params_t vs_params = {.mvp = compute_mvp(state.rx, state.ry, state.rz, width, height)};
    
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

    sg_apply_uniforms(0, SG_RANGE(vs_params));
    sg_draw(0, 3 * triangles_used, 1);

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
    desc.width = 1280;
    desc.height = 720;

    return desc;
}