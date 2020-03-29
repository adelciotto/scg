// Simple Computer Graphics (SCG) by Anthony Del Ciotto
//
// Copyright © 2020 Anthony Del Ciotto
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE file for more details.
//
// ABOUT:
// My personal single header file library for quick and simple
// cross-platform graphics applications.
//
// Version: 0.0.1 (WIP)
//
// Do this:
// #define SCG_IMPLEMENTATION
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// // i.e. it should look like this:
// #include ...
// #include ...
// #include ...
// #define SCG_IMPLEMENTATION
// #include "scg.h"
//
// View any of the examples for demonstration of usage.

#ifndef INCLUDE_SCG_H
#define INCLUDE_SCG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define float32_t float
#define float64_t double

#define SCG_PI 3.1415926535f

#define SCG_FONT_SIZE 8

#define scg_log_errorf(FMT, ...)                                               \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s() in %s, line %i: " FMT,    \
                 __func__, __FILE__, __LINE__, __VA_ARGS__)
#define scg_log_error(MSG) scg_log_errorf("%s", MSG)
#define scg_log_warnf(FMT, ...)                                                \
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s() in %s, line %i: " FMT,     \
                __func__, __FILE__, __LINE__, __VA_ARGS__)
#define scg_log_warn(MSG) scg_log_warnf("%s", MSG)
#define scg_log_infof(FMT, ...)                                                \
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s() in %s, line %i: " FMT,     \
                __func__, __FILE__, __LINE__, __VA_ARGS__)
#define scg_log_info(MSG) scg_log_infof("%s", MSG)

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
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    struct {
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } data;
#else
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } data;
#endif
} scg_pixel_t;

#define scg_pixel_new_rgb(R, G, B)                                             \
    ((scg_pixel_t){.data = {.r = (R), .g = (G), .b = (B), .a = 255}})
#define scg_pixel_new_rgba(R, G, B, A)                                         \
    ((scg_pixel_t){.data = {.r = (R), .g = (G), .b = (B), .a = (A)}})
#define scg_pixel_new_uint32(VALUE) ((scg_pixel_t){.packed = (VALUE)})
#define scg_pixel_index_from_xy(X, Y, WIDTH) ((int)((Y) * (WIDTH) + (X)))

#define SCG_COLOR_WHITE scg_pixel_new_rgb(255, 255, 255)
#define SCG_COLOR_GRAY scg_pixel_new_rgb(128, 128, 128)
#define SCG_COLOR_BLACK scg_pixel_new_rgb(0, 0, 0)
#define SCG_COLOR_RED scg_pixel_new_rgb(255, 0, 0)
#define SCG_COLOR_GREEN scg_pixel_new_rgb(0, 255, 0)
#define SCG_COLOR_BLUE scg_pixel_new_rgb(0, 0, 255)
#define SCG_COLOR_YELLOW scg_pixel_new_rgb(255, 255, 0)
#define SCG_COLOR_MAGENTA scg_pixel_new_rgb(255, 0, 255)
#define SCG_COLOR_95_GREEN scg_pixel_new_rgb(0, 128, 128)
#define SCG_COLOR_ICE_BLUE scg_pixel_new_rgb(153, 255, 255)
#define SCG_COLOR_SKY_BLUE scg_pixel_new_rgb(135, 206, 235)

typedef enum scg_blend_mode_t {
    SCG_BLEND_MODE_NONE,
    SCG_BLEND_MODE_MASK,
    SCG_BLEND_MODE_ALPHA
} scg_blend_mode_t;

typedef struct scg_image_t {
    int width;
    int height;
    int pitch;
    uint32_t *pixels;
    scg_blend_mode_t blend_mode;
} scg_image_t;

typedef struct scg_frame_metrics_t {
    int target_fps;
    float64_t frame_time_secs;
    float64_t frame_time_millisecs;
    int fps;
} scg_frame_metrics_t;

extern scg_image_t *scg_image_new(int width, int height);
extern scg_image_t *scg_image_new_from_bmp(const char *filepath);
extern void scg_image_set_blend_mode(scg_image_t *image,
                                     scg_blend_mode_t blend_mode);
extern scg_pixel_t scg_image_get_pixel(scg_image_t *image, int x, int y);
extern void scg_image_set_pixel(scg_image_t *image, int x, int y,
                                scg_pixel_t color);
extern void scg_image_clear(scg_image_t *image, scg_pixel_t color);
extern void scg_image_draw_image(scg_image_t *dest, scg_image_t *src, int x,
                                 int y);
extern void scg_image_draw_image_rotate(scg_image_t *image, scg_image_t *src,
                                        int x, int y, float32_t angle);
extern void scg_image_draw_image_rotate_scale(scg_image_t *dest,
                                              scg_image_t *src, int x, int y,
                                              float32_t angle, float32_t sx,
                                              float32_t sy);
extern void scg_image_draw_line(scg_image_t *image, int x0, int y0, int x1,
                                int y1, scg_pixel_t color);
extern void scg_image_draw_rect(scg_image_t *image, int x, int y, int w, int h,
                                scg_pixel_t color);
extern void scg_image_fill_rect(scg_image_t *image, int x, int y, int w, int h,
                                scg_pixel_t color);
extern void scg_image_draw_char(scg_image_t *image, char char_code, int x,
                                int y, scg_pixel_t color);
extern void scg_image_draw_string(scg_image_t *image, const char *str, int x,
                                  int y, bool anchor_to_center,
                                  scg_pixel_t color);
extern void scg_image_draw_wchar(scg_image_t *image, wchar_t char_code, int x,
                                 int y, scg_pixel_t color);
extern void scg_image_draw_wstring(scg_image_t *image, const wchar_t *str,
                                   int x, int y, bool anchor_to_center,
                                   scg_pixel_t color);
extern void scg_image_draw_frame_metrics(scg_image_t *image,
                                         scg_frame_metrics_t frame_metrics);
extern bool scg_image_save_to_bmp(scg_image_t *image, const char *filepath);
extern void scg_image_free(scg_image_t *image);

#define SCG__MAX_SOUNDS 16

