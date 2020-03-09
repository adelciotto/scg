// Simple Computer Graphics (SCG) by Anthony Del Ciotto
//
// Copyright © 2020 Anthony Del Ciotto
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE file for more details.
//
// ABOUT:
//
// My WIP personal library for getting graphics on the screen quickly.
// It also plays sounds and does some other stuff too...
//
// TODO:
// configuration:
//  - define structure for application config
//	- read from file
//	- load screen config (width, height, fullscreen, etc...)
//	- load sound config (volume, etc..)
// color:
//  - color conversions (hsv, hsl, etc)
// drawing:
//	- basic circle
//	- draw primitives/images with transforms (scale, rotate, etc)
//		- provide with_transform functions for drawing polygons
//  - fix artifacts when rotating images
//	- linear gradients
// draw text:
//  - multiline
//  - different colored characters
// input:
//  - mouse
//  - gamepad
// simple audio - maybe simplify api?
//	- pause device
// examples:
// - basic (sine wave scroll text)

#ifndef INCLUDE_SCG_H
#define INCLUDE_SCG_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define float32_t float
#define float64_t double
#define bool_t unsigned int

#define SCG_TRUE 1
#define SCG_FALSE 0
#define SCG_PI 3.1415926535f

#define scg_log_error(...)                                                     \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_warn(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_info(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

typedef struct scg_return_status_t {
    bool_t is_error;
    const char *error_msg;
} scg_return_status_t;

extern inline scg_return_status_t
scg_return_status_failure(const char *error_msg);
extern inline scg_return_status_t scg_return_status_success(void);

extern inline int scg_min_int(int val, int min);
extern inline int scg_max_int(int val, int max);

extern inline float32_t scg_min_float32(float32_t val, float32_t min);
extern inline float32_t scg_max_float32(float32_t val, float32_t max);
extern inline float32_t scg_clamp_float32(float32_t val, float32_t min,
                                          float32_t max);
extern inline int scg_round_float32(float32_t val);

extern inline uint64_t scg_get_performance_counter(void);
extern inline uint64_t scg_get_performance_frequency(void);
extern inline float64_t scg_get_elapsed_time_secs(uint64_t end, uint64_t start);
extern inline float64_t scg_get_elapsed_time_millisecs(uint64_t end,
                                                       uint64_t start);

typedef struct scg_rectangle_t {
	int x;
	int y;
	int w;
	int h;
	int x0;
	int y0;
	int x1;
	int y1;
} scg_rectangle_t;

extern inline scg_rectangle_t scg_rectangle_create(int x, int y, int w, int h);

typedef struct scg_color_t {
    float32_t r;
    float32_t g;
    float32_t b;
    float32_t a;
} scg_color_t;

extern inline scg_color_t scg_color_create_from_rgb(float32_t r, float32_t g,
                                                    float32_t b);
extern inline scg_color_t scg_color_create_from_rgba(float32_t r, float32_t g,
                                                     float32_t b, float32_t a);

extern inline scg_color_t scg_color_create_from_uint8_rgb(uint8_t r, uint8_t g,
                                                          uint8_t b);
extern inline scg_color_t scg_color_create_from_uint8_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern inline void scg_color_to_rgba_uint8(scg_color_t color, uint8_t out[4]);

#define SCG_COLOR_WHITE scg_color_create_from_rgb(1.0f, 1.0f, 1.0f)
#define SCG_COLOR_BLACK scg_color_create_from_rgb(0.0f, 0.0f, 0.0f)
#define SCG_COLOR_RED scg_color_create_from_rgb(1.0f, 0.0f, 0.0f)
#define SCG_COLOR_GREEN scg_color_create_from_rgb(0.0f, 1.0f, 0.0f)
#define SCG_COLOR_BLUE scg_color_create_from_rgb(0.0f, 0.0f, 1.0f)
#define SCG_COLOR_YELLOW scg_color_create_from_rgb(1.0f, 1.0f, 0.0f)
#define SCG_COLOR_95_GREEN scg_color_create_from_rgb(0, 0.5f, 0.5f);
#define SCG_COLOR_TRANSPARENT scg_color_create_from_rgba(0.0f, 0.0f, 0.0f, 0.0f)

typedef struct scg_screen_t scg_screen_t;

typedef enum scg_blend_mode_t {
    SCG_BLEND_MODE_NONE = SDL_BLENDMODE_NONE,
    SCG_BLEND_MODE_BLEND = SDL_BLENDMODE_BLEND,
    SCG_BLEND_MODE_ADD = SDL_BLENDMODE_ADD,
    SCG_BLEND_MODE_MOD = SDL_BLENDMODE_MOD
} scg_blend_mode_t;

