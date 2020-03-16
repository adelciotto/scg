// Simple Computer Graphics (SCG) by Anthony Del Ciotto
//
// Copyright © 2020 Anthony Del Ciotto
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE file for more details.
//
// ABOUT:
//
// My personal WIP single header file library for quick and simple
// cross-platform interactive applications.

#ifndef INCLUDE_SCG_H
#define INCLUDE_SCG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define float32_t float
#define float64_t double

#define SCG_PI 3.1415926535f

#define scg_log_error(...)                                                     \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_warn(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_info(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

typedef struct scg_error_t {
    bool none;
    const char *message;
} scg_error_t;

#define scg_error_new(error_msg) ((scg_error_t){false, error_msg})
#define scg_error_none() ((scg_error_t){true, ""})

extern int scg_min_int(int val, int min);
extern int scg_max_int(int val, int max);
extern float32_t scg_min_float32(float32_t val, float32_t min);
extern float32_t scg_max_float32(float32_t val, float32_t max);

extern int scg_round_float32(float32_t val);
extern float32_t scg_clamp_float32(float32_t val, float32_t min, float32_t max);

extern uint64_t scg_get_performance_counter(void);
extern uint64_t scg_get_performance_frequency(void);
extern float64_t scg_get_elapsed_time_secs(uint64_t end, uint64_t start);
extern float64_t scg_get_elapsed_time_millisecs(uint64_t end, uint64_t start);

typedef union scg_pixel_t {
    uint32_t packed;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } data;
} scg_pixel_t;

#define scg_pixel_new_rgb(r, g, b) ((scg_pixel_t){.data = {(r), (g), (b), 255}})
#define scg_pixel_new_rgba(r, g, b, a)                                         \
    ((scg_pixel_t){.data = {(r), (g), (b), (a)}})
#define scg_pixel_new_uint32(value) ((scg_pixel_t){.packed = (value)})
#define scg_pixel_index_from_xy(x, y, width) ((int)((y) * (width) + (x)))

#define SCG_COLOR_WHITE scg_pixel_new_rgb(255, 255, 255)
#define SCG_COLOR_BLACK scg_pixel_new_rgb(0, 0, 0)
#define SCG_COLOR_RED scg_pixel_new_rgb(255, 0, 0)
#define SCG_COLOR_GREEN scg_pixel_new_rgb(0, 255, 0)
#define SCG_COLOR_BLUE scg_pixel_new_rgb(0, 0, 255)
#define SCG_COLOR_YELLOW scg_pixel_new_rgb(255, 255, 0)
#define SCG_COLOR_95_GREEN scg_pixel_new_rgb(0, 128, 128);
#define SCG_COLOR_ICE_BLUE scg_pixel_new_rgb(153, 255, 255);
#define SCG_COLOR_SKY_BLUE scg_pixel_new_rgb(135, 206, 235);

typedef enum scg_blend_mode_t {
    SCG_BLEND_MODE_NONE,
    SCG_BLEND_MODE_MASK,
    SCG_BLEND_MODE_ALPHA
} scg_blend_mode_t;

typedef struct scg_image_t {
    int width;
    int height;
    int pitch;
    SDL_PixelFormat *pixel_format;
    uint32_t *pixels;
    scg_blend_mode_t blend_mode;
} scg_image_t;

typedef struct scg_frame_metrics_t {
    int target_fps;
    float64_t frame_time_secs;
    float64_t frame_time_millisecs;
    float64_t fps;
} scg_frame_metrics_t;

extern scg_error_t scg_image_new(scg_image_t *image, int width, int height);
extern scg_error_t scg_image_new_from_bmp(scg_image_t *image,
                                          const char *filepath);
extern void scg_image_set_blend_mode(scg_image_t *image,
                                     scg_blend_mode_t blend_mode);
extern scg_pixel_t scg_image_get_pixel(scg_image_t *image, int x, int y);
extern void scg_image_set_pixel(scg_image_t *image, int x, int y,
                                scg_pixel_t color);
extern void scg_image_clear(scg_image_t *image, scg_pixel_t color);
extern void scg_image_draw_image(scg_image_t *dest, scg_image_t *src, int x,
                                 int y);
extern void scg_image_draw_image_transform(scg_image_t *dest, scg_image_t *src,
                                           int x, int y, float32_t angle,
                                           float32_t sx, float32_t sy);
extern void scg_image_draw_line(scg_image_t *image, int x0, int y0, int x1,
                                int y1, scg_pixel_t color);
extern void scg_image_draw_rect(scg_image_t *image, int x, int y, int w, int h,
                                scg_pixel_t color);
extern void scg_image_fill_rect(scg_image_t *image, int x, int y, int w, int h,
                                scg_pixel_t color);
