#pragma once

#include <cmath>

#ifndef SOKOL_IMPL
#define SOKOL_IMPL
#define SOKOL_GLES3
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_gp.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_imgui.h"
#endif

/// @brief Draws a circle outline
/// @param x x-coordinate of the origin
/// @param y y-coordinate of the origin
/// @param r inner radius
/// @param t outline thickness
/// @param resolution resolution of the circle, higher value = better result
void draw_circle_outline(float x, float y, float r, float t, int resolution) {
}

/// @brief Draws a filled circle
/// @param x x-coordinate of the origin
/// @param y y-coordinate of the origin
/// @param r radius
/// @param resolution resolution of the circle, higher value = better result
void draw_filled_circle(float x, float y, float r, int resolution) {
    float previous_x = x;
    float previous_y = y + r;
    for (int i = 1; i <= resolution; i++) {
        float angle = ((2.0f * M_PI) / resolution) * i;
        
        float vertex_x = sinf(angle) * r + x;
        float vertex_y = cosf(angle) * r + y;

        // sgp_draw_point(vertex_x, vertex_y);
        
        sgp_draw_filled_triangle(x, y, previous_x, previous_y, vertex_x, vertex_y);

        previous_x = vertex_x;
        previous_y = vertex_y;
    }
}