typedef enum scg_image_access_mode_t {
    SCG_IMAGE_ACCESS_MODE_STATIC = SDL_TEXTUREACCESS_STATIC,
    SCG_IMAGE_ACCESS_MODE_PIXEL = SDL_TEXTUREACCESS_STREAMING,
    SCG_IMAGE_ACCESS_MODE_TARGET = SDL_TEXTUREACCESS_TARGET
} scg_image_access_mode_t;

typedef struct scg_image_t {
    int width;
    int height;
    int pitch;
    SDL_Texture *sdl_texture;
    SDL_PixelFormat *pixel_format;
    scg_blend_mode_t blend_mode;
    scg_image_access_mode_t access_mode;
    scg_color_t mod_color;
    bool_t locked;
    uint32_t *pixels;
} scg_image_t;

extern inline int scg_pixel_index_from_xy(int x, int y, int w);

extern scg_return_status_t
scg_image_create(scg_image_t *image, scg_screen_t *screen, int width,
                 int height, scg_image_access_mode_t access_mode);
extern scg_return_status_t scg_image_create_from_bmp(scg_image_t *image,
                                                     scg_screen_t *screen,
                                                     const char *filepath);
extern scg_return_status_t scg_image_lock(scg_image_t *image);
extern void scg_image_unlock(scg_image_t *image);
extern scg_color_t scg_image_map_uint32_to_rgba(scg_image_t *image, uint32_t pixel);
extern uint32_t scg_image_map_rgba_to_uint32(scg_image_t *image,
                                             scg_color_t color);
extern void scg_image_set_blend_mode(scg_image_t *image,
                                     scg_blend_mode_t blend_mode);
extern void scg_image_set_mod_color(scg_image_t *image, scg_color_t color);
extern void scg_image_destroy(scg_image_t *image);

scg_return_status_t scg_screen_create(scg_screen_t *screen, const char *title,
                                      int width, int height, int scale,
                                      bool_t fullscreen);
extern bool_t scg_screen_is_running(scg_screen_t *screen);
extern void scg_screen_set_blend_mode(scg_screen_t *screen,
                                      scg_blend_mode_t blend_mode);
extern void scg_screen_set_draw_color(scg_screen_t *screen, scg_color_t color);
extern void scg_screen_clear(scg_screen_t *screen);
extern void scg_screen_draw_line(scg_screen_t *screen, int x0, int y0, int x1,
                                 int y1);
extern void scg_screen_draw_rect(scg_screen_t *screen, int x, int y, int w,
                                 int h, scg_color_t color);
extern void scg_screen_fill_rect(scg_screen_t *screen, int x, int y, int w,
                                 int h);
extern void scg_screen_draw_image(scg_screen_t *screen, scg_image_t *image, scg_rectangle_t src_rect, scg_rectangle_t dest_rect);
extern void scg_screen_draw_image_xy(scg_screen_t *screen, scg_image_t *image,
                                  int x, int y);
extern void scg_screen_draw_image_transform(scg_screen_t *screen,
                                                 scg_image_t *image, int x,
                                                 int y, float32_t angle,
                                                 float32_t scalex,
                                                 float32_t scaley);
extern void scg_screen_draw_string(scg_screen_t *screen, const char *str, int x,
                                   int y, bool_t anchor_to_center);
extern void scg_screen_draw_fps(scg_screen_t *screen);
extern void scg_screen_present(scg_screen_t *screen);
extern void scg_screen_log_info(scg_screen_t *screen);
extern void scg_screen_close(scg_screen_t *screen);
extern void scg_screen_destroy(scg_screen_t *screen);

typedef struct scg_sound_device_t scg_sound_device_t;
typedef struct scg_sound_t scg_sound_t;

extern scg_return_status_t
scg_sound_device_create(scg_sound_device_t *sound_device, int frames_per_sec);
extern void scg_sound_device_log_info(scg_sound_device_t *sound_device);
extern scg_return_status_t
scg_sound_create_from_wav(scg_sound_device_t *sound_device, scg_sound_t *sound,
                          const char *filepath, bool_t loop);
extern void scg_sound_play(scg_sound_t *sound);
extern void scg_sound_device_update(scg_sound_device_t *sound_device);
extern void scg_sound_device_destroy(scg_sound_device_t *sound_device);

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

extern void scg_keyboard_create(scg_keyboard_t *keyboard);
extern int scg_keyboard_is_key_down(scg_keyboard_t *keyboard,
                                    scg_key_code_t code);
extern int scg_keyboard_is_key_up(scg_keyboard_t *keyboard,
                                  scg_key_code_t code);