extern void scg_image_draw_char(scg_image_t *image, char ch, int x, int y,
                                scg_pixel_t color);
extern void scg_image_draw_string(scg_image_t *image, const char *str, int x,
                                  int y, bool anchor_to_center,
                                  scg_pixel_t color);
extern void scg_image_draw_frame_metrics(scg_image_t *image,
                                         scg_frame_metrics_t frame_metrics);
extern void scg_image_destroy(scg_image_t *image);

extern scg_error_t scg_init(void);
extern void scg_quit(void);

typedef struct scg_screen_t scg_screen_t;

scg_error_t scg_screen_new(scg_screen_t *screen, const char *title,
                           scg_image_t *back_buffer, int window_scale,
                           bool fullscreen);
extern bool scg_screen_is_running(scg_screen_t *screen);
extern void scg_screen_present(scg_screen_t *screen);
extern void scg_screen_log_info(scg_screen_t *screen);
extern void scg_screen_close(scg_screen_t *screen);
extern void scg_screen_destroy(scg_screen_t *screen);

typedef struct scg_sound_device_t scg_sound_device_t;

extern scg_error_t scg_sound_device_new(scg_sound_device_t *sound_device,
                                        int target_fps);
extern void scg_sound_device_log_info(scg_sound_device_t *sound_device);
extern void scg_sound_device_update(scg_sound_device_t *sound_device);
extern void scg_sound_device_destroy(scg_sound_device_t *sound_device);

typedef struct scg_sound_t scg_sound_t;

extern scg_error_t scg_sound_new_from_wav(scg_sound_device_t *sound_device,
                                          scg_sound_t *sound,
                                          const char *filepath, bool loop);
extern void scg_sound_play(scg_sound_t *sound);
extern float32_t scg_sound_get_position(scg_sound_t *sound);

typedef enum scg_key_code_t {
    SCG_KEY_UP = SDL_SCANCODE_UP,
    SCG_KEY_DOWN = SDL_SCANCODE_DOWN,
    SCG_KEY_LEFT = SDL_SCANCODE_LEFT,
    SCG_KEY_RIGHT = SDL_SCANCODE_RIGHT,
    SCG_KEY_X = SDL_SCANCODE_X,
    SCG_KEY_C = SDL_SCANCODE_C,
    SCG_KEY_Z = SDL_SCANCODE_Z,
    SCG_KEY_SPACE = SDL_SCANCODE_SPACE,
    SCG_KEY_ESCAPE = SDL_SCANCODE_ESCAPE
} scg_key_code_t;

typedef struct scg_keyboard_t scg_keyboard_t;

extern void scg_keyboard_new(scg_keyboard_t *keyboard);
extern bool scg_keyboard_is_key_down(scg_keyboard_t *keyboard,
                                     scg_key_code_t code);
extern bool scg_keyboard_is_key_up(scg_keyboard_t *keyboard,
                                   scg_key_code_t code);
extern bool scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
                                          scg_key_code_t code);
extern void scg_keyboard_update(scg_keyboard_t *keyboard);

///////////////
//
// Everything below here is implementation details
//

#define SCG_MAX_SOUNDS 16 // TODO: use linked list for sounds?

struct scg_screen_t {
    const char *title;
    int width;
    int height;
    int window_scale;

    int target_fps;
    float64_t target_frame_time_secs;
    uint64_t last_frame_counter;
    float64_t frame_metrics_update_counter;
    scg_frame_metrics_t frame_metrics;
    bool is_running;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
    scg_image_t *back_buffer;
};

struct scg_sound_t {
    int device_id;
    SDL_AudioSpec sdl_spec;
    uint32_t length;
    uint8_t *buffer;
    uint32_t play_offset;
    uint8_t *end_position;
    bool is_playing;
    bool loop;
};

struct scg_sound_device_t {
    SDL_AudioDeviceID device_id;
    int frequency;
    uint8_t num_channels;
    uint16_t num_samples;
    int bytes_per_sample;
    int latency_sample_count;
    uint32_t buffer_size;
    uint8_t *buffer;

    scg_sound_t *sounds[SCG_MAX_SOUNDS];
    int num_sounds;
};

struct scg_keyboard_t {
    const uint8_t *current_key_states;
    uint8_t previous_key_states[SDL_NUM_SCANCODES];
};

#ifdef __cplusplus
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#ifdef SCG_IMPLEMENTATION

#include <math.h>
#include <stdlib.h>

#define SCG_DEFAULT_REFRESH_RATE 60
#define SCG_FONT_SIZE 8

extern const char scg__font8x8[128][SCG_FONT_SIZE];

static const char *scg__sprintf(const char *fmt, ...) {
    static char buffer[1024];
    va_list v;

    va_start(v, fmt);
    vsnprintf(buffer, 1024, fmt, v);
    va_end(v);

    return buffer;
}

