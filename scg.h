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

// This is for quick and dirty string formatting, and
// is designed to work with some stack allocated buffer.
// It returns an int like the std sprintf. Most of the time this won't be
// checked, and the buffer should just be large enough.
extern int scg_sprintf(char *buf, const char *fmt, ...);

// This will dynamically allocate the correct amount of memory for
// the formatted string. The buf argument must be free'd by the caller.
extern int scg_asprintf(char **buf, const char *fmt, ...);

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

extern scg_pixel_t scg_pixel_lerp_rgb(scg_pixel_t start, scg_pixel_t end,
                                      float32_t t);

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
extern void scg_image_draw_circle(scg_image_t *image, int x, int y, int r,
                                  scg_pixel_t color);
extern void scg_image_fill_circle(scg_image_t *image, int x, int y, int r,
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
    bool current_key_states[SDL_NUM_SCANCODES];
    bool last_frame_key_states[SDL_NUM_SCANCODES];
} scg_keyboard_t;

extern bool scg_keyboard_is_key_down(scg_keyboard_t *keyboard,
                                     scg_key_code_t code);
extern bool scg_keyboard_is_key_up(scg_keyboard_t *keyboard,
                                   scg_key_code_t code);
extern bool scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
                                          scg_key_code_t code);

typedef enum scg_mouse_button_t {
    SCG_MOUSE_BUTTON_LEFT = SDL_BUTTON_LEFT,
    SCG_MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    SCG_MOUSE_BUTTON_RIGHT = SDL_BUTTON_RIGHT
} scg_mouse_button_t;

typedef struct scg_mouse_t {
    int x, y;
    int window_x, window_y;
    uint32_t button_state;
} scg_mouse_t;

extern bool scg_mouse_is_button_down(scg_mouse_t *mouse,
                                     scg_mouse_button_t button);
extern bool scg_mouse_is_button_up(scg_mouse_t *mouse,
                                   scg_mouse_button_t button);

typedef struct scg_config_t {
    struct {
        int width;
        int height;
        const char *title;
        int scale;
        bool fullscreen;
        bool vsync;
        bool lock_fps;
        bool show_frame_metrics;
    } video;

    struct {
        bool hide_mouse_cursor;
    } input;

    struct {
        bool enabled;
        int volume;
    } audio;
} scg_config_t;

extern scg_config_t scg_config_new_default(void);

typedef struct scg__screen_t {
    int window_width, window_height;
    int target_fps;
    float64_t target_frame_time_secs;
    uint64_t last_frame_counter;
    float64_t frame_metrics_update_counter;
    scg_frame_metrics_t frame_metrics;
    bool vsync;
    bool lock_fps;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
} scg__screen_t;

typedef struct scg_app_t {
    bool running;
    float32_t delta_time;
    float32_t elapsed_time;
    scg_config_t config;
    scg_image_t *draw_target;
    scg_keyboard_t *keyboard;
    scg_mouse_t *mouse;
    scg_audio_t *audio;

    uint64_t delta_time_counter;
    scg__screen_t *screen;
} scg_app_t;

extern void scg_app_init(scg_app_t *app, scg_config_t config);
extern bool scg_app_process_events(scg_app_t *app);
extern void scg_app_present(scg_app_t *app);
extern void scg_app_close(scg_app_t *app);
extern void scg_app_free(scg_app_t *app);

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
#include <stdarg.h>
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

static const char scg__base64_table[64];
static const char *scg__font8x8_data;
static const char *scg__font8x8_hiragana_data;

static char scg__font8x8[SCG__FONT_NUM_CHARS * SCG_FONT_SIZE];
static char scg__font8x8_hiragana[SCG__FONT_HIRAGANA_NUM_CHARS * SCG_FONT_SIZE];

static void scg__decode_font_data(char *out, size_t out_length,
                                  const char *data);

static scg__screen_t *scg__screen_new(scg_image_t *draw_target,
                                      const char *title, int scale,
                                      bool fullscreen, bool vsync,
                                      bool lock_fps, bool hide_mouse_cursor);
