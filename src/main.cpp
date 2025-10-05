#include "defs.h"
#include "drawing_utils.h"

#include <cmath>

#define VECMATH_GENERICS
#include "vecmath.h"

#include "imgui/imgui.h"
#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

// TODO: Implement index buffers

sg_pipeline pipeline;
sg_buffer vertex_buffer{};

uint64_t last_time;

// From https://github.com/floooh/sokol-samples/blob/master/html5/cube-emsc.c
// Calculates a single vector that maps the object space to screen space
static vecmath::mat44_t compute_mvp(float rx, float ry, float rz, int width, int height) {
    vecmath::mat44_t proj = vecmath::mat44_perspective_fov_rh(vecmath::vm_radians(30.0f), (float)width / (float)height, 0.01f, 10.0f);
    vecmath::mat44_t view = vecmath::mat44_look_at_rh(vecmath::vec3(0.0f, 1.5f, 4.0f), vecmath::vec3(0.0f, 0.0f, 0.0f), vecmath::vec3(0.0f, 1.0f, 0.0f));
    vecmath::mat44_t view_proj = vm_mul(view, proj);

    vecmath::mat44_t rxm = vecmath::mat44_rotation_x(vecmath::vm_radians(rx));
    vecmath::mat44_t rym = vecmath::mat44_rotation_y(vecmath::vm_radians(ry));
    vecmath::mat44_t rzm = vecmath::mat44_rotation_z(vecmath::vm_radians(rz));

    vecmath::mat44_t model = vm_mul(rxm, vm_mul(rym, rzm));  // X * Y * Z
    return vm_mul(model, view_proj);
}

typedef struct {
    vecmath::mat44_t mvp;
} vs_params_t;

static struct {
    // Rotation
    float rx = 0.0f, ry = 0.0f, rz = 0.0f;
    // Rotation speed
    float rsx = 0.0f, rsy = 100.0f, rsz = 0.0f;
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

    simgui_desc_t simgui_desc{
        .logger.func = slog_func,
    };
    simgui_setup(&simgui_desc);

    stm_setup();

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

    last_time = stm_now();

    printf("Finished initialisation\n");
}

// Called on every frame of the application
static void frame(void) {
    // Time stuff
    uint64_t now = stm_now();
    double timediff = stm_sec(stm_diff(now, last_time));
    last_time = now;

    // Get current window size
    int width = sapp_width(), height = sapp_height();
    float ratio = width / (float)height;

    simgui_new_frame({ width, height, sapp_frame_duration(), sapp_dpi_scale() });

    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(350,280), ImGuiCond_FirstUseEver);

    ImGui::Begin("Properties");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("w: %d, h: %d, dpi_scale: %.1f", sapp_width(), sapp_height(), sapp_dpi_scale());
    if (ImGui::Button(sapp_is_fullscreen() ? "Switch to windowed" : "Switch to fullscreen")) {
        sapp_toggle_fullscreen();
    }
    
    ImGui::SeparatorText("Rotation speeds");
    ImGui::SliderFloat("X##x", &state.rsx, 0.0f, 1000.0f);
    ImGui::SliderFloat("Y##x", &state.rsy, 0.0f, 1000.0f);
    ImGui::SliderFloat("Z##x", &state.rsz, 0.0f, 1000.0f);
    
    ImGui::SeparatorText("Rotation");
    ImGui::SliderFloat("X", &state.rx, 0.0f, 360.0f);
    ImGui::SliderFloat("Y", &state.ry, 0.0f, 360.0f);
    ImGui::SliderFloat("Z", &state.rz, 0.0f, 360.0f);

    state.rx += timediff * state.rsx;
    state.ry += timediff * state.rsy;
    state.rz += timediff * state.rsz;

    state.rx = fmod(state.rx, 360.0f);
    state.ry = fmod(state.ry, 360.0f);
    state.rz = fmod(state.rz, 360.0f);

    ImGui::End();

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
    
    draw_cylinder(0, 0, 0.0f, 0.5f, 0.18f, orange_yellow2, orange_yellow, orange_yellow2, orange_yellow3, 96);

    sg_range range{};
    range.ptr = triangles;
    range.size = sizeof(triangles);

    sg_update_buffer(vertex_buffer, range);

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
    
    simgui_render();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
}

// Called when the application is shutting down.
static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void input(const sapp_event* event) {
    simgui_handle_event(event);
}

// Implement application main through Sokol APP.
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    sapp_desc desc{
        .init_cb = init,
        .frame_cb = frame,
        .event_cb = input,
        .cleanup_cb = cleanup,
        .window_title = "3D coin",
        .logger.func = slog_func,
        .high_dpi = true,
        .width = 1280,
        .height = 720,
        .icon.sokol_default = true,
        .enable_clipboard = true,
    };

    return desc;
}