//
// scg_min_int implementation
//

int scg_min_int(int val, int min) {
    return val < min ? val : min;
}

//
// scg_max_int implementation
//

int scg_max_int(int val, int max) {
    return val > max ? val : max;
}

//
// scg_min_float32 implementation
//

float32_t scg_min_float32(float32_t val, float32_t min) {
    return val < min ? val : min;
}

//
// scg_max_float32 implementation
//

float32_t scg_max_float32(float32_t val, float32_t max) {
    return val > max ? val : max;
}

//
// scg_round_float32 implementation
//

int scg_round_float32(float32_t val) {
    return (int)(val + 0.5f);
}

//
// scg_clamp_float32 implementation
//

float32_t scg_clamp_float32(float32_t val, float32_t min, float32_t max) {
    const float32_t t = val < min ? min : val;
    return t > max ? max : t;
}

//
// scg_get_performance_counter implementation
//

Uint64 scg_get_performance_counter(void) {
    return SDL_GetPerformanceCounter();
}

//
// scg_get_performance_frequency implementation
//

Uint64 scg_get_performance_frequency(void) {
    return SDL_GetPerformanceFrequency();
}

//
// scg_get_elapsed_time_secs implementation
//

float64_t scg_get_elapsed_time_secs(uint64_t end, uint64_t start) {
    return (float64_t)(end - start) /
           (float64_t)scg_get_performance_frequency();
}

//
// scg_get_elapsed_time_millisecs implementation
//

float64_t scg_get_elapsed_time_millisecs(uint64_t end, uint64_t start) {
    return (float64_t)((end - start) * 1000) /
           (float64_t)scg_get_performance_frequency();
}

//
// scg_image_new implementation
//

scg_error_t scg_image_new(scg_image_t *image, int width, int height) {
    uint32_t *pixels = calloc(width * height, sizeof(*pixels));
    if (pixels == NULL) {
        return scg_error_new("Failed to allocate memory for image");
    }

    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    if (pixel_format == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to allocate pixel format. %s", SDL_GetError());
        return scg_error_new(error_msg);
    }

    image->width = width;
    image->height = height;
    image->pitch = width * sizeof(*pixels);
    image->pixel_format = pixel_format;
    image->pixels = pixels;
    image->blend_mode = SCG_BLEND_MODE_NONE;

    return scg_error_none();
}

//
// scg_image_new_from_bmp implementation
//

scg_error_t scg_image_new_from_bmp(scg_image_t *image, const char *filepath) {
    SDL_Surface *surface = SDL_LoadBMP(filepath);
    if (surface == NULL) {
        const char *error_msg = scg__sprintf("Failed to load %s image file. %s",
                                             filepath, SDL_GetError());
        return scg_error_new(error_msg);
    }

    // Convert the surface to RGBA32 pixel format.
    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    if (pixel_format == NULL) {
        const char *error_msg = scg__sprintf(
            "Failed to allocate pixel format for %s image file. %s", filepath,
            SDL_GetError());
        return scg_error_new(error_msg);
    }
    SDL_Surface *converted_surface =
        SDL_ConvertSurface(surface, pixel_format, 0);
    if (converted_surface == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to convert surface for %s image file. %s",
                         filepath, SDL_GetError());
        return scg_error_new(error_msg);
    }

    // We no longer need the original surface.
    SDL_FreeSurface(surface);

    int surface_w = converted_surface->w;
    int surface_h = converted_surface->h;
    int surface_pitch = converted_surface->pitch;
    uint32_t *surface_pixels = (uint32_t *)converted_surface->pixels;

    // Allocate new pixel buffer and copy over the converted surface pixel data.
    uint32_t *pixels = malloc(surface_pitch * surface_h);
    if (pixels == NULL) {
        const char *error_msg = scg__sprintf(
            "Failed to allocate memory for %s image file", filepath);
        return scg_error_new(error_msg);
    }
    memcpy(pixels, surface_pixels, surface_pitch * surface_h);

    // Assign all the image properties.
    image->width = surface_w;
    image->height = surface_h;
    image->pitch = surface_pitch;
    image->pixel_format = pixel_format;
    image->pixels = pixels;
    image->blend_mode = SCG_BLEND_MODE_NONE;

    // We no longer need the converted surface.
    SDL_FreeSurface(converted_surface);

    return scg_error_none();
}

//
// scg_image_set_blend_mode
//

void scg_image_set_blend_mode(scg_image_t *image, scg_blend_mode_t blend_mode) {
    image->blend_mode = blend_mode;
}

//
// scg_image_get_pixel implementation
//