extern int scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
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
    int scale;
    scg_color_t draw_color;
    scg_blend_mode_t blend_mode;

    int target_frames_per_sec;
    float64_t target_time_per_frame_secs;
    uint64_t last_frame_counter;
    struct frame_metrics {
        float64_t update_counter;
        float64_t time_per_frame_secs;
        float64_t time_per_frame_millisecs;
        float64_t fps;
    } frame_metrics;

    bool_t running;

    scg_image_t bitmap_font;
    SDL_Rect font_chars[128];

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_PixelFormat *pixel_format;
};

struct scg_sound_t {
    int device_id;
    SDL_AudioSpec sdl_spec;
    uint32_t length;
    uint8_t *buffer;
    int play_offset;
    uint8_t *end_position;
    bool_t is_playing;
    bool_t loop;
};

struct scg_sound_device_t {
    SDL_AudioDeviceID device_id;
    int freq;
    uint8_t channels;
    uint16_t samples;
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

//
// scg_return_status_failure implementation
//

inline scg_return_status_t scg_return_status_failure(const char *error_msg) {
    return (scg_return_status_t){SCG_TRUE, error_msg};
}

//
// scg_return_status_success implementation
//

inline scg_return_status_t scg_return_status_success(void) {
    return (scg_return_status_t){SCG_FALSE, ""};
}

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

inline int scg_min_int(int val, int min) {
    return val < min ? val : min;
}

//
// scg_max_int implementation
//

inline int scg_max_int(int val, int max) {
    return val > max ? val : max;
}

//
// scg_min_float32 implementation
//

inline float32_t scg_min_float32(float32_t val, float32_t min) {
    return val < min ? val : min;
}

//
// scg_max_float32 implementation
//

inline float32_t scg_max_float32(float32_t val, float32_t max) {
    return val > max ? val : max;
}

//
// scg_clamp_float32 implementation
//

inline float32_t scg_clamp_float32(float32_t val, float32_t min,
                                   float32_t max) {
    const float32_t t = val < min ? min : val;
    return t > max ? max : t;
}

//
// scg_round_float32 implementation
//

inline int scg_round_float32(float32_t val) {
    return (int)(val + 0.5f);
}

//
// scg_get_performance_counter implementation
//

inline Uint64 scg_get_performance_counter(void) {
    return SDL_GetPerformanceCounter();
}

//
// scg_get_performance_frequency implementation
//

inline Uint64 scg_get_performance_frequency(void) {
    return SDL_GetPerformanceFrequency();
}

//
// scg_get_elapsed_time_secs implementation
//

inline float64_t scg_get_elapsed_time_secs(uint64_t end, uint64_t start) {
    return (float64_t)(end - start) /
           (float64_t)scg_get_performance_frequency();
}

//
// scg_get_elapsed_time_millisecs implementation
//

inline float64_t scg_get_elapsed_time_millisecs(uint64_t end, uint64_t start) {
    return (float64_t)((end - start) * 1000) /
           (float64_t)scg_get_performance_frequency();
}

//
// scg_rectangle_create implementation
//

inline scg_rectangle_t scg_rectangle_create(int x, int y, int w, int h) {
	return (scg_rectangle_t){
		.x = x,
		.y = y,
		.w = w,
		.h = h,
		.x0 = x,
		.y0 = y,
		.x1 = x + w,
		.y1 = y + h
	};
}

//
// scg_color_create_from_rgb implementation
//

inline scg_color_t scg_color_create_from_rgb(float32_t r, float32_t g,
                                             float32_t b) {
    return (scg_color_t){r, g, b, 1.0f};
}

//
// scg_color_create_from_rgba implementation
//

inline scg_color_t scg_color_create_from_rgba(float32_t r, float32_t g,
                                              float32_t b, float32_t a) {
    return (scg_color_t){r, g, b, a};
}

//
// scg_color_create_from_uint8_rgb implementation
//

inline scg_color_t scg_color_create_from_uint8_rgb(uint8_t r, uint8_t g,
                                                   uint8_t b) {
    float32_t rf = (float32_t)r / 255.0f;
    float32_t gf = (float32_t)g / 255.0f;
    float32_t bf = (float32_t)b / 255.0f;

    return scg_color_create_from_rgb(rf, gf, bf);
}


//
// scg_color_create_from_uint8_rgba implementation
//

inline scg_color_t scg_color_create_from_uint8_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float32_t rf = (float32_t)r / 255.0f;
    float32_t gf = (float32_t)g / 255.0f;
    float32_t bf = (float32_t)b / 255.0f;
    float32_t af = (float32_t)a / 255.0f;

    return scg_color_create_from_rgba(rf, gf, bf, af);
}

//
// scg_color_to_rgba_uint8 implementation
//

