#pragma once

#ifndef SOKOL_IMPL
#define SOKOL_IMPL
#define SOKOL_GLES3
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#define SOKOL_TIME_IMPL
#endif

#ifndef SOKOL_TIME
#include "sokol/sokol_time.h"
#endif

#define TRIANGLE_ARRAY_LEN 1024

struct colour_t {
    float r, g, b, a;
};

struct vertex_t {
    // Coord
    float x, y, z;
    // Colour
    colour_t colour{};
};

typedef vertex_t triangle_t[3];

// 1K triangle buffer
static triangle_t triangles[TRIANGLE_ARRAY_LEN];
static uint16_t triangles_used = 0;