scg_pixel_t scg_image_get_pixel(scg_image_t *image, int x, int y) {
    int w = image->width;
    int h = image->height;

    // Return black pixel if outside of image bounds.
    if (x < 0 || x >= w) {
        return SCG_COLOR_BLACK;
    }
    if (y < 0 || y >= h) {
        return SCG_COLOR_BLACK;
    }

    int i = scg_pixel_index_from_xy(x, y, w);
    return scg_pixel_new_uint32(image->pixels[i]);
}

//
// scg_image_set_pixel implementation
//

void scg_image_set_pixel(scg_image_t *image, int x, int y, scg_pixel_t color) {
    int w = image->width;
    int h = image->height;

    if (x < 0 || x >= w) {
        return;
    }
    if (y < 0 || y >= h) {
        return;
    }

    scg_blend_mode_t blend_mode = image->blend_mode;
    int i = scg_pixel_index_from_xy(x, y, w);

    if (blend_mode == SCG_BLEND_MODE_NONE) {
        image->pixels[i] = color.packed;
    }

    if (blend_mode == SCG_BLEND_MODE_MASK) {
        if (color.data.a == 255) {
            image->pixels[i] = color.packed;
        }
    }

    if (blend_mode == SCG_BLEND_MODE_ALPHA) {
        scg_pixel_t d = scg_pixel_new_uint32(image->pixels[i]);
        float32_t a = (float32_t)(color.data.a / 255.0f);
        float32_t c = 1.0f - a;
        float32_t r = a * (float32_t)color.data.r + c * (float32_t)d.data.r;
        float32_t g = a * (float32_t)color.data.g + c * (float32_t)d.data.g;
        float32_t b = a * (float32_t)color.data.b + c * (float32_t)d.data.b;

        scg_pixel_t blended_color =
            scg_pixel_new_rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
        image->pixels[i] = blended_color.packed;
    }
}

//
// scg_image_clear implementation
//

void scg_image_clear(scg_image_t *image, scg_pixel_t color) {
    int num_pixels = image->width * image->height;
    uint32_t pixel = color.packed;

    for (int i = 0; i < num_pixels; i++) {
        image->pixels[i] = pixel;
    }
}

//
// scg_image_draw_image implementation
//

void scg_image_draw_image(scg_image_t *dest, scg_image_t *src, int x, int y) {
    int src_w = src->width;
    int src_h = src->height;

    for (int i = 0; i < src_h; i++) {
        for (int j = 0; j < src_w; j++) {
            scg_pixel_t color = scg_image_get_pixel(src, j, i);
            scg_image_set_pixel(dest, x + j, y + i, color);
        }
    }
}

//
// scg_image_draw_image_transform implementation
//

void scg_image_draw_image_transform(scg_image_t *dest, scg_image_t *src, int x,
                                    int y, float32_t angle, float32_t sx,
                                    float32_t sy) {
    if (sx <= 0.0f)
        sx = 1.0f;
    if (sy <= 0.0f)
        sy = 1.0f;

    float32_t src_w = src->width;
    float32_t src_h = src->height;
    float32_t src_sw = src_w * sx;
    float32_t src_sh = src_h * sy;

    float32_t ratio_x = src_w / src_sw;
    float32_t ratio_y = src_h / src_sh;
    float32_t origin_x = src_w * 0.5f;
    float32_t origin_y = src_h * 0.5f;

    float32_t s = sinf(-angle);
    float32_t c = cosf(-angle);

    int x0 = (int)(-src_sw * c + src_sh * s);
    int y0 = (int)(-src_sw * s - src_sh * c);
    int x1 = (int)(src_sw * c + src_sh * s);
    int y1 = (int)(src_sw * s - src_sh * c);
    int x2 = (int)(src_sw * c - src_sh * s);
    int y2 = (int)(src_sw * s + src_sh * c);
    int x3 = (int)(-src_sw * c - src_sh * s);
    int y3 = (int)(-src_sw * s + src_sh * c);

    int minx =
        scg_min_int(0, scg_min_int(scg_min_int(x0, scg_min_int(x1, x2)), x3));
    int miny =
        scg_min_int(0, scg_min_int(scg_min_int(y0, scg_min_int(y1, y2)), y3));
    int maxx = scg_max_int(x0, scg_max_int(x1, scg_max_int(x2, x3)));
    int maxy = scg_max_int(y0, scg_max_int(y1, scg_max_int(y2, y3)));

    for (int i = miny; i < maxy; i++) {
        for (int j = minx; j < maxx; j++) {
            float32_t image_x = (float32_t)j * ratio_x - origin_x;
            float32_t image_y = (float32_t)i * ratio_y - origin_y;
            float32_t x_rot = (image_x * c - image_y * s) + origin_x;
            float32_t y_rot = (image_x * s + image_y * c) + origin_y;

            if (x_rot >= 0 && x_rot < src_w && y_rot >= 0 && y_rot < src_h) {
                scg_pixel_t color =
                    scg_image_get_pixel(src, (int)x_rot, (int)y_rot);
                scg_image_set_pixel(dest, x + j, y + i, color);
            }
        }
    }
}