inline void scg_color_to_rgba_uint8(scg_color_t color, uint8_t out[4]) {
    out[0] = (uint8_t)scg_min_float32(color.r * 255.0f, 255.0f);
    out[1] = (uint8_t)scg_min_float32(color.g * 255.0f, 255.0f);
    out[2] = (uint8_t)scg_min_float32(color.b * 255.0f, 255.0f);
    out[3] = (uint8_t)scg_min_float32(color.a * 255.0f, 255.0f);
}

//
// scg_pixel_index_from_xy implementation
//

inline int scg_pixel_index_from_xy(int x, int y, int w) {
    return (y * w) + x;
}

//
// scg_image_create implementation
//

scg_return_status_t scg_image_create(scg_image_t *image, scg_screen_t *screen,
                                     int w, int h,
                                     scg_image_access_mode_t access_mode) {
    SDL_Texture *texture = SDL_CreateTexture(
        screen->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, access_mode, w, h);
    if (texture == NULL) {
        const char *error_msg = scg__sprintf(
            "Failed to create texture for image. %s", SDL_GetError());
        return scg_return_status_failure(error_msg);
    }

    SDL_BlendMode sdl_blend_mode;
    if (SDL_GetTextureBlendMode(texture, &sdl_blend_mode) != 0) {
        scg_log_warn("Could not get image blend mode. Error: %s",
                     SDL_GetError());
        sdl_blend_mode = SCG_BLEND_MODE_NONE;
    }
    scg_blend_mode_t blend_mode = sdl_blend_mode;

    uint8_t r, g, b;
    if (SDL_GetTextureColorMod(texture, &r, &g, &b) != 0) {
        scg_log_warn("Could not get image mod color. Error: %s",
                     SDL_GetError());
        r = g = b = 255;
    }
    scg_color_t mod_color = scg_color_create_from_uint8_rgb(r, g, b);

    image->width = w;
    image->height = h;
    image->sdl_texture = texture;
    image->pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    image->access_mode = access_mode;
    image->blend_mode = blend_mode;
    image->mod_color = mod_color;
    image->locked = SCG_FALSE;
    image->pitch = w * 4;
    image->pixels = NULL;

    return scg_return_status_success();
}

//
// scg_image_create_from_bmp implementation
//

scg_return_status_t scg_image_create_from_bmp(scg_image_t *image,
                                              scg_screen_t *screen,
                                              const char *filepath) {
    SDL_Surface *surface = SDL_LoadBMP(filepath);
    if (surface == NULL) {
        const char *error_msg = scg__sprintf("Failed to load %s image file. %s",
                                             filepath, SDL_GetError());
        return scg_return_status_failure(error_msg);
    }

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(screen->sdl_renderer, surface);
    if (texture == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to create texture for %s image file. %s",
                         filepath, SDL_GetError());
        return scg_return_status_failure(error_msg);
    }

    SDL_BlendMode sdl_blend_mode;
    if (SDL_GetTextureBlendMode(texture, &sdl_blend_mode) != 0) {
        scg_log_warn("Could not get image blend mode. Error: %s",
                     SDL_GetError());
        sdl_blend_mode = SCG_BLEND_MODE_NONE;
    }
    scg_blend_mode_t blend_mode = sdl_blend_mode;

    scg_color_t mod_color;
    uint8_t r, g, b;
    if (SDL_GetTextureColorMod(texture, &r, &g, &b) != 0) {
        scg_log_warn("Could not get image mod color. Error: %s",
                     SDL_GetError());
        r = g = b = 255;
    }
    mod_color = scg_color_create_from_uint8_rgb(r, g, b);

	size_t num_bytes = surface->w * surface->h * sizeof(uint32_t); //surface->pitch * surface->h;
	uint32_t *pixels = malloc(num_bytes);
	memcpy(pixels, surface->pixels, num_bytes);

    image->width = surface->w;
    image->height = surface->h;
    image->sdl_texture = texture;
    image->pixel_format = SDL_AllocFormat(surface->format->format);
    image->access_mode = SCG_IMAGE_ACCESS_MODE_STATIC;
    image->blend_mode = blend_mode;
    image->mod_color = mod_color;
    image->locked = SCG_FALSE;
    image->pitch = surface->pitch;
    image->pixels = pixels;

    SDL_FreeSurface(surface);

    return scg_return_status_success();
}

//
// scg_image_lock implementation
//

scg_return_status_t scg_image_lock(scg_image_t *image) {
    if (image->locked == SCG_TRUE) {
        return scg_return_status_success();
    }

    void *pixels = NULL;
    int pitch;
    if (SDL_LockTexture(image->sdl_texture, NULL, &pixels, &pitch) != 0) {
        const char *error_msg = scg__sprintf(
            "Failed to lock texture for image. %s", SDL_GetError());
        return scg_return_status_failure(error_msg);
    }

    image->pixels = (uint32_t *)pixels;
    image->pitch = pitch;
    image->locked = SCG_TRUE;

    return scg_return_status_success();
}