static void scg__screen_present(scg__screen_t *screen,
                                scg_image_t *draw_target);
static void scg__screen_free(scg__screen_t *screen);

static scg_keyboard_t *scg__keyboard_new(void);
static void scg__keyboard_update_keystates(scg_keyboard_t *keyboard);

static scg_mouse_t *scg__mouse_new(void);
static void scg__mouse_update(scg_mouse_t *mouse, int w, int h, int win_w,
                              int win_h);

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

static int scg__vsprintf(char *buf, const char *fmt, va_list args) {
    va_list tmpa;
    va_copy(tmpa, args);

    int size = vsnprintf(NULL, 0, fmt, tmpa);

    va_end(tmpa);

    if (size < 0) {
        return -1;
    }

    size = vsnprintf(buf, size + 1, fmt, args);
    return size;
}

//
// scg_sprintf implementation
//

int scg_sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int size = scg__vsprintf(buf, fmt, args);

    va_end(args);

    return size;
}

static int scg__vasprintf(char **buf, const char *fmt, va_list args) {
    va_list tmpa;
    va_copy(tmpa, args);

    int size = vsnprintf(NULL, 0, fmt, tmpa);

    va_end(tmpa);

    if (size < 0) {
        return -1;
    }

    *buf = malloc(size + 1);
    if (*buf == NULL) {
        return -1;
    }

    size = vsnprintf(*buf, size + 1, fmt, args);
    return size;
}

//
// scg_asprintf implementation
//

int scg_asprintf(char **buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int size = scg__vasprintf(buf, fmt, args);

    va_end(args);

    return size;
}

//
// scg_pixel_lerp_rgb implementation
//

