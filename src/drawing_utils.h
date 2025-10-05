#pragma once

#include <cmath>

#include "defs.h"

void draw_triangle(triangle_t triangle) {
    if (triangles_used >= TRIANGLE_ARRAY_LEN) {
        return;
    }

    triangles[triangles_used][0] = triangle[0];
    triangles[triangles_used][1] = triangle[1];
    triangles[triangles_used][2] = triangle[2];

    triangles_used += 1;
}

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
/// @param z z-coordinate of the origin
/// @param r radius
/// @param centre_colour colour of the circle's centre
/// @param outer_colour colour of the circle's edges
/// @param resolution resolution of the circle, higher value = better result
void draw_filled_circle(float x, float y, float z, float r, colour_t centre_colour, colour_t outer_colour, int resolution) {
    float previous_x = x;
    float previous_y = y + r;

    for (int i = 1; i <= resolution; i++) {
        float angle = ((2.0f * M_PI) / resolution) * i;

        float vertex_x = sinf(angle) * r + x;
        float vertex_y = cosf(angle) * r + y;

        // sgp_draw_point(vertex_x, vertex_y);
        triangle_t triangle;

        triangle[0].x = x;
        triangle[0].y = y;
        triangle[0].z = z;
        triangle[0].colour = centre_colour;
        triangle[1].x = previous_x;
        triangle[1].y = previous_y;
        triangle[1].z = z;
        triangle[1].colour = outer_colour;
        triangle[2].x = vertex_x;
        triangle[2].y = vertex_y;
        triangle[2].z = z;
        triangle[2].colour = outer_colour;

        draw_triangle(triangle);
        // sgp_draw_filled_triangle(x, y, previous_x, previous_y, vertex_x, vertex_y);

        previous_x = vertex_x;
        previous_y = vertex_y;
    }
}