//
// scg_image_unlock implementation
//

void scg_image_unlock(scg_image_t *image) {
    if (image->locked == SCG_FALSE) {
        return;
    }

    SDL_UnlockTexture(image->sdl_texture);

    image->pixels = NULL;
    image->locked = SCG_FALSE;
}

//
// scg_image_map_uint32_to_rgba implementation
//

scg_color_t scg_image_map_uint32_to_rgba(scg_image_t *image, uint32_t pixel) {
	uint8_t r, g, b, a;
	SDL_GetRGBA(pixel, image->pixel_format, &r, &g, &b, &a);

	return scg_color_create_from_uint8_rgba(r, g, b, a);
}

//
// scg_image_map_rgba_to_uint32 implementation
//

uint32_t scg_image_map_rgba_to_uint32(scg_image_t *image, scg_color_t color) {
    uint8_t rgba[4];
    scg_color_to_rgba_uint8(color, rgba);

    return SDL_MapRGBA(image->pixel_format, rgba[0], rgba[1], rgba[2], rgba[3]);
}

//
// scg_image_set_blend_mode implementation
//

void scg_image_set_blend_mode(scg_image_t *image, scg_blend_mode_t blend_mode) {
    image->blend_mode = blend_mode;

    if (SDL_SetTextureBlendMode(image->sdl_texture, blend_mode) != 0) {
        scg_log_warn("Could not set the image blend mode. Error: %s",
                     SDL_GetError());
    }
}

//
// scg_image_set_mod_color implementation
//

void scg_image_set_mod_color(scg_image_t *image, scg_color_t color) {
    image->mod_color = color;

    uint8_t rgba[4];
    scg_color_to_rgba_uint8(color, rgba);

    if (SDL_SetTextureColorMod(image->sdl_texture, rgba[0], rgba[1], rgba[2]) !=
        0) {
        scg_log_warn("Could not set the image mod color. Error: %s",
                     SDL_GetError());
    }
}

//
// scg_image_destroy implementation
//

void scg_image_destroy(scg_image_t *image) {
	if (image->pixels != NULL && image->locked == SCG_FALSE) {
		free(image->pixels);
	}

    SDL_FreeFormat(image->pixel_format);
    SDL_DestroyTexture(image->sdl_texture);
}

static int scg__get_monitor_refresh_rate(SDL_DisplayMode display_mode) {
    int result = display_mode.refresh_rate;

    if (result == 0) {
        return SCG_DEFAULT_REFRESH_RATE;
    }

    return result;
}

static scg_return_status_t scg__init_bitmap_font(scg_screen_t *screen) {
    scg_image_t bitmap_font;
    scg_return_status_t return_status = scg_image_create(
        &bitmap_font, screen, 128, 64, SCG_IMAGE_ACCESS_MODE_PIXEL);
    if (return_status.is_error) {
        return return_status;
    }

    return_status = scg_image_lock(&bitmap_font);
    if (return_status.is_error) {
        return return_status;
    }

    int char_code = 0;
    int w = bitmap_font.width;
    int h = bitmap_font.height;

    for (int y = 0; y < h; y += SCG_FONT_SIZE) {
        for (int x = 0; x < w; x += SCG_FONT_SIZE) {
            SDL_Rect rect = (SDL_Rect){
                .x = x, .y = y, .w = SCG_FONT_SIZE, .h = SCG_FONT_SIZE};
            const char *char_bitmap = scg__font8x8[char_code];

            screen->font_chars[char_code] = rect;

            for (int i = 0; i < SCG_FONT_SIZE; i++) {
                for (int j = 0; j < SCG_FONT_SIZE; j++) {
                    int set = char_bitmap[i] & 1 << j;
                    int index = scg_pixel_index_from_xy(x + j, y + i, w);

                    scg_color_t color =
                        set ? SCG_COLOR_WHITE : SCG_COLOR_TRANSPARENT;
                    bitmap_font.pixels[index] =
                        scg_image_map_rgba_to_uint32(&bitmap_font, color);
                }
            }

            char_code++;
        }
    }

    scg_image_unlock(&bitmap_font);

    scg_image_set_blend_mode(&bitmap_font, SCG_BLEND_MODE_BLEND);

    screen->bitmap_font = bitmap_font;
    return scg_return_status_success();
}

//
// scg_screen_create implementation
//