scg_pixel_t scg_pixel_lerp_rgb(scg_pixel_t start, scg_pixel_t end,
                               float32_t t) {
    t = scg_clamp_float32(t, 0.0f, 1.0f);

    float32_t r = (1.0f - t) * start.data.r + t * end.data.r;
    float32_t g = (1.0f - t) * start.data.g + t * end.data.g;
    float32_t b = (1.0f - t) * start.data.b + t * end.data.b;

    return scg_pixel_new_rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
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

//
// scg_image_draw_circle implementation
//

void scg_image_draw_circle(scg_image_t *image, int x, int y, int r,
                           scg_pixel_t color) {
    int f = 1 - r;
    int ddf_x = 0;
    int ddf_y = -2 * r;
    int xi = 0;
    int yi = r;

    scg_image_set_pixel(image, x, y + r, color);
    scg_image_set_pixel(image, x, y - r, color);
    scg_image_set_pixel(image, x + r, y, color);
    scg_image_set_pixel(image, x - r, y, color);

    while (xi < yi) {
        if (f >= 0) {
            yi--;
            ddf_y += 2;
            f += ddf_y;
        }

        xi++;
        ddf_x += 2;
        f += ddf_x + 1;

        scg_image_set_pixel(image, x + xi, y + yi, color);
        scg_image_set_pixel(image, x - xi, y + yi, color);
        scg_image_set_pixel(image, x + xi, y - yi, color);
        scg_image_set_pixel(image, x - xi, y - yi, color);
        scg_image_set_pixel(image, x + yi, y + xi, color);
        scg_image_set_pixel(image, x - yi, y + xi, color);
        scg_image_set_pixel(image, x + yi, y - xi, color);
        scg_image_set_pixel(image, x - yi, y - xi, color);
    }
}

//
// scg_image_fill_circle implementation
//

void scg_image_fill_circle(scg_image_t *image, int x, int y, int r,
                           scg_pixel_t color) {
    int f = 1 - r;
    int ddf_x = 0;
    int ddf_y = -2 * r;
    int xi = 0;
    int yi = r;

    scg_image_draw_line(image, x, y - r, x, y + r, color);
    scg_image_draw_line(image, x - r, y, x + r, y, color);

    while (xi < yi) {
        if (f >= 0) {
            yi--;
            ddf_y += 2;
            f += ddf_y;
        }

        xi++;
        ddf_x += 2;
        f += ddf_x + 1;

        scg_image_draw_line(image, x - xi, y + yi, x + xi, y + yi, color);
        scg_image_draw_line(image, x - xi, y - yi, x + xi, y - yi, color);
        scg_image_draw_line(image, x - yi, y + xi, x + yi, y + xi, color);
        scg_image_draw_line(image, x - yi, y - xi, x + yi, y - xi, color);
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

    char bitmap[SCG_FONT_SIZE];
    int index = char_code * SCG_FONT_SIZE;
    memcpy(bitmap, scg__font8x8 + index, SCG_FONT_SIZE);

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

//
// scg_image_draw_wchar implementation
//

void scg_image_draw_wchar(scg_image_t *image, wchar_t ch, int x, int y,
                          scg_pixel_t color) {
    char bitmap[SCG_FONT_SIZE];

    if (ch <= SCG__FONT_CHAR_CODE_END) {
        int index = ch * SCG_FONT_SIZE;
        memcpy(bitmap, scg__font8x8 + index, SCG_FONT_SIZE);
    } else if (ch >= SCG__FONT_HIRAGANA_CHAR_CODE_START &&
               ch <= SCG__FONT_HIRAGANA_CHAR_CODE_END) {
        int mapped_code = SCG__FONT_HIRAGANA_NUM_CHARS -
                          (SCG__FONT_HIRAGANA_CHAR_CODE_END - ch);

        int index = mapped_code * SCG_FONT_SIZE;
        memcpy(bitmap, scg__font8x8_hiragana + index, SCG_FONT_SIZE);
    } else {
        int index = SCG__FONT_CHAR_CODE_QUESTION_MARK * SCG_FONT_SIZE;
        memcpy(bitmap, scg__font8x8 + index, SCG_FONT_SIZE);
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
    return keyboard->last_frame_key_states[key] == 0 &&
           keyboard->current_key_states[key] == 1;
}

//
// scg_mouse_is_button_down implementation
//

bool scg_mouse_is_button_down(scg_mouse_t *mouse, scg_mouse_button_t button) {
    return mouse->button_state & SDL_BUTTON(button);
}

//
// scg_mouse_is_button_up implementation
//

bool scg_mouse_is_button_up(scg_mouse_t *mouse, scg_mouse_button_t button) {
    return !(mouse->button_state & SDL_BUTTON(button));
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
                  .lock_fps = true,
                  .show_frame_metrics = true},
        .input = {.hide_mouse_cursor = true},
        .audio = {.enabled = false, .volume = SCG__MAX_VOLUME / 2}};
}

//
// scg_app_init implementation
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
                        config.video.fullscreen, config.video.vsync,
                        config.video.lock_fps, config.input.hide_mouse_cursor);
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

    scg_mouse_t *mouse = scg__mouse_new();
    if (mouse == NULL) {
        scg_log_error("Failed to create mouse");

        free(keyboard);
        scg__screen_free(screen);
        scg_image_free(draw_target);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    scg__mouse_update(mouse, draw_target->width, draw_target->height,
                      screen->window_width, screen->window_height);

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

    scg__decode_font_data(scg__font8x8, SCG__FONT_NUM_CHARS * SCG_FONT_SIZE,
                          scg__font8x8_data);
    scg__decode_font_data(scg__font8x8_hiragana,
                          SCG__FONT_HIRAGANA_NUM_CHARS * SCG_FONT_SIZE,
                          scg__font8x8_hiragana_data);

    // Log some information to stdout.
    {
        scg_log_infof("Application '%s' successfuly initialised. "
                      "Width: %d, Height: %d, Target FPS: %d, VSync: %d",
                      config.video.title, draw_target->width,
                      draw_target->height, screen->target_fps, screen->vsync);
        if (config.audio.enabled) {
            scg_log_infof("Audio successfuly initialised. "
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
    app->mouse = mouse;
    app->audio = audio;
    app->delta_time = 0.0f;
    app->elapsed_time = 0.0f;
    app->delta_time_counter = scg_get_performance_counter();
}

//
// scg_app_process_events implementation
//

bool scg_app_process_events(scg_app_t *app) {
    // Handle events that need to be polled. This may include
    // text input, mouse events, and game controller events.
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                app->running = false;
                return false;
            case SDL_KEYDOWN:
                app->keyboard->current_key_states[event.key.keysym.scancode] = true;
                break;
            case SDL_KEYUP:
                app->keyboard->current_key_states[event.key.keysym.scancode] = false;
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
                scg__mouse_update(app->mouse, app->draw_target->width,
                                  app->draw_target->height,
                                  app->screen->window_width,
                                  app->screen->window_height);
                break;
            }
        }
    }

    if (scg_keyboard_is_key_triggered(app->keyboard, SCG_KEY_ESCAPE)) {
        app->running = false;
        return false;
    }

    // Calculate the delta time and elapsed time in seconds.
    // This is useful for apps that want some quick consistent animation
    // and don't care about fixed updates.
    {
        uint64_t now = scg_get_performance_counter();
        app->delta_time =
            scg_get_elapsed_time_secs(now, app->delta_time_counter);
        app->delta_time_counter = now;

        app->elapsed_time += app->delta_time;
    }

    return true;
}

//
// scg_app_present implementation
//

void scg_app_present(scg_app_t *app) {
    if (app->config.video.show_frame_metrics) {
        scg_image_draw_frame_metrics(app->draw_target,
                                     app->screen->frame_metrics);
    }

    scg__keyboard_update_keystates(app->keyboard);

    if (app->audio != NULL) {
        scg__audio_update(app->audio);
    }

    scg__screen_present(app->screen, app->draw_target);
}

//
// scg_app_close implementation
//

void scg_app_close(scg_app_t *app) {
    app->running = false;
}

//
// scg_app_free implementation
//

void scg_app_free(scg_app_t *app) {
    if (app->audio != NULL) {
        scg__audio_free(app->audio);
    }

    free(app->mouse);
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
                                      bool fullscreen, bool vsync,
                                      bool lock_fps, bool hide_mouse_cursor) {
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
    int window_w = w * scale;
    int window_h = h * scale;

    // Setup the SDL window.
    {
        sdl_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, window_w,
                                      window_h * scale, SDL_WINDOW_SHOWN);
        if (sdl_window == NULL) {
            scg_log_errorf("Failed to create SDL Window. %s", SDL_GetError());

            return NULL;
        }

        if (fullscreen) {
            SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);

            // Desktop fullscreen mode will take the current size of the users
            // desktop,
            SDL_GetWindowSize(sdl_window, &window_w, &window_h);
        }

        int toggle = hide_mouse_cursor ? SDL_DISABLE : SDL_ENABLE;
        SDL_ShowCursor(toggle);
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

    screen->window_width = window_w;
    screen->window_height = window_h;
    screen->target_fps = scg__get_monitor_refresh_rate(display_mode);
    screen->target_frame_time_secs = 1.0 / (float64_t)screen->target_fps;
    screen->last_frame_counter = scg_get_performance_counter();
    screen->sdl_window = sdl_window;
    screen->sdl_renderer = sdl_renderer;
    screen->sdl_texture = sdl_texture;
    screen->vsync = vsync;
    screen->lock_fps = lock_fps;

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
    // ~16ms).
    if (screen->lock_fps) {
        float64_t target_secs = screen->target_frame_time_secs;

        if (scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                      screen->last_frame_counter) <
            target_secs) {
            float64_t elapsed_time = scg_get_elapsed_time_secs(
                scg_get_performance_counter(), screen->last_frame_counter);

            int32_t sleep_time = ((target_secs - elapsed_time) * 1000) - 1;
            if (sleep_time > 0) {
                SDL_Delay(sleep_time);
            }

            while (scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                             screen->last_frame_counter) <
                   target_secs) {
            }
        }
    }

    uint64_t end_frame_counter = scg_get_performance_counter();

    // Update the texture and present it on screen.
    {
        SDL_UpdateTexture(screen->sdl_texture, NULL, draw_target->pixels,
                          draw_target->pitch);
        SDL_RenderClear(screen->sdl_renderer);
        SDL_RenderCopy(screen->sdl_renderer, screen->sdl_texture, NULL, NULL);
        SDL_RenderPresent(screen->sdl_renderer);
    }

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

    memset(keyboard->current_key_states, 0, sizeof(bool) * SDL_NUM_SCANCODES);
    memset(keyboard->last_frame_key_states, 0, sizeof(bool) * SDL_NUM_SCANCODES);

    return keyboard;
}