//
// scg_image_draw_line implementation
//

void scg_image_draw_line(scg_image_t *image, int x0, int y0, int x1, int y1,
                         scg_pixel_t color) {
    // Line is vertical
    if (x0 == x1) {
        if (y1 < y0) {
            int tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        for (int y = y0; y <= y1; y++) {
            scg_image_set_pixel(image, x0, y, color);
        }

        return;
    }

    // Line is horizontal
    if (y0 == y1) {
        if (x1 < x0) {
            int tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        for (int x = x0; x <= x1; x++) {
            scg_image_set_pixel(image, x, y0, color);
        }

        return;
    }

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int step_x = x0 < x1 ? 1 : -1;
    int step_y = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2 = 0;

    for (;;) {
        scg_image_set_pixel(image, x0, y0, color);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += step_x;
        }
        if (e2 < dy) {
            err += dx;
            y0 += step_y;
        }
    }
}

//
// scg_image_draw_rect implementation
//

void scg_image_draw_rect(scg_image_t *image, int x, int y, int w, int h,
                         scg_pixel_t color) {
    int minx = x;
    int miny = y;
    int maxx = minx + w;
    int maxy = miny + h;

    scg_image_draw_line(image, minx, miny, maxx, miny, color);
    scg_image_draw_line(image, maxx, miny, maxx, maxy, color);
    scg_image_draw_line(image, maxx, maxy, minx, maxy, color);
    scg_image_draw_line(image, minx, maxy, minx, miny, color);
}

//
// scg_image_fill_rect implementation
//

void scg_image_fill_rect(scg_image_t *image, int x, int y, int w, int h,
                         scg_pixel_t color) {
    for (int i = 0; i <= h; i++) {
        for (int j = 0; j <= w; j++) {
            scg_image_set_pixel(image, x + j, y + i, color);
        }
    }
}

//
// scg_image_draw_char implementation
//

void scg_image_draw_char(scg_image_t *image, char ch, int x, int y,
                         scg_pixel_t color) {
    int char_code = (int)ch;
    if (char_code < 0 || char_code > 127) {
        char_code = 63; // draw '?' for unknown char
    }

    const char *bitmap = scg__font8x8[char_code];

    for (int i = 0; i < SCG_FONT_SIZE; i++) {
        for (int j = 0; j < SCG_FONT_SIZE; j++) {
            int set = bitmap[i] & 1 << j;

            if (set) {
                scg_image_set_pixel(image, x + j, y + i, color);
            }
        }
    }
}

static int scg__string_width(const char *str, int size) {
    return strlen(str) * size;
}

//
// scg_image_draw_string implementation
//

void scg_image_draw_string(scg_image_t *image, const char *str, int x, int y,
                           bool anchor_to_center, scg_pixel_t color) {
    int current_x = x;
    int current_y = y;
    int horizontal_spacing = 1;

    if (anchor_to_center) {
        int width = scg__string_width(str, SCG_FONT_SIZE + horizontal_spacing);
        current_x = x - width / 2;
        current_y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        scg_image_draw_char(image, str[i], current_x, current_y, color);
        current_x += SCG_FONT_SIZE + horizontal_spacing;
    }
}

//
// scg_image_draw_frame_metrics implementation
//

void scg_image_draw_frame_metrics(scg_image_t *image,
                                  scg_frame_metrics_t frame_metrics) {
    float32_t fps = frame_metrics.fps;
    float32_t frame_time_ms = frame_metrics.frame_time_millisecs;
    const char *fmt = "fps:%.2f ms/f:%.4f";

    ssize_t bsize = snprintf(NULL, 0, fmt, fps, frame_time_ms);
    char buffer[bsize];
    snprintf(buffer, bsize + 1, fmt, fps, frame_time_ms);

    scg_pixel_t color = SCG_COLOR_GREEN;
    float32_t target_fps = (float32_t)frame_metrics.target_fps;
    if (fps < target_fps * 0.95) {
        color = SCG_COLOR_YELLOW;
    }
    if (fps < target_fps * 0.5) {
        color = SCG_COLOR_RED;
    }

    // Draw the metrics with no alpha blending.
    scg_blend_mode_t blend_mode = image->blend_mode;
    scg_image_set_blend_mode(image, SCG_BLEND_MODE_NONE);
    scg_image_draw_string(image, buffer, 10, 10, 0, color);
    scg_image_set_blend_mode(image, blend_mode);
}

//
// scg_image_destroy implementation
//

void scg_image_destroy(scg_image_t *image) {
    SDL_FreeFormat(image->pixel_format);
    free(image->pixels);
}

//
// scg_init implementation
//

scg_error_t scg_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        return scg_error_new(SDL_GetError());
    }

    return scg_error_none();
}