scg_return_status_t scg_screen_create(scg_screen_t *screen, const char *title,
                                      int width, int height, int scale,
                                      bool_t fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        return scg_return_status_failure(SDL_GetError());
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
        return scg_return_status_failure(SDL_GetError());
    }

    SDL_Window *sdl_window = SDL_CreateWindow(
        title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale,
        height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL) {
        return scg_return_status_failure(SDL_GetError());
    }

    if (fullscreen == SCG_TRUE) {
        SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(
        sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) {
        return scg_return_status_failure(SDL_GetError());
    }

    SDL_RenderSetLogicalSize(sdl_renderer, width, height);

    screen->title = title;
    screen->width = width;
    screen->height = height;
    screen->scale = scale;
    screen->target_frames_per_sec = scg__get_monitor_refresh_rate(display_mode);
    screen->target_time_per_frame_secs =
        1.0 / (float64_t)screen->target_frames_per_sec;
    screen->last_frame_counter = scg_get_performance_counter();
    screen->sdl_window = sdl_window;
    screen->sdl_renderer = sdl_renderer;
    screen->frame_metrics.update_counter = scg_get_performance_counter();
    screen->running = SCG_TRUE;

    scg_return_status_t return_status = scg__init_bitmap_font(screen);
    if (return_status.is_error) {
        return return_status;
    }

    scg_screen_set_draw_color(screen, SCG_COLOR_WHITE);
    scg_screen_set_blend_mode(screen, SCG_BLEND_MODE_BLEND);

    return scg_return_status_success();
}

//
// scg_screen_is_running implementation
//

bool_t scg_screen_is_running(scg_screen_t *screen) {
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        screen->running = SCG_FALSE;
        break;
    }

    return screen->running == SCG_TRUE;
}

//
// scg_screen_set_blend_mode implementation
//

void scg_screen_set_blend_mode(scg_screen_t *screen,
                               scg_blend_mode_t blend_mode) {
    screen->blend_mode = blend_mode;

    if (SDL_SetRenderDrawBlendMode(screen->sdl_renderer, blend_mode) != 0) {
        scg_log_warn("Could not set the screen blend mode. Error: %s",
                     SDL_GetError());
    }
}

//
// scg_screen_set_draw_color implementation
//

void scg_screen_set_draw_color(scg_screen_t *screen, scg_color_t color) {
    screen->draw_color = color;

    scg_image_set_mod_color(&screen->bitmap_font, color);

    uint8_t rgba[4];
    scg_color_to_rgba_uint8(color, rgba);
    if (SDL_SetRenderDrawColor(screen->sdl_renderer, rgba[0], rgba[1], rgba[2],
                               rgba[3]) != 0) {
        scg_log_warn("Could not set the screen draw color. Error: %s",
                     SDL_GetError());
    }
}

//
// scg_screen_clear implementation
//

void scg_screen_clear(scg_screen_t *screen) {
    SDL_RenderClear(screen->sdl_renderer);
}

//
// scg_screen_draw_line implementation
//

void scg_screen_draw_line(scg_screen_t *screen, int x0, int y0, int x1,
		int y1) {
	SDL_RenderDrawLine(screen->sdl_renderer, x0, y0, x1, y1);
}

//
// scg_screen_fill_rect implementation
//

void scg_screen_fill_rect(scg_screen_t *screen, int x, int y, int w, int h) {
    SDL_Rect rect = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};

    SDL_RenderFillRect(screen->sdl_renderer, &rect);
}

//
// scg_screen_draw_image implementation
//

void scg_screen_draw_image(scg_screen_t *screen, scg_image_t *image, scg_rectangle_t src_rect, scg_rectangle_t dest_rect) {
	SDL_Rect sdl_src_rect = (SDL_Rect){.x = src_rect.x, .y = src_rect.y, .w = src_rect.w, .h = src_rect.h};
	SDL_Rect sdl_dest_rect = (SDL_Rect){.x = dest_rect.x, .y = dest_rect.y, .w = dest_rect.w, .h = dest_rect.h};

    SDL_RenderCopy(screen->sdl_renderer, image->sdl_texture, &sdl_src_rect, &sdl_dest_rect);
}

//
// scg_screen_draw_image_xy implementation
//

void scg_screen_draw_image_xy(scg_screen_t *screen, scg_image_t *image, int x,
                           int y) {
    SDL_Rect dest_rect =
        (SDL_Rect){.x = x, .y = y, .w = image->width, .h = image->height};

    SDL_RenderCopy(screen->sdl_renderer, image->sdl_texture, NULL, &dest_rect);
}

//
// scg_screen_draw_image_transform implementation
//