typedef struct scg_sound_t {
    int device_id;
    SDL_AudioSpec sdl_spec;
    uint32_t length;
    uint8_t *buffer;
    uint32_t play_offset;
    uint8_t *end_position;
    bool is_playing;
    bool loop;
} scg_sound_t;

typedef struct scg_audio_t {
    SDL_AudioDeviceID device_id;
    int frequency;
    uint8_t num_channels;
    uint16_t num_samples;
    int bytes_per_sample;
    int latency_sample_count;
    uint32_t buffer_size;
    uint8_t *buffer;

    scg_sound_t *sounds[SCG__MAX_SOUNDS]; // TODO: use linked list for sounds?
    int num_sounds;
} scg_audio_t;

extern scg_sound_t *scg_sound_new_from_wav(scg_audio_t *audio,
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

typedef struct scg_keyboard_t {
    const uint8_t *current_key_states;
    uint8_t previous_key_states[SDL_NUM_SCANCODES];
} scg_keyboard_t;

extern bool scg_keyboard_is_key_down(scg_keyboard_t *keyboard,
                                     scg_key_code_t code);
extern bool scg_keyboard_is_key_up(scg_keyboard_t *keyboard,
                                   scg_key_code_t code);
extern bool scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
                                          scg_key_code_t code);

typedef struct scg_config_t {
    struct {
        int width;
        int height;
        const char *title;
        int scale;
        bool fullscreen;
        bool vsync;
        bool show_frame_metrics;
    } video;

    struct {
        bool enabled;
        bool text_input;
    } input;

    struct {
        bool enabled;
        int volume;
    } audio;
} scg_config_t;

extern scg_config_t scg_config_new_default(void);

typedef struct scg__screen_t {
    int target_fps;
    float64_t target_frame_time_secs;
    uint64_t last_frame_counter;
    float64_t frame_metrics_update_counter;
    scg_frame_metrics_t frame_metrics;
    bool vsync;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
} scg__screen_t;

typedef struct scg_app_t {
    bool running;
    float32_t delta_time;
    scg_config_t config;
    scg_image_t *draw_target;
    scg_keyboard_t *keyboard;
    scg_audio_t *audio;

    uint64_t delta_time_counter;
    scg__screen_t *screen;
} scg_app_t;

extern void scg_app_init(scg_app_t *app, scg_config_t config);
extern void scg_app_begin_frame(scg_app_t *app);
extern void scg_app_end_frame(scg_app_t *app);
extern void scg_app_shutdown(scg_app_t *app);

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_SCG_H

//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#ifdef SCG_IMPLEMENTATION

#include <math.h>
#include <stdlib.h>

#define SCG__DEFAULT_REFRESH_RATE 60

#define SCG__FONT_NUM_CHARS 128
#define SCG__FONT_CHAR_CODE_START 0
#define SCG__FONT_CHAR_CODE_END 127
#define SCG__FONT_CHAR_CODE_SPACE 32
#define SCG__FONT_CHAR_CODE_QUESTION_MARK 63
#define SCG__FONT_HIRAGANA_NUM_CHARS 96
#define SCG__FONT_HIRAGANA_CHAR_CODE_START 0x3040
#define SCG__FONT_HIRAGANA_CHAR_CODE_END 0x309F

#define SCG__IMAGE_PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888

#define SCG__MAX_VOLUME SDL_MIX_MAXVOLUME

static const char scg__font8x8[SCG__FONT_NUM_CHARS][SCG_FONT_SIZE];
static const char scg__font8x8_hiragana[SCG__FONT_HIRAGANA_NUM_CHARS]
                                       [SCG_FONT_SIZE];

static scg__screen_t *scg__screen_new(scg_image_t *draw_target,
                                      const char *title, int scale,
                                      bool fullscreen, bool vsync);
static void scg__screen_present(scg__screen_t *screen,
                                scg_image_t *draw_target);
static void scg__screen_free(scg__screen_t *screen);

static scg_keyboard_t *scg__keyboard_new(void);
static void scg__keyboard_update(scg_keyboard_t *keyboard);

static scg_audio_t *scg__audio_new(int target_fps);
static void scg__audio_update(scg_audio_t *audio);
static void scg__audio_free(scg_audio_t *audio);

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