//
// scg_quit implementation
//

void scg_quit(void) {
    SDL_Quit();
}

static int scg__get_monitor_refresh_rate(SDL_DisplayMode display_mode) {
    int result = display_mode.refresh_rate;

    if (result == 0) {
        return SCG_DEFAULT_REFRESH_RATE;
    }

    return result;
}

//
// scg_screen_new implementation
//

scg_error_t scg_screen_new(scg_screen_t *screen, const char *title,
                           scg_image_t *back_buffer, int window_scale,
                           bool fullscreen) {
    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
        return scg_error_new(SDL_GetError());
    }

    int w = back_buffer->width;
    int h = back_buffer->height;

    SDL_Window *sdl_window = SDL_CreateWindow(
        title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w * window_scale,
        h * window_scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL) {
        return scg_error_new(SDL_GetError());
    }

    if (fullscreen) {
        SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(
        sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) {
        return scg_error_new(SDL_GetError());
    }

    SDL_RenderSetLogicalSize(sdl_renderer, w, h);

    SDL_Texture *sdl_texture =
        SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32,
                          SDL_TEXTUREACCESS_STREAMING, w, h);
    if (sdl_texture == NULL) {
        return scg_error_new(SDL_GetError());
    }

    screen->title = title;
    screen->width = w;
    screen->height = h;
    screen->window_scale = window_scale;
    screen->target_fps = scg__get_monitor_refresh_rate(display_mode);
    screen->target_frame_time_secs = 1.0 / (float64_t)screen->target_fps;
    screen->last_frame_counter = scg_get_performance_counter();
    screen->sdl_window = sdl_window;
    screen->sdl_renderer = sdl_renderer;
    screen->sdl_texture = sdl_texture;
    screen->back_buffer = back_buffer;
    screen->frame_metrics_update_counter = scg_get_performance_counter();
    screen->frame_metrics.target_fps = screen->target_fps;
    screen->is_running = true;

    return scg_error_none();
}

//
// scg_screen_is_running implementation
//

bool scg_screen_is_running(scg_screen_t *screen) {
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        screen->is_running = false;
        break;
    }

    return screen->is_running;
}

//
// scg_screen_present implementation
//

void scg_screen_present(scg_screen_t *screen) {
    // Wait until we have reached the target amount of time per frame (e.g 60hz,
    // ~16ms). Spinning in a while loop seems to be the most accurate way to do
    // this, as trying to use SDL_Delay (sleeping) is dependant on other
    // factors.
    {
        float64_t target_secs = screen->target_frame_time_secs;
        float64_t elapsed_time_secs = scg_get_elapsed_time_secs(
            scg_get_performance_counter(), screen->last_frame_counter);

        if (elapsed_time_secs < target_secs) {
            while (elapsed_time_secs < target_secs) {
                elapsed_time_secs = scg_get_elapsed_time_secs(
                    scg_get_performance_counter(), screen->last_frame_counter);
            }
        }
    }

    uint64_t end_frame_counter = scg_get_performance_counter();

    SDL_UpdateTexture(screen->sdl_texture, NULL, screen->back_buffer->pixels,
                      screen->back_buffer->pitch);
    SDL_RenderClear(screen->sdl_renderer);
    SDL_RenderCopy(screen->sdl_renderer, screen->sdl_texture, NULL, NULL);
    SDL_RenderPresent(screen->sdl_renderer);

    // Update the frame metrics every second.
    {
        float64_t elapsed_time_secs =
            scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                      screen->frame_metrics_update_counter);
        if (elapsed_time_secs >= 1.0f) {
            screen->frame_metrics.frame_time_secs = scg_get_elapsed_time_secs(
                end_frame_counter, screen->last_frame_counter);
            screen->frame_metrics.frame_time_millisecs =
                scg_get_elapsed_time_millisecs(end_frame_counter,
                                               screen->last_frame_counter);
            screen->frame_metrics.fps =
                (float64_t)scg_get_performance_frequency() /
                (float64_t)(end_frame_counter - screen->last_frame_counter);

            screen->frame_metrics_update_counter =
                scg_get_performance_counter();
        }
    }

    screen->last_frame_counter = end_frame_counter;
}

//
// scg_screen_log_info implementation
//

void scg_screen_log_info(scg_screen_t *screen) {
    scg_log_info("screen has w:%d, h:%d, scale:%d, target fps:%d",
                 screen->width, screen->height, screen->window_scale,
                 screen->target_fps);
}

//
// scg_screen_close implementation
//