void scg_screen_draw_image_transform(scg_screen_t *screen,
                                          scg_image_t *image, int x, int y,
                                          float32_t angle, float32_t sx,
                                          float32_t sy) {
    int w = (int)((float32_t)image->width * sx);
    int h = (int)((float32_t)image->height * sy);

    SDL_Rect dest_rect = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};

    SDL_RenderCopyEx(screen->sdl_renderer, image->sdl_texture, NULL, &dest_rect,
                     angle, NULL, SDL_FLIP_NONE);
}

static int scg__string_width(const char *str, int size) {
    return strlen(str) * size;
}

//
// scg_screen_draw_string implementation
//

void scg_screen_draw_string(scg_screen_t *screen, const char *str, int x, int y,
                            bool_t anchor_to_center) {
    int current_x = x;
    int current_y = y;

    if (anchor_to_center == SCG_TRUE) {
        int width = scg__string_width(str, SCG_FONT_SIZE);
        current_x = x - width / 2;
        current_y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') {
            int char_code = (int)str[i];
            if (char_code < 0 || char_code > 127) {
                char_code = 63; // draw '?' for unknown char */
            }

            SDL_Rect dest_rect = (SDL_Rect){.x = current_x,
                                            .y = current_y,
                                            .w = SCG_FONT_SIZE,
                                            .h = SCG_FONT_SIZE};
            SDL_RenderCopy(screen->sdl_renderer,
                           screen->bitmap_font.sdl_texture,
                           &screen->font_chars[char_code], &dest_rect);
        }

        current_x += SCG_FONT_SIZE + 1;
    }
}

//
// scg_screen_draw_fps implementation
//

void scg_screen_draw_fps(scg_screen_t *screen) {
    float32_t fps = screen->frame_metrics.fps;
    ssize_t bsize = snprintf(NULL, 0, "fps:%.2f", fps);
    char buffer[bsize];
    snprintf(buffer, bsize + 1, "fps:%.2f", fps);

    scg_color_t current_draw_color = screen->draw_color;

    scg_color_t text_color = SCG_COLOR_GREEN;
    float32_t target_fps = (float32_t)screen->target_frames_per_sec;
    if (fps < target_fps * 0.95) {
        text_color = SCG_COLOR_YELLOW;
    }
    if (fps < target_fps * 0.5) {
        text_color = SCG_COLOR_RED;
    }

    scg_screen_set_draw_color(screen, text_color);
    scg_screen_draw_string(screen, buffer, 10, 10, SCG_FALSE);
    scg_screen_set_draw_color(screen, current_draw_color);
}

//
// scg_screen_present implementation
//

void scg_screen_present(scg_screen_t *screen) {
    // Wait until we have reached the target amount of time per frame (e.g 60hz,
    // ~16ms). Spinning in a while loop seems to be the most accurate way to do
    // this, as trying to use SDL_Delay (sleeping) is dependant on other
    // factors.
    float64_t target_secs = screen->target_time_per_frame_secs;
    float64_t elapsed_time_secs = scg_get_elapsed_time_secs(
        scg_get_performance_counter(), screen->last_frame_counter);

    if (elapsed_time_secs < target_secs) {
        while (elapsed_time_secs < target_secs) {
            elapsed_time_secs = scg_get_elapsed_time_secs(
                scg_get_performance_counter(), screen->last_frame_counter);
        }
    }

    uint64_t end_frame_counter = scg_get_performance_counter();

    SDL_RenderPresent(screen->sdl_renderer);

    if (scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                  screen->frame_metrics.update_counter) >=
        1.0f) {
        screen->frame_metrics.time_per_frame_secs = scg_get_elapsed_time_secs(
            end_frame_counter, screen->last_frame_counter);
        screen->frame_metrics.time_per_frame_millisecs =
            scg_get_elapsed_time_millisecs(end_frame_counter,
                                           screen->last_frame_counter);
        screen->frame_metrics.fps =
            (float64_t)scg_get_performance_frequency() /
            (float64_t)(end_frame_counter - screen->last_frame_counter);
        screen->frame_metrics.update_counter = scg_get_performance_counter();
    }

    screen->last_frame_counter = end_frame_counter;
}

//
// scg_screen_log_info implementation
//

void scg_screen_log_info(scg_screen_t *screen) {
    scg_log_info("screen has w:%d, h:%d, scale:%d, target fps:%d",
                 screen->width, screen->height, screen->scale,
                 screen->target_frames_per_sec);
}

//
// scg_screen_close implementation
//

void scg_screen_close(scg_screen_t *screen) {
    screen->running = SCG_FALSE;
}

//
// scg_screen_destroy implementation
//

void scg_screen_destroy(scg_screen_t *screen) {
    scg_image_destroy(&screen->bitmap_font);
    SDL_DestroyRenderer(screen->sdl_renderer);
    SDL_DestroyWindow(screen->sdl_window);

    SDL_Quit();
}