static void scg__keyboard_update_keystates(scg_keyboard_t *keyboard) {
    memcpy(keyboard->last_frame_key_states, keyboard->current_key_states,
           sizeof(bool) * SDL_NUM_SCANCODES);
}

static scg_mouse_t *scg__mouse_new(void) {
    scg_mouse_t *mouse = malloc(sizeof(*mouse));
    if (mouse == NULL) {
        scg_log_error("Failed to allocate memory for mouse");

        return NULL;
    }

    return mouse;
}

static void scg__mouse_update(scg_mouse_t *mouse, int w, int h, int win_w,
                              int win_h) {
    int x, y;
    uint32_t button_state = SDL_GetMouseState(&x, &y);

    mouse->window_x = x;
    mouse->window_y = y;
    mouse->x = (int)(((float32_t)x / (float32_t)win_w) * (float32_t)w);
    mouse->y = (int)(((float32_t)y / (float32_t)win_h) * (float32_t)h);
    mouse->button_state = button_state;
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

static uint8_t scg__base64_index_from_char(char c) {
    for (int i = 0; i < 64; i++) {
        if (c == scg__base64_table[i]) {
            return i;
        }
    }

    return 0;
}

static void scg__decode_font_data(char *out, size_t out_len, const char *data) {
    const int len = strlen(data);

    uint8_t indices[4];
    uint8_t buf[3];
    size_t size = 0;

    for (int i = 0; i < len; i += 4) {
        indices[0] = scg__base64_index_from_char(data[i]);
        indices[1] = scg__base64_index_from_char(data[i + 1]);
        indices[2] = scg__base64_index_from_char(data[i + 2]);
        indices[3] = scg__base64_index_from_char(data[i + 3]);

        buf[0] = (indices[0] << 2) + ((indices[1] & 0x30) >> 4);
        buf[1] = ((indices[1] & 0xF) << 4) + ((indices[2] & 0x3C) >> 2);
        buf[2] = ((indices[2] & 0x3) << 6) + indices[3];

        for (int j = 0; j < 3; j++) {
            if (size == out_len) {
                return;
            }

            out[size++] = buf[j];
        }
    }
}

static const char scg__base64_table[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// Bitmap fonts taken from https://github.com/dhepper/font8x8.
static const char *scg__font8x8_data =
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGDw8GBgAGAA2NgAAAA"
    "AAADY2fzZ/"
    "NjYADD4DHjAfDAAAYzMYDGZjABw2HG47M24ABgYDAAAAAAAYDAYGBgwYAAYMGBgYDAYAAGY8/"
    "zxmAAAADAw/"
    "DAwAAAAAAAAADAwGAAAAPwAAAAAAAAAAAAwMAGAwGAwGAwEAPmNze29nPgAMDgwMDAw/"
    "AB4zMBwGMz8AHjMwHDAzHgA4PDYzfzB4AD8DHzAwMx4AHAYDHzMzHgA/"
    "MzAYDAwMAB4zMx4zMx4AHjMzPjAYDgAADAwAAAwMAAAMDAAADAwGGAwGAwYMGAAAAD8AAD8AAA"
    "YMGDAYDAYAHjMwGAwADAA+Y3t7ewMeAAweMzM/"
    "MzMAP2ZmPmZmPwA8ZgMDA2Y8AB82ZmZmNh8Af0YWHhZGfwB/"
    "RhYeFgYPADxmAwNzZnwAMzMzPzMzMwAeDAwMDAweAHgwMDAzMx4AZ2Y2HjZmZwAPBgYGRmZ/"
    "AGN3f39rY2MAY2dve3NjYwAcNmNjYzYcAD9mZj4GBg8AHjMzMzseOAA/"
    "ZmY+NmZnAB4zBw44Mx4APy0MDAwMHgAzMzMzMzM/"
    "ADMzMzMzHgwAY2Nja393YwBjYzYcHDZjADMzMx4MDB4Af2MxGExmfwAeBgYGBgYeAAMGDBgwYE"
    "AAHhgYGBgYHgAIHDZjAAAAAAAAAAAAAAD/"
    "DAwYAAAAAAAAAB4wPjNuAAcGBj5mZjsAAAAeMwMzHgA4MDA+MzNuAAAAHjM/"
    "Ax4AHDYGDwYGDwAAAG4zMz4wHwcGNm5mZmcADAAODAwMHgAwADAwMDMzHgcGZjYeNmcADgwMDA"
    "wMHgAAADN/"
    "f2tjAAAAHzMzMzMAAAAeMzMzHgAAADtmZj4GDwAAbjMzPjB4AAA7bmYGDwAAAD4DHjAfAAgMPg"
    "wMLBgAAAAzMzMzbgAAADMzMx4MAAAAY2t/"
    "fzYAAABjNhw2YwAAADMzMz4wHwAAPxkMJj8AOAwMBwwMOAAYGBgAGBgYAAcMDDgMDAcAbjsAAA"
    "AAAAAAAAAAAAAAAA==";

static const char *scg__font8x8_hiragana_data =
    "AAAAAAAAAAAEPwQ8Vk0mAAQ/"
    "BDxWTSYAAAAAESElAgAAAREhISUCAAAcABwiIBgAPAA8QkAgGAAcAD4QOCRiABwAPhA4JGIAJE"
    "8EPEZFIgAkTwQ8RkUiAAQkT1RSEgkARCQPVFJSCQAIHwg/"
    "HAI8AEQvBB8OAR4AEAgEAgQIEAAoRBIhAgQIAAAieSEhIhAAQCIRPRESCAAAADwAAgI8ACBAFi"
    "ABAQ4AEH4QPAICHAAkTxQuAQEOAAACAgJCIhwAIEISIgIiHAAQfhgUGBAMAEQvBgUGBAMAIHIv"
    "IhoCHACAUDoXGgIcAB4IBH8IBDgATyQCfwgEOAACDwJyAglxAEIvAnICCXEACH4IPEBAOABELw"
    "QeICAcAAAAABwiIBwAABwiQUAgHABAIB4hICAcAAA+"
    "CAQEBDgAAD5IJAQEOAAEBAg8AgI8AEQkCDwCAjwAMgInInIpEQAAAnoCCnICAAgJPktlVSIABA"
    "c0TGZUJAAAADxKSUUiAAAieiJyKhIAgFEdETkVCQBAsV0RORUJAAAAEzJREQ4AQCADMlERDgBA"
    "oEMyUREOABwACCpJEAwATCAIKkkQDABMoEgKKUgMAAAABAoRIEAAIEAUKhEgQAAgUCQKESBAAH"
    "0RfRE5VQkAnVEdETlVCQBdsV0ROVUJAH4IPggcKgQAAAckJH4lEgAED2QGBSY8AAAJPUpLRSoA"
    "Ag8CD2JCPAAAABIfIhIEAAASP0JCNAQAAAARPVM5EQAAET1TUTkRAAAIOAgcKgQACAg4CBwqBA"
    "AeAAI6RkIwAAAgIiIqJBAAHwg8QklUOAAEBwQMFlUkAD8QCDxCQTAAAAAIDjhMKgAEBwQ8RkUk"
    "AA4IPEppVTIABjxCOQQ2SQAEDwRuEQhwAAgIBAxWUiEAQC4APEJAOAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAgCBAAAAAAECgQAAAAAAAAAAICBAw"
    "DAAgQBQkCBgGAAAAAAAAAAAA";

#endif // SCG_IMPLEMENTATION