void scg_screen_close(scg_screen_t *screen) {
    screen->is_running = false;
}

//
// scg_screen_destroy implementation
//

void scg_screen_destroy(scg_screen_t *screen) {
    SDL_DestroyTexture(screen->sdl_texture);
    SDL_DestroyRenderer(screen->sdl_renderer);
    SDL_DestroyWindow(screen->sdl_window);
}

//
// scg_sound_device_new implementation
//

scg_error_t scg_sound_device_new(scg_sound_device_t *sound_device,
                                 int target_fps) {
    SDL_AudioSpec desired, obtained;

    int desired_num_channels = 2;
    size_t bytes_per_sample = sizeof(int16_t) * desired_num_channels;

    memset(&desired, 0, sizeof(desired));
    desired.freq = 48000;
    desired.format = AUDIO_S16LSB;
    desired.channels = desired_num_channels;
    desired.samples = desired.freq * bytes_per_sample / target_fps;
    desired.callback = NULL;

    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(
        NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (device_id == 0) {
        const char *error_msg = scg__sprintf(
            "Failed to open audio device. Error %s", SDL_GetError());
        return scg_error_new(error_msg);
    }

    if (obtained.format != desired.format) {
        return scg_error_new("Audio device does not support format S16LSB");
    }

    int obtained_frequency = obtained.freq;
    int latency_sample_count = obtained_frequency / 15;
    uint32_t buffer_size = latency_sample_count * bytes_per_sample;

    uint8_t *buffer = calloc(latency_sample_count, bytes_per_sample);
    if (buffer == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to allocate memory for sound buffer. bytes=%d",
                         sound_device->latency_sample_count * bytes_per_sample);
        return scg_error_new(error_msg);
    }

    sound_device->device_id = device_id;
    sound_device->frequency = obtained_frequency;
    sound_device->num_channels = obtained.channels;
    sound_device->num_samples = obtained.samples;
    sound_device->bytes_per_sample = bytes_per_sample;
    sound_device->latency_sample_count = latency_sample_count;
    sound_device->buffer_size = buffer_size;
    sound_device->buffer = buffer;
    sound_device->num_sounds = 0;

    SDL_PauseAudioDevice(device_id, 0);

    return scg_error_none();
}

//
// scg_sound_device_log_info implementation
//

void scg_sound_device_log_info(scg_sound_device_t *sound_device) {
    scg_log_info("sound device has id:%d, channels:%d, samples/sec:%d, "
                 "samples/frame:%d, bytes/sample:%d",
                 sound_device->device_id, sound_device->num_channels,
                 sound_device->frequency, sound_device->latency_sample_count,
                 sound_device->bytes_per_sample);
}

//
// scg_sound_device_update implementation
//

void scg_sound_device_update(scg_sound_device_t *sound_device) {
    memset(sound_device->buffer, 0, sound_device->buffer_size);

    uint32_t target_queue_bytes = sound_device->buffer_size;
    uint32_t bytes_to_write =
        target_queue_bytes - SDL_GetQueuedAudioSize(sound_device->device_id);

    for (int i = 0; i < sound_device->num_sounds; i++) {
        scg_sound_t *sound = sound_device->sounds[i];

        if (!sound->is_playing) {
            continue;
        }

        const uint8_t *current_play_position =
            sound->buffer + sound->play_offset;
        const uint32_t remaining_bytes_to_play =
            sound->length - sound->play_offset;

        if (current_play_position >= sound->end_position) {
            if (!sound->loop) {
                sound->is_playing = false;
            }

            sound->play_offset = 0;
        } else {
            uint32_t bytes_to_mix = (bytes_to_write > remaining_bytes_to_play)
                                        ? remaining_bytes_to_play
                                        : bytes_to_write;

            SDL_MixAudioFormat(sound_device->buffer, current_play_position,
                               AUDIO_S16LSB, bytes_to_mix,
                               SDL_MIX_MAXVOLUME / 2);

            sound->play_offset += bytes_to_mix;
        }
    }

    SDL_QueueAudio(sound_device->device_id, sound_device->buffer,
                   bytes_to_write);
}

//
// scg_sound_device_destroy_sounds implementation
//

void scg_sound_device_destroy(scg_sound_device_t *sound_device) {
    SDL_PauseAudioDevice(sound_device->device_id, 1);
    SDL_ClearQueuedAudio(sound_device->device_id);

    for (int i = 0; i < sound_device->num_sounds; i++) {
        SDL_FreeWAV(sound_device->sounds[i]->buffer);
    }

    free(sound_device->buffer);
    SDL_CloseAudioDevice(sound_device->device_id);
}

//
// scg_sound_new implementation
//