//
// scg_sound_device_create implementation
//

scg_return_status_t scg_sound_device_create(scg_sound_device_t *sound_device,
                                            int frames_per_sec) {
    SDL_AudioSpec want, have;

    int channels = 2;
    size_t bytes_per_sample = sizeof(int16_t) * channels;

    memset(&want, 0, sizeof(want));
    want.freq = 48000;
    want.format = AUDIO_S16LSB;
    want.channels = channels;
    want.samples = want.freq * bytes_per_sample / frames_per_sec;
    want.callback = NULL;

    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(
        NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (device_id == 0) {
        return scg_return_status_failure(SDL_GetError());
    }

    if (have.format != want.format) {
        return scg_return_status_failure(
            "Audio device does not support format S16LSB");
    }

    sound_device->device_id = device_id;
    sound_device->freq = have.freq;
    sound_device->channels = have.channels;
    sound_device->samples = have.samples;
    sound_device->bytes_per_sample = bytes_per_sample;
    sound_device->latency_sample_count = sound_device->freq / 15;
    sound_device->buffer_size =
        sound_device->latency_sample_count * bytes_per_sample;

    sound_device->buffer =
        (uint8_t *)calloc(sound_device->latency_sample_count, bytes_per_sample);
    if (sound_device->buffer == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to allocate memory for sound buffer. bytes=%d",
                         sound_device->latency_sample_count * bytes_per_sample);
        return scg_return_status_failure(error_msg);
    }

    sound_device->num_sounds = 0;

    SDL_PauseAudioDevice(device_id, 0);

    return scg_return_status_success();
}

//
// scg_sound_device_log_info implementation
//

void scg_sound_device_log_info(scg_sound_device_t *sound_device) {
    scg_log_info("sound device has id:%d, channels:%d, samples/sec:%d, "
                 "samples/frame:%d, bytes/sample:%d",
                 sound_device->device_id, sound_device->channels,
                 sound_device->freq, sound_device->latency_sample_count,
                 sound_device->bytes_per_sample);
}

//
// scg_sound_create implementation
//

scg_return_status_t scg_sound_create_from_wav(scg_sound_device_t *sound_device,
                                              scg_sound_t *sound,
                                              const char *filepath,
                                              bool_t loop) {
    if (sound_device->num_sounds == SCG_MAX_SOUNDS) {
        return scg_return_status_failure("Maximum sounds reached");
    }

    SDL_AudioSpec spec;
    uint32_t length;
    uint8_t *buffer;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == NULL) {
        return scg_return_status_failure(SDL_GetError());
    }

    sound->sdl_spec = spec;
    sound->length = length;
    sound->buffer = buffer;
    sound->play_offset = 0;
    sound->end_position = buffer + length;
    sound->is_playing = SCG_FALSE;
    sound->loop = loop;

    sound_device->sounds[sound_device->num_sounds++] = sound;

    return scg_return_status_success();
}

//
// scg_sound_play implementation
//

void scg_sound_play(scg_sound_t *sound) {
    if (sound->is_playing == SCG_FALSE) {
        sound->is_playing = SCG_TRUE;
    }
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

        if (sound->is_playing == SCG_TRUE) {
            if (sound->buffer + sound->play_offset >= sound->end_position) {
                if (sound->loop == SCG_FALSE) {
                    sound->is_playing = 0;
                }
                sound->play_offset = 0;
            } else {
                uint32_t temp_len =
                    bytes_to_write > (sound->length - sound->play_offset)
                        ? (sound->length - sound->play_offset)
                        : bytes_to_write;
                SDL_MixAudioFormat(
                    sound_device->buffer, sound->buffer + sound->play_offset,
                    AUDIO_S16LSB, temp_len, SDL_MIX_MAXVOLUME / 2);
                sound->play_offset += temp_len;
            }
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
// scg_keyboard_create implementation
//

void scg_keyboard_create(scg_keyboard_t *keyboard) {
    keyboard->current_key_states = SDL_GetKeyboardState(NULL);
    memset(keyboard->previous_key_states, 0,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);
}

//
// scg_keyboard_is_key_down implementation
//

int scg_keyboard_is_key_down(scg_keyboard_t *keyboard, scg_key_code_t key) {
    return keyboard->current_key_states[key] == 1;
}

//
// scg_keyboard_is_key_up implementation
//

int scg_keyboard_is_key_up(scg_keyboard_t *keyboard, scg_key_code_t key) {
    return keyboard->current_key_states[key] == 0;
}

//
// scg_keyboard_is_key_triggered implementation
//

int scg_keyboard_is_key_triggered(scg_keyboard_t *keyboard,
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