scg_image_t *scg_image_new(int width, int height) {
    uint32_t *pixels = calloc(width * height, sizeof(*pixels));
    if (pixels == NULL) {
        scg_log_error("Failed to allocate memory for pixels");

        return NULL;
    }

    scg_image_t *image = malloc(sizeof(*image));
    if (image == NULL) {
        scg_log_error("Failed to allocate memory for image");

        free(pixels);
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->pitch = width * sizeof(*pixels);
    image->pixels = pixels;
    image->blend_mode = SCG_BLEND_MODE_NONE;

    return image;
}

//
// scg_image_new_from_bmp implementation
//

scg_image_t *scg_image_new_from_bmp(const char *filepath) {
    SDL_Surface *surface = SDL_LoadBMP(filepath);
    if (surface == NULL) {
        scg_log_errorf("Failed to load image file at %s. %s", filepath,
                       SDL_GetError());

        return NULL;
    }

    // Convert the surface to RGBA32 pixel format.
    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SCG__IMAGE_PIXEL_FORMAT);
    if (pixel_format == NULL) {
        scg_log_errorf(
            "Failed to allocate pixel format for image file at %s. %s",
            filepath, SDL_GetError());

        SDL_FreeSurface(surface);
        return NULL;
    }
    SDL_Surface *converted_surface =
        SDL_ConvertSurface(surface, pixel_format, 0);
    if (converted_surface == NULL) {
        scg_log_errorf("Failed to convert surface for image file at %s. %s",
                       filepath, SDL_GetError());

        SDL_FreeFormat(pixel_format);
        SDL_FreeSurface(surface);
        return NULL;
    }

    // We no longer need the original surface.
    SDL_FreeFormat(pixel_format);
    SDL_FreeSurface(surface);

    int surface_w = converted_surface->w;
    int surface_h = converted_surface->h;
    int surface_pitch = converted_surface->pitch;
    uint32_t *surface_pixels = (uint32_t *)converted_surface->pixels;

    // Allocate new pixel buffer and copy over the converted surface pixel data.
    uint32_t *pixels = malloc(surface_pitch * surface_h);
    if (pixels == NULL) {
        scg_log_errorf("Failed to allocate memory for image file at %s",
                       filepath);

        SDL_FreeSurface(converted_surface);
        return NULL;
    }
    memcpy(pixels, surface_pixels, surface_pitch * surface_h);

    scg_image_t *image = malloc(sizeof(*image));
    if (image == NULL) {
        scg_log_error("Failed to allocate memory for image");

        free(pixels);
        SDL_FreeSurface(converted_surface);
        return NULL;
    }

    // Assign all the image properties.
    image->width = surface_w;
    image->height = surface_h;
    image->pitch = surface_pitch;
    image->pixels = pixels;
    image->blend_mode = SCG_BLEND_MODE_NONE;

    // We no longer need the converted surface.
    SDL_FreeSurface(converted_surface);

    return image;
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

    if (x < 0 || x >= w || y < 0 || y >= h) {
        return SCG_COLOR_MAGENTA;
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

    if (x < 0 || x >= w || y < 0 || y >= h) {
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
// scg_image_draw_image_rotate implementation
//

void scg_image_draw_image_rotate(scg_image_t *dest, scg_image_t *src, int x,
                                 int y, float32_t angle) {
    float32_t src_w = src->width;
    float32_t src_h = src->height;
    float32_t origin_x = src_w * 0.5f;
    float32_t origin_y = src_h * 0.5f;
    float32_t sin_theta = sinf(-angle);
    float32_t cos_theta = cosf(-angle);

    float32_t a = src_w * cos_theta;
    float32_t b = src_h * cos_theta;
    float32_t c = src_w * sin_theta;
    float32_t d = src_h * sin_theta;
    float32_t e = -src_w * cos_theta;
    float32_t f = -src_w * sin_theta;
    int x0 = (int)(e + d);
    int y0 = (int)(f - b);
    int x1 = (int)(a + d);
    int y1 = (int)(c - b);
    int x2 = (int)(a - d);
    int y2 = (int)(c + b);
    int x3 = (int)(e - d);
    int y3 = (int)(f + b);

    int minx =
        scg_min_int(0, scg_min_int(scg_min_int(x0, scg_min_int(x1, x2)), x3));
    int miny =
        scg_min_int(0, scg_min_int(scg_min_int(y0, scg_min_int(y1, y2)), y3));
    int maxx = scg_max_int(x0, scg_max_int(x1, scg_max_int(x2, x3)));
    int maxy = scg_max_int(y0, scg_max_int(y1, scg_max_int(y2, y3)));

    // TODO: Try a pixel subsampling approach to improve the visual quality
    // of the rotation.
    // Reference:
    // http://www.leptonica.org/rotation.html#ROTATION-BY-AREA-MAPPING
    for (int i = miny; i < maxy; i++) {
        for (int j = minx; j < maxx; j++) {
            float32_t image_x = (float32_t)j - origin_x;
            float32_t image_y = (float32_t)i - origin_y;
            float32_t xt =
                (image_x * cos_theta - image_y * sin_theta) + origin_x;
            float32_t yt =
                (image_x * sin_theta + image_y * cos_theta) + origin_y;

            if (xt >= 0 && xt < src_w && yt >= 0 && yt < src_h) {
                scg_pixel_t color = scg_image_get_pixel(src, (int)xt, (int)yt);
                scg_image_set_pixel(dest, x + j, y + i, color);
            }
        }
    }
}

//
// scg_image_draw_image_rotate_scale implementation
//

void scg_image_draw_image_rotate_scale(scg_image_t *dest, scg_image_t *src,
                                       int x, int y, float32_t angle,
                                       float32_t sx, float32_t sy) {
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

    float32_t sin_theta = sinf(-angle);
    float32_t cos_theta = cosf(-angle);

    float32_t a = src_sw * cos_theta;
    float32_t b = src_sh * cos_theta;
    float32_t c = src_sw * sin_theta;
    float32_t d = src_sh * sin_theta;
    float32_t e = -src_sw * cos_theta;
    float32_t f = -src_sw * sin_theta;
    int x0 = (int)(e + d);
    int y0 = (int)(f - b);
    int x1 = (int)(a + d);
    int y1 = (int)(c - b);
    int x2 = (int)(a - d);
    int y2 = (int)(c + b);
    int x3 = (int)(e - d);
    int y3 = (int)(f + b);

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
            float32_t xt =
                (image_x * cos_theta - image_y * sin_theta) + origin_x;
            float32_t yt =
                (image_x * sin_theta + image_y * cos_theta) + origin_y;

            if (xt >= 0 && xt < src_w && yt >= 0 && yt < src_h) {
                scg_pixel_t color = scg_image_get_pixel(src, (int)xt, (int)yt);
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

static void scg__draw_char_bitmap(scg_image_t *image, const char *bitmap, int x,
                                  int y, scg_pixel_t color) {
    for (int i = 0; i < SCG_FONT_SIZE; i++) {
        for (int j = 0; j < SCG_FONT_SIZE; j++) {
            int set = bitmap[i] & 1 << j;

            if (set) {
                scg_image_set_pixel(image, x + j, y + i, color);
            }
        }
    }
}

//
// scg_image_draw_char implementation
//

void scg_image_draw_char(scg_image_t *image, char ch, int x, int y,
                         scg_pixel_t color) {
    uint8_t char_code = (uint8_t)ch;
    if (char_code > SCG__FONT_CHAR_CODE_END) {
        char_code = SCG__FONT_CHAR_CODE_QUESTION_MARK;
    }

    const char *bitmap = scg__font8x8[char_code];
    scg__draw_char_bitmap(image, bitmap, x, y, color);
}

//
// scg_image_draw_string implementation
//

void scg_image_draw_string(scg_image_t *image, const char *str, int x, int y,
                           bool anchor_to_center, scg_pixel_t color) {
    int current_x = x;
    int current_y = y;

    if (anchor_to_center) {
        int width = strlen(str) * SCG_FONT_SIZE;
        current_x = x - width / 2;
        current_y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != SCG__FONT_CHAR_CODE_SPACE) {
            scg_image_draw_char(image, str[i], current_x, current_y, color);
        }

        current_x += SCG_FONT_SIZE;
    }
}

static const char *scg__bitmap_from_wchar(wchar_t code) {
    if (code <= SCG__FONT_CHAR_CODE_END) {
        return scg__font8x8[code];
    }

    if (code >= SCG__FONT_HIRAGANA_CHAR_CODE_START &&
        code <= SCG__FONT_HIRAGANA_CHAR_CODE_END) {
        uint16_t mapped_code = SCG__FONT_HIRAGANA_NUM_CHARS -
                               (SCG__FONT_HIRAGANA_CHAR_CODE_END - code);
        return scg__font8x8_hiragana[mapped_code];
    }

    return NULL;
}

//
// scg_image_draw_wchar implementation
//

void scg_image_draw_wchar(scg_image_t *image, wchar_t char_code, int x, int y,
                          scg_pixel_t color) {
    const char *bitmap = scg__bitmap_from_wchar(char_code);
    if (bitmap == NULL) {
        bitmap = scg__font8x8[SCG__FONT_CHAR_CODE_QUESTION_MARK];
    }

    scg__draw_char_bitmap(image, bitmap, x, y, color);
}

//
// scg_image_draw_wstring implementation
//

void scg_image_draw_wstring(scg_image_t *image, const wchar_t *str, int x,
                            int y, bool anchor_to_center, scg_pixel_t color) {
    int current_x = x;
    int current_y = y;

    if (anchor_to_center) {
        int width = wcslen(str) * SCG_FONT_SIZE;
        current_x = x - width / 2;
        current_y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != SCG__FONT_CHAR_CODE_SPACE) {
            scg_image_draw_wchar(image, str[i], current_x, current_y, color);
        }

        current_x += SCG_FONT_SIZE;
    }
}

//
// scg_image_draw_frame_metrics implementation
//

void scg_image_draw_frame_metrics(scg_image_t *image,
                                  scg_frame_metrics_t frame_metrics) {
    int fps = frame_metrics.fps;
    float32_t frame_time_ms = frame_metrics.frame_time_millisecs;
    const char *fmt = "fps:%d ms/f:%.4f";

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
// scg_image_save_to_bmp implementation
//

bool scg_image_save_to_bmp(scg_image_t *image, const char *filepath) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
        (void *)image->pixels, image->width, image->height, 32, image->pitch,
        SCG__IMAGE_PIXEL_FORMAT);
    if (surface == NULL) {
        scg_log_errorf("Failed to create SDL surface from image. %s",
                       SDL_GetError());

        return false;
    }

    if (SDL_SaveBMP(surface, filepath) != 0) {
        scg_log_errorf("Failed to save image to %s. %s", filepath,
                       SDL_GetError());

        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);

    return true;
}

//
// scg_image_free implementation
//

void scg_image_free(scg_image_t *image) {
    free(image->pixels);
    free(image);
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
// scg_sound_new_from_wav implementation
//

scg_sound_t *scg_sound_new_from_wav(scg_audio_t *audio, const char *filepath,
                                    bool loop) {
    if (audio->num_sounds == SCG__MAX_SOUNDS) {
        scg_log_error("Maximum sounds reached");

        return NULL;
    }

    SDL_AudioSpec spec;
    uint32_t length;
    uint8_t *buffer;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == NULL) {
        scg_log_errorf("Failed to load WAV file at %s. %s", filepath,
                       SDL_GetError());

        return NULL;
    }

    scg_sound_t *sound = malloc(sizeof(*sound));
    if (sound == NULL) {
        scg_log_error("Failed to allocate memory for sound");

        SDL_FreeWAV(buffer);
        return NULL;
    }

    sound->sdl_spec = spec;
    sound->length = length;
    sound->buffer = buffer;
    sound->play_offset = 0;
    sound->end_position = buffer + length;
    sound->is_playing = false;
    sound->loop = loop;

    audio->sounds[audio->num_sounds++] = sound;

    return sound;
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
// scg_config_new_default implementation
//

scg_config_t scg_config_new_default(void) {
    return (scg_config_t){
        .video = {.width = 640,
                  .height = 480,
                  .title = "SCG Application",
                  .scale = 1,
                  .fullscreen = false,
                  .vsync = true,
                  .show_frame_metrics = true},
        .audio = {.enabled = false, .volume = SCG__MAX_VOLUME / 2}};
}

//
// scg_app_initgg implementation
//

void scg_app_init(scg_app_t *app, scg_config_t config) {
    // Initialise the SDL library.
    {
        uint32_t flags = SDL_INIT_VIDEO;
        if (config.audio.enabled) {
            flags |= SDL_INIT_AUDIO;
        }

        if (SDL_Init(flags) != 0) {
            scg_log_errorf("Failed to initialise SDL. %s", SDL_GetError());

            exit(EXIT_FAILURE);
        }
    }

    scg_image_t *draw_target =
        scg_image_new(config.video.width, config.video.height);
    if (draw_target == NULL) {
        scg_log_error("Failed to create draw target");

        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    scg__screen_t *screen =
        scg__screen_new(draw_target, config.video.title, config.video.scale,
                        config.video.fullscreen, config.video.vsync);
    if (screen == NULL) {
        scg_log_error("Failed to create screen");

        scg_image_free(draw_target);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    scg_keyboard_t *keyboard = scg__keyboard_new();
    if (keyboard == NULL) {
        scg_log_error("Failed to create keyboard");

        scg__screen_free(screen);
        scg_image_free(draw_target);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Audio is not enabled by default, so we must default the sound device to
    // uninitialised.

    scg_audio_t *audio = NULL;
    if (config.audio.enabled) {
        audio = scg__audio_new(screen->target_fps);
        if (audio == NULL) {
            scg_log_error("Failed to create sound device");

            free(keyboard);
            scg__screen_free(screen);
            scg_image_free(draw_target);
            exit(EXIT_FAILURE);
        }
    }

    // Log some information to stdout.
    {
        scg_log_infof("Application '%s' successfuly initialised. "
                      "Width: %d, Height: %d, Target FPS: %d, VSync: %d",
                      config.video.title, draw_target->width,
                      draw_target->height, screen->target_fps, screen->vsync);
        if (config.audio.enabled) {
            scg_log_infof("Audio successfuly initialised."
                          "Device ID: %d, Channels: %d, Samples/sec: %d, "
                          "Samples/frame: %d, Bytes/sample: %d",
                          audio->device_id, audio->num_channels,
                          audio->frequency, audio->latency_sample_count,
                          audio->bytes_per_sample);
        }

        SDL_RendererInfo info;
        SDL_GetRendererInfo(screen->sdl_renderer, &info);
        scg_log_infof("Renderer name: %s", info.name);

        char buffer[1024] = "";
        for (unsigned int i = 0; i < info.num_texture_formats; i++) {
            strcat(buffer, SDL_GetPixelFormatName(info.texture_formats[i]));
            if (i != info.num_texture_formats - 1) {
                strcat(buffer, ", ");
            }
        }
        scg_log_infof("Supported texture formats: %s", buffer);
    }

    app->running = true;
    app->config = config;
    app->draw_target = draw_target;
    app->screen = screen;
    app->keyboard = keyboard;
    app->audio = audio;
    app->delta_time = 0.0f;
    app->delta_time_counter = scg_get_performance_counter();
}

//
// scg_app_begin_frame implementation
//

void scg_app_begin_frame(scg_app_t *app) {
    // Handle events that need to be polled. This may include
    // text input, mouse events, and game controller events.
    {
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            app->running = false;
            break;
        // Quit the app on ESC key, for convinience.
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                app->running = false;
                break;
            }
        }
    }

    // Calculate the delta time in seconds per frame.
    // This is useful for apps that want some quick consistent animation
    // and don't care about fixed updates.
    {
        uint64_t now = scg_get_performance_counter();
        app->delta_time =
            scg_get_elapsed_time_secs(now, app->delta_time_counter);
        app->delta_time_counter = now;
    }
}

//
// scg_app_end_frame implementation
//

void scg_app_end_frame(scg_app_t *app) {
    if (app->config.video.show_frame_metrics) {
        scg_image_draw_frame_metrics(app->draw_target,
                                     app->screen->frame_metrics);
    }

    scg__keyboard_update(app->keyboard);

    if (app->audio != NULL) {
        scg__audio_update(app->audio);
    }

    scg__screen_present(app->screen, app->draw_target);
}

//
// scg_app_shutdown implementation
//

void scg_app_shutdown(scg_app_t *app) {
    if (app->audio != NULL) {
        scg__audio_free(app->audio);
    }

    free(app->keyboard);
    scg__screen_free(app->screen);
    scg_image_free(app->draw_target);

    SDL_Quit();
}

static int scg__get_monitor_refresh_rate(SDL_DisplayMode display_mode) {
    int result = display_mode.refresh_rate;

    if (result == 0) {
        return SCG__DEFAULT_REFRESH_RATE;
    }

    return result;
}

static scg__screen_t *scg__screen_new(scg_image_t *draw_target,
                                      const char *title, int scale,
                                      bool fullscreen, bool vsync) {
    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
        scg_log_errorf("Failed to get SDL desktop display mode. %s",
                       SDL_GetError());

        return NULL;
    }

    SDL_Window *sdl_window = NULL;
    SDL_Renderer *sdl_renderer = NULL;
    SDL_Texture *sdl_texture = NULL;

    int w = draw_target->width;
    int h = draw_target->height;

    // Setup the SDL window.
    {
        sdl_window = SDL_CreateWindow(
            title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w * scale,
            h * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (sdl_window == NULL) {
            scg_log_errorf("Failed to create SDL Window. %s", SDL_GetError());

            return NULL;
        }

        if (fullscreen) {
            SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
    }

    // Setup the SDL renderer.
    {
        uint32_t sdl_renderer_flags = SDL_RENDERER_ACCELERATED;
        if (vsync) {
            sdl_renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, sdl_renderer_flags);
        if (sdl_renderer == NULL) {
            scg_log_errorf("Failed to create SDL Renderer. %s", SDL_GetError());

            SDL_DestroyWindow(sdl_window);
            return NULL;
        }

        SDL_RenderSetLogicalSize(sdl_renderer, w, h);
    }

    // Setup the SDL texture which will represent our screen.
    {
        sdl_texture = SDL_CreateTexture(sdl_renderer, SCG__IMAGE_PIXEL_FORMAT,
                                        SDL_TEXTUREACCESS_STREAMING, w, h);
        if (sdl_texture == NULL) {
            scg_log_errorf("Failed to create SDL Texture. %s", SDL_GetError());

            SDL_DestroyRenderer(sdl_renderer);
            SDL_DestroyWindow(sdl_window);
            return NULL;
        }
    }

    scg__screen_t *screen = malloc(sizeof(*screen));
    if (screen == NULL) {
        scg_log_error("Failed to allocate memory for screen");

        SDL_DestroyTexture(sdl_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        return NULL;
    }

    screen->target_fps = scg__get_monitor_refresh_rate(display_mode);
    screen->target_frame_time_secs = 1.0 / (float64_t)screen->target_fps;
    screen->last_frame_counter = scg_get_performance_counter();
    screen->sdl_window = sdl_window;
    screen->sdl_renderer = sdl_renderer;
    screen->sdl_texture = sdl_texture;
    screen->vsync = vsync;

    screen->frame_metrics.frame_time_secs = 0.0f;
    screen->frame_metrics.frame_time_millisecs = 0.0f;
    screen->frame_metrics.fps = 0;
    screen->frame_metrics_update_counter = scg_get_performance_counter();
    screen->frame_metrics.target_fps = screen->target_fps;

    return screen;
}

static void scg__screen_present(scg__screen_t *screen,
                                scg_image_t *draw_target) {
    // Wait until we have reached the target amount of time per frame (e.g 60hz,
    // ~16ms). Spinning in a while loop seems to be the most accurate way to do
    // this, as trying to use SDL_Delay (sleeping) is dependant on other
    // factors.
    if (screen->vsync) {
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

    SDL_UpdateTexture(screen->sdl_texture, NULL, draw_target->pixels,
                      draw_target->pitch);
    SDL_RenderClear(screen->sdl_renderer);
    SDL_RenderCopy(screen->sdl_renderer, screen->sdl_texture, NULL, NULL);
    SDL_RenderPresent(screen->sdl_renderer);

    // Update the frame metrics every second.
    // TODO: Instead of capturing every second, average the metrics...
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
            screen->frame_metrics.fps = roundf(
                (float64_t)scg_get_performance_frequency() /
                (float64_t)(end_frame_counter - screen->last_frame_counter));

            screen->frame_metrics_update_counter =
                scg_get_performance_counter();
        }
    }

    screen->last_frame_counter = end_frame_counter;
}

static void scg__screen_free(scg__screen_t *screen) {
    SDL_DestroyTexture(screen->sdl_texture);
    SDL_DestroyRenderer(screen->sdl_renderer);
    SDL_DestroyWindow(screen->sdl_window);

    free(screen);
}

static scg_keyboard_t *scg__keyboard_new(void) {
    scg_keyboard_t *keyboard = malloc(sizeof(*keyboard));
    if (keyboard == NULL) {
        scg_log_error("Failed to allocate memory for keyboard");

        return NULL;
    }

    keyboard->current_key_states = SDL_GetKeyboardState(NULL);
    memset(keyboard->previous_key_states, 0,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);

    return keyboard;
}

static void scg__keyboard_update(scg_keyboard_t *keyboard) {
    memcpy(keyboard->previous_key_states, keyboard->current_key_states,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);
}

static scg_audio_t *scg__audio_new(int target_fps) {
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
        scg_log_errorf("Failed to open SDL audio device. %s", SDL_GetError());

        return NULL;
    }

    if (obtained.format != desired.format) {
        scg_log_error("Audio device does not support format S16LSB");

        SDL_CloseAudioDevice(device_id);
        return NULL;
    }

    int obtained_frequency = obtained.freq;
    int latency_sample_count = obtained_frequency / 15;
    uint32_t buffer_size = latency_sample_count * bytes_per_sample;

    uint8_t *buffer = calloc(latency_sample_count, bytes_per_sample);
    if (buffer == NULL) {
        scg_log_errorf("Failed to allocate memory for sound buffer. bytes=%zu",
                       latency_sample_count * bytes_per_sample);

        SDL_CloseAudioDevice(device_id);
        return NULL;
    }

    scg_audio_t *audio = malloc(sizeof(*audio));
    if (audio == NULL) {
        scg_log_error("Failed to allocate memory for audio");

        free(buffer);
        SDL_CloseAudioDevice(device_id);
        return NULL;
    }

    audio->device_id = device_id;
    audio->frequency = obtained_frequency;
    audio->num_channels = obtained.channels;
    audio->num_samples = obtained.samples;
    audio->bytes_per_sample = bytes_per_sample;
    audio->latency_sample_count = latency_sample_count;
    audio->buffer_size = buffer_size;
    audio->buffer = buffer;
    audio->num_sounds = 0;

    SDL_PauseAudioDevice(device_id, 0);

    return audio;
}

static void scg__audio_update(scg_audio_t *audio) {
    memset(audio->buffer, 0, audio->buffer_size);

    uint32_t target_queue_bytes = audio->buffer_size;
    uint32_t bytes_to_write =
        target_queue_bytes - SDL_GetQueuedAudioSize(audio->device_id);

    for (int i = 0; i < audio->num_sounds; i++) {
        scg_sound_t *sound = audio->sounds[i];

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

            SDL_MixAudioFormat(audio->buffer, current_play_position,
                               AUDIO_S16LSB, bytes_to_mix, SCG__MAX_VOLUME / 2);

            sound->play_offset += bytes_to_mix;
        }
    }

    SDL_QueueAudio(audio->device_id, audio->buffer, bytes_to_write);
}

static void scg__audio_free(scg_audio_t *audio) {
    SDL_PauseAudioDevice(audio->device_id, 1);
    SDL_ClearQueuedAudio(audio->device_id);

    for (int i = 0; i < audio->num_sounds; i++) {
        SDL_FreeWAV(audio->sounds[i]->buffer);
    }

    free(audio->buffer);
    SDL_CloseAudioDevice(audio->device_id);

    free(audio);
}

// Bitmap fonts taken from https://github.com/dhepper/font8x8.
static const char scg__font8x8[128][8] = {
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

static const char scg__font8x8_hiragana[96][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+3040
    {0x04, 0x3F, 0x04, 0x3C, 0x56, 0x4D, 0x26, 0x00}, // U+3041 (Hiragana a)
    {0x04, 0x3F, 0x04, 0x3C, 0x56, 0x4D, 0x26, 0x00}, // U+3042 (Hiragana A)
    {0x00, 0x00, 0x00, 0x11, 0x21, 0x25, 0x02, 0x00}, // U+3043 (Hiragana i)
    {0x00, 0x01, 0x11, 0x21, 0x21, 0x25, 0x02, 0x00}, // U+3044 (Hiragana I)
    {0x00, 0x1C, 0x00, 0x1C, 0x22, 0x20, 0x18, 0x00}, // U+3045 (Hiragana u)
    {0x3C, 0x00, 0x3C, 0x42, 0x40, 0x20, 0x18, 0x00}, // U+3046 (Hiragana U)
    {0x1C, 0x00, 0x3E, 0x10, 0x38, 0x24, 0x62, 0x00}, // U+3047 (Hiragana e)
    {0x1C, 0x00, 0x3E, 0x10, 0x38, 0x24, 0x62, 0x00}, // U+3048 (Hiragana E)
    {0x24, 0x4F, 0x04, 0x3C, 0x46, 0x45, 0x22, 0x00}, // U+3049 (Hiragana o)
    {0x24, 0x4F, 0x04, 0x3C, 0x46, 0x45, 0x22, 0x00}, // U+304A (Hiragana O)
    {0x04, 0x24, 0x4F, 0x54, 0x52, 0x12, 0x09, 0x00}, // U+304B (Hiragana KA)
    {0x44, 0x24, 0x0F, 0x54, 0x52, 0x52, 0x09, 0x00}, // U+304C (Hiragana GA)
    {0x08, 0x1F, 0x08, 0x3F, 0x1C, 0x02, 0x3C, 0x00}, // U+304D (Hiragana KI)
    {0x44, 0x2F, 0x04, 0x1F, 0x0E, 0x01, 0x1E, 0x00}, // U+304E (Hiragana GI)
    {0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00}, // U+304F (Hiragana KU)
    {0x28, 0x44, 0x12, 0x21, 0x02, 0x04, 0x08, 0x00}, // U+3050 (Hiragana GU)
    {0x00, 0x22, 0x79, 0x21, 0x21, 0x22, 0x10, 0x00}, // U+3051 (Hiragana KE)
    {0x40, 0x22, 0x11, 0x3D, 0x11, 0x12, 0x08, 0x00}, // U+3052 (Hiragana GE)
    {0x00, 0x00, 0x3C, 0x00, 0x02, 0x02, 0x3C, 0x00}, // U+3053 (Hiragana KO)
    {0x20, 0x40, 0x16, 0x20, 0x01, 0x01, 0x0E, 0x00}, // U+3054 (Hiragana GO)
    {0x10, 0x7E, 0x10, 0x3C, 0x02, 0x02, 0x1C, 0x00}, // U+3055 (Hiragana SA)
    {0x24, 0x4F, 0x14, 0x2E, 0x01, 0x01, 0x0E, 0x00}, // U+3056 (Hiragana ZA)
    {0x00, 0x02, 0x02, 0x02, 0x42, 0x22, 0x1C, 0x00}, // U+3057 (Hiragana SI)
    {0x20, 0x42, 0x12, 0x22, 0x02, 0x22, 0x1C, 0x00}, // U+3058 (Hiragana ZI)
    {0x10, 0x7E, 0x18, 0x14, 0x18, 0x10, 0x0C, 0x00}, // U+3059 (Hiragana SU)
    {0x44, 0x2F, 0x06, 0x05, 0x06, 0x04, 0x03, 0x00}, // U+305A (Hiragana ZU)
    {0x20, 0x72, 0x2F, 0x22, 0x1A, 0x02, 0x1C, 0x00}, // U+305B (Hiragana SE)
    {(char)0x80, 0x50, 0x3A, 0x17, 0x1A, 0x02, 0x1C,
     0x00},                                           // U+305C (Hiragana ZE)
    {0x1E, 0x08, 0x04, 0x7F, 0x08, 0x04, 0x38, 0x00}, // U+305D (Hiragana SO)
    {0x4F, 0x24, 0x02, 0x7F, 0x08, 0x04, 0x38, 0x00}, // U+305E (Hiragana ZO)
    {0x02, 0x0F, 0x02, 0x72, 0x02, 0x09, 0x71, 0x00}, // U+305F (Hiragana TA)
    {0x42, 0x2F, 0x02, 0x72, 0x02, 0x09, 0x71, 0x00}, // U+3060 (Hiragana DA)
    {0x08, 0x7E, 0x08, 0x3C, 0x40, 0x40, 0x38, 0x00}, // U+3061 (Hiragana TI)
    {0x44, 0x2F, 0x04, 0x1E, 0x20, 0x20, 0x1C, 0x00}, // U+3062 (Hiragana DI)
    {0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x1C, 0x00}, // U+3063 (Hiragana tu)
    {0x00, 0x1C, 0x22, 0x41, 0x40, 0x20, 0x1C, 0x00}, // U+3064 (Hiragana TU)
    {0x40, 0x20, 0x1E, 0x21, 0x20, 0x20, 0x1C, 0x00}, // U+3065 (Hiragana DU)
    {0x00, 0x3E, 0x08, 0x04, 0x04, 0x04, 0x38, 0x00}, // U+3066 (Hiragana TE)
    {0x00, 0x3E, 0x48, 0x24, 0x04, 0x04, 0x38, 0x00}, // U+3067 (Hiragana DE)
    {0x04, 0x04, 0x08, 0x3C, 0x02, 0x02, 0x3C, 0x00}, // U+3068 (Hiragana TO)
    {0x44, 0x24, 0x08, 0x3C, 0x02, 0x02, 0x3C, 0x00}, // U+3069 (Hiragana DO)
    {0x32, 0x02, 0x27, 0x22, 0x72, 0x29, 0x11, 0x00}, // U+306A (Hiragana NA)
    {0x00, 0x02, 0x7A, 0x02, 0x0A, 0x72, 0x02, 0x00}, // U+306B (Hiragana NI)
    {0x08, 0x09, 0x3E, 0x4B, 0x65, 0x55, 0x22, 0x00}, // U+306C (Hiragana NU)
    {0x04, 0x07, 0x34, 0x4C, 0x66, 0x54, 0x24, 0x00}, // U+306D (Hiragana NE)
    {0x00, 0x00, 0x3C, 0x4A, 0x49, 0x45, 0x22, 0x00}, // U+306E (Hiragana NO)
    {0x00, 0x22, 0x7A, 0x22, 0x72, 0x2A, 0x12, 0x00}, // U+306F (Hiragana HA)
    {(char)0x80, 0x51, 0x1D, 0x11, 0x39, 0x15, 0x09,
     0x00}, // U+3070 (Hiragana BA)
    {0x40, (char)0xB1, 0x5D, 0x11, 0x39, 0x15, 0x09,
     0x00},                                           // U+3071 (Hiragana PA)
    {0x00, 0x00, 0x13, 0x32, 0x51, 0x11, 0x0E, 0x00}, // U+3072 (Hiragana HI)
    {0x40, 0x20, 0x03, 0x32, 0x51, 0x11, 0x0E, 0x00}, // U+3073 (Hiragana BI)
    {0x40, (char)0xA0, 0x43, 0x32, 0x51, 0x11, 0x0E,
     0x00},                                           // U+3074 (Hiragana PI)
    {0x1C, 0x00, 0x08, 0x2A, 0x49, 0x10, 0x0C, 0x00}, // U+3075 (Hiragana HU)
    {0x4C, 0x20, 0x08, 0x2A, 0x49, 0x10, 0x0C, 0x00}, // U+3076 (Hiragana BU)
    {0x4C, (char)0xA0, 0x48, 0x0A, 0x29, 0x48, 0x0C,
     0x00},                                           // U+3077 (Hiragana PU)
    {0x00, 0x00, 0x04, 0x0A, 0x11, 0x20, 0x40, 0x00}, // U+3078 (Hiragana HE)
    {0x20, 0x40, 0x14, 0x2A, 0x11, 0x20, 0x40, 0x00}, // U+3079 (Hiragana BE)
    {0x20, 0x50, 0x24, 0x0A, 0x11, 0x20, 0x40, 0x00}, // U+307A (Hiragana PE)
    {0x7D, 0x11, 0x7D, 0x11, 0x39, 0x55, 0x09, 0x00}, // U+307B (Hiragana HO)
    {(char)0x9D, 0x51, 0x1D, 0x11, 0x39, 0x55, 0x09,
     0x00}, // U+307C (Hiragana BO)
    {0x5D, (char)0xB1, 0x5D, 0x11, 0x39, 0x55, 0x09,
     0x00},                                           // U+307D (Hiragana PO)
    {0x7E, 0x08, 0x3E, 0x08, 0x1C, 0x2A, 0x04, 0x00}, // U+307E (Hiragana MA)
    {0x00, 0x07, 0x24, 0x24, 0x7E, 0x25, 0x12, 0x00}, // U+307F (Hiragana MI)
    {0x04, 0x0F, 0x64, 0x06, 0x05, 0x26, 0x3C, 0x00}, // U+3080 (Hiragana MU)
    {0x00, 0x09, 0x3D, 0x4A, 0x4B, 0x45, 0x2A, 0x00}, // U+3081 (Hiragana ME)
    {0x02, 0x0F, 0x02, 0x0F, 0x62, 0x42, 0x3C, 0x00}, // U+3082 (Hiragana MO)
    {0x00, 0x00, 0x12, 0x1F, 0x22, 0x12, 0x04, 0x00}, // U+3083 (Hiragana ya)
    {0x00, 0x12, 0x3F, 0x42, 0x42, 0x34, 0x04, 0x00}, // U+3084 (Hiragana YA)
    {0x00, 0x00, 0x11, 0x3D, 0x53, 0x39, 0x11, 0x00}, // U+3085 (Hiragana yu)
    {0x00, 0x11, 0x3D, 0x53, 0x51, 0x39, 0x11, 0x00}, // U+3086 (Hiragana YU)
    {0x00, 0x08, 0x38, 0x08, 0x1C, 0x2A, 0x04, 0x00}, // U+3087 (Hiragana yo)
    {0x08, 0x08, 0x38, 0x08, 0x1C, 0x2A, 0x04, 0x00}, // U+3088 (Hiragana YO)
    {0x1E, 0x00, 0x02, 0x3A, 0x46, 0x42, 0x30, 0x00}, // U+3089 (Hiragana RA)
    {0x00, 0x20, 0x22, 0x22, 0x2A, 0x24, 0x10, 0x00}, // U+308A (Hiragana RI)
    {0x1F, 0x08, 0x3C, 0x42, 0x49, 0x54, 0x38, 0x00}, // U+308B (Hiragana RU)
    {0x04, 0x07, 0x04, 0x0C, 0x16, 0x55, 0x24, 0x00}, // U+308C (Hiragana RE)
    {0x3F, 0x10, 0x08, 0x3C, 0x42, 0x41, 0x30, 0x00}, // U+308D (Hiragana RO)
    {0x00, 0x00, 0x08, 0x0E, 0x38, 0x4C, 0x2A, 0x00}, // U+308E (Hiragana wa)
    {0x04, 0x07, 0x04, 0x3C, 0x46, 0x45, 0x24, 0x00}, // U+308F (Hiragana WA)
    {0x0E, 0x08, 0x3C, 0x4A, 0x69, 0x55, 0x32, 0x00}, // U+3090 (Hiragana WI)
    {0x06, 0x3C, 0x42, 0x39, 0x04, 0x36, 0x49, 0x00}, // U+3091 (Hiragana WE)
    {0x04, 0x0F, 0x04, 0x6E, 0x11, 0x08, 0x70, 0x00}, // U+3092 (Hiragana WO)
    {0x08, 0x08, 0x04, 0x0C, 0x56, 0x52, 0x21, 0x00}, // U+3093 (Hiragana N)
    {0x40, 0x2E, 0x00, 0x3C, 0x42, 0x40, 0x38, 0x00}, // U+3094 (Hiragana VU)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+3095
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+3096
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+3097
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+3098
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00}, // U+3099 (voiced combinator mark)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00}, // U+309A (semivoiced combinator mark)
    {0x40, (char)0x80, 0x20, 0x40, 0x00, 0x00, 0x00,
     0x00}, // U+309B (Hiragana voiced mark)
    {0x40, (char)0xA0, 0x40, 0x00, 0x00, 0x00, 0x00,
     0x00}, // U+309C (Hiragana semivoiced mark)
    {0x00, 0x00, 0x08, 0x08, 0x10, 0x30, 0x0C,
     0x00}, // U+309D (Hiragana iteration mark)
    {0x20, 0x40, 0x14, 0x24, 0x08, 0x18, 0x06,
     0x00}, // U+309E (Hiragana voiced iteration mark)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // U+309F
};

#endif // SCG_IMPLEMENTATION