scg_error_t scg_sound_new_from_wav(scg_sound_device_t *sound_device,
                                   scg_sound_t *sound, const char *filepath,
                                   bool loop) {
    if (sound_device->num_sounds == SCG_MAX_SOUNDS) {
        return scg_error_new("Maximum sounds reached");
    }

    SDL_AudioSpec spec;
    uint32_t length;
    uint8_t *buffer;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == NULL) {
        const char *error_msg = scg__sprintf(
            "Failed to load WAV file %s. Error %s", filepath, SDL_GetError());
        return scg_error_new(error_msg);
    }

    sound->sdl_spec = spec;
    sound->length = length;
    sound->buffer = buffer;
    sound->play_offset = 0;
    sound->end_position = buffer + length;
    sound->is_playing = false;
    sound->loop = loop;

    sound_device->sounds[sound_device->num_sounds++] = sound;

    return scg_error_none();
}

//
// scg_sound_play implementation
//

void scg_sound_play(scg_sound_t *sound) {
    if (!sound->is_playing) {
        sound->is_playing = true;
    }
}

//
// scg_sound_get_position implementation
//

float32_t scg_sound_get_position(scg_sound_t *sound) {
    if (!sound->is_playing) {
        return 0.0f;
    }

    return (float32_t)sound->play_offset / (float32_t)sound->length;
}

//
// scg_keyboard_new implementation
//

void scg_keyboard_new(scg_keyboard_t *keyboard) {
    keyboard->current_key_states = SDL_GetKeyboardState(NULL);
    memset(keyboard->previous_key_states, 0,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);
}

//
// scg_keyboard_is_key_down implementation
//

bool scg_keyboard_is_key_down(scg_keyboard_t *keyboard, scg_key_code_t key) {
    return keyboard->current_key_states[key] == 1;
}

//
// scg_keyboard_is_key_up implementation
//

bool scg_keyboard_is_key_up(scg_keyboard_t *keyboard, scg_key_code_t key) {
    return keyboard->current_key_states[key] == 0;
}

//
// scg_keyboard_is_key_triggered implementation
//

bool scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
                                   scg_key_code_t key) {
    return keyboard->previous_key_states[key] == 0 &&
           keyboard->current_key_states[key] == 1;
}

//
// scg_keyboard_update implementation
//

void scg_keyboard_update(scg_keyboard_t *keyboard) {
    memcpy(keyboard->previous_key_states, keyboard->current_key_states,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);
}

// Bitmap font data taken from https://github.com/dhepper/font8x8.
const char scg__font8x8[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0000 (nul)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0001
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0002
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0003
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0004
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0005
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0006
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0007
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0008
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0009
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+000F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0010
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0011
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0012
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0013
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0014
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0015
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0016
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0017
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0018
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0019
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+001F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},       // U+0021 (!)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0022 (")
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},       // U+0023 (#)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},       // U+0024 ($)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},       // U+0025 (%)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},       // U+0026 (&)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0027 (')
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},       // U+0028 (()
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},       // U+0029 ())
    {0x00, 0x66, 0x3C, (char)0xFF, 0x3C, 0x66, 0x00, 0x00}, // U+002A (*)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},       // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},       // U+002C (,)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},       // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},       // U+002E (.)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},       // U+002F (/)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},       // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},       // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},       // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},       // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},       // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},       // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},       // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},       // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},       // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},       // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},       // U+003A (:)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},       // U+003B (;)
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},       // U+003C (<)
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},       // U+003D (=)
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},       // U+003E (>)
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},       // U+003F (?)
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},       // U+0040 (@)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},       // U+0041 (A)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},       // U+0042 (B)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},       // U+0043 (C)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},       // U+0044 (D)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},       // U+0045 (E)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},       // U+0046 (F)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},       // U+0047 (G)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},       // U+0048 (H)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},       // U+0049 (I)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},       // U+004A (J)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},       // U+004B (K)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},       // U+004C (L)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},       // U+004D (M)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},       // U+004E (N)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},       // U+004F (O)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},       // U+0050 (P)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},       // U+0051 (Q)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},       // U+0052 (R)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},       // U+0053 (S)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},       // U+0054 (T)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},       // U+0055 (U)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},       // U+0056 (V)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},       // U+0057 (W)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},       // U+0058 (X)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},       // U+0059 (Y)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},       // U+005A (Z)
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},       // U+005B ([)
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},       // U+005C (\)
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},       // U+005D (])
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},       // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (char)0xFF}, // U+005F (_)
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},       // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},       // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},       // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},       // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},       // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},       // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},       // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},       // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},       // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},       // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},       // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},       // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},       // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},       // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},       // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},       // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},       // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},       // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},       // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},       // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},       // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},       // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},       // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},       // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},       // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},       // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},       // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},       // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},       // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // U+007E (~)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}        // U+007F
};

#endif // SCG_IMPLEMENTATION
