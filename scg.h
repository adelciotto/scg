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

typedef struct scg_return_status {
    bool_t is_error;
    const char *error_msg;
} scg_return_status;

extern void scg_swap_int(int *a, int *b);
extern void scg_swap_float32(float32_t *a, float32_t *b);
extern int scg_min_int(int val, int min);
extern int scg_max_int(int val, int max);
extern float32_t scg_min_float32(float32_t val, float32_t min);
extern float32_t scg_max_float32(float32_t val, float32_t max);
extern float32_t scg_clamp_float32(float32_t val, float32_t min, float32_t max);

extern uint64_t scg_get_performance_counter(void);
extern uint64_t scg_get_performance_frequency(void);
extern float64_t scg_get_elapsed_time_secs(uint64_t end, uint64_t start);
extern float64_t scg_get_elapsed_time_millisecs(uint64_t end, uint64_t start);

typedef union scg_pixel {
    uint32_t packed;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } color;
} scg_pixel;

extern scg_pixel scg_pixel_create(uint8_t r, uint8_t g, uint8_t b);
extern scg_pixel scg_pixel_create_from_uint32(uint32_t packed);

#define SCG_COLOR_WHITE scg_pixel_create(255, 255, 255)
#define SCG_COLOR_BLACK scg_pixel_create(0, 0, 0)
#define SCG_COLOR_RED scg_pixel_create(255, 0, 0)
#define SCG_COLOR_GREEN scg_pixel_create(0, 255, 0)
#define SCG_COLOR_BLUE scg_pixel_create(0, 0, 255)
#define SCG_COLOR_YELLOW scg_pixel_create(255, 255, 0)
#define SCG_COLOR_95_GREEN scg_pixel_create(0, 128, 128);
#define SCG_COLOR_ICE_BLUE scg_pixel_create(153, 255, 255);

typedef struct scg_image {
    int width;
    int height;
    uint32_t *pixels;
} scg_image;

extern scg_return_status scg_image_create_from_tga(scg_image *image,
                                                   const char *filepath);
extern void scg_image_destroy(scg_image *image);

typedef enum scg_blend_mode {
    SCG_BLEND_MODE_NONE,
    SCG_BLEND_MODE_MASK,
    SCG_BLEND_MODE_ALPHA
} scg_blend_mode;

typedef struct scg_screen scg_screen;

scg_return_status scg_screen_create(scg_screen *screen, const char *title,
                                    int width, int height, int scale,
                                    bool_t fullscreen);
extern bool_t scg_screen_is_running(scg_screen *screen);
extern void scg_screen_set_blend_mode(scg_screen *screen,
                                      scg_blend_mode blend_mode);
extern void scg_screen_set_pixel(scg_screen *screen, int x, int y,
                                 scg_pixel pixel);
extern void scg_screen_clear(scg_screen *screen, scg_pixel color);
extern void scg_screen_draw_line(scg_screen *screen, int px0, int py0, int px1,
                                 int py1, scg_pixel color);
extern void scg_screen_draw_rect(scg_screen *screen, int px, int py, int width,
                                 int height, scg_pixel color);
extern void scg_screen_fill_rect(scg_screen *screen, int px, int py, int width,
                                 int height, scg_pixel color);
// Maybe a temporary function...
extern void scg_polygon_create_points(int num_points,
                                      float32_t out[num_points][2],
                                      float32_t radius);
extern void scg_screen_draw_polygon(scg_screen *screen, int px, int py,
                                    float32_t points[][2], int num_points,
                                    scg_pixel color);
extern void scg_screen_draw_image(scg_screen *screen, scg_image image, int px,
                                  int py);
extern void scg_screen_draw_image_with_transform(scg_screen *screen,
                                                 scg_image image, int px,
                                                 int py, float32_t angle,
                                                 float32_t sx, float32_t sy);
extern void scg_screen_draw_string(scg_screen *screen, const char *str, int x,
                                   int y, int anchor_to_center,
                                   scg_pixel color);
extern void scg_screen_draw_fps(scg_screen *screen);
extern void scg_screen_present(scg_screen *screen);
extern void scg_screen_log_info(scg_screen *screen);
extern void scg_screen_close(scg_screen *screen);
extern void scg_screen_destroy(scg_screen *screen);

typedef struct scg_sound_device scg_sound_device;
typedef struct scg_sound scg_sound;

extern scg_return_status scg_sound_device_create(scg_sound_device *sound_device,
                                                 int frames_per_sec);
extern void scg_sound_device_log_info(scg_sound_device *sound_device);
extern scg_return_status
scg_sound_create_from_wav(scg_sound_device *sound_device, scg_sound *sound,
                          const char *filepath, bool_t loop);
extern void scg_sound_play(scg_sound *sound);
extern void scg_sound_device_update(scg_sound_device *sound_device);
extern void scg_sound_device_destroy(scg_sound_device *sound_device);

typedef enum scg_key_code {
    SCG_KEY_UP = SDL_SCANCODE_UP,
    SCG_KEY_DOWN = SDL_SCANCODE_DOWN,
    SCG_KEY_LEFT = SDL_SCANCODE_LEFT,
    SCG_KEY_RIGHT = SDL_SCANCODE_RIGHT,
    SCG_KEY_X = SDL_SCANCODE_X,
    SCG_KEY_C = SDL_SCANCODE_C,
    SCG_KEY_Z = SDL_SCANCODE_Z,
    SCG_KEY_SPACE = SDL_SCANCODE_SPACE,
    SCG_KEY_ESCAPE = SDL_SCANCODE_ESCAPE
} scg_key_code;

typedef struct scg_keyboard scg_keyboard;

extern void scg_keyboard_create(scg_keyboard *keyboard);
extern int scg_keyboard_is_key_down(scg_keyboard *keyboard, scg_key_code code);
extern int scg_keyboard_is_key_up(scg_keyboard *keyboard, scg_key_code code);
extern int scg_keyboard_is_key_triggered(scg_keyboard *keyboard,
                                         scg_key_code code);
extern void scg_keyboard_update(scg_keyboard *keyboard);

///////////////
//
// Everything below here is implementation details
//

#define SCG_MAX_SOUNDS 16 // TODO: use linked list for sounds?

struct scg_screen {
    const char *title;
    int width;
    int height;
    int scale;
    scg_blend_mode blend_mode;

    int target_frames_per_sec;
    float64_t target_time_per_frame_secs;
    uint64_t last_frame_counter;
    struct frame_metrics {
        float64_t update_counter;
        float64_t time_per_frame_secs;
        float64_t time_per_frame_millisecs;
        float64_t fps;
    } frame_metrics;

    bool_t is_running;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
    uint32_t *pixels;
    int pitch;
    int total_num_pixels;
};

struct scg_sound {
    int device_id;
    SDL_AudioSpec sdl_spec;
    uint32_t length;
    uint8_t *buffer;
    int play_offset;
    uint8_t *end_position;
    bool_t is_playing;
    bool_t loop;
};

struct scg_sound_device {
    SDL_AudioDeviceID device_id;
    int freq;
    uint8_t channels;
    uint16_t samples;
    int bytes_per_sample;
    int latency_sample_count;
    uint32_t buffer_size;
    uint8_t *buffer;

    scg_sound *sounds[SCG_MAX_SOUNDS];
    int num_sounds;
};

struct scg_keyboard {
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

static scg_return_status scg__return_status_failure(const char *error_msg) {
    return (scg_return_status){SCG_TRUE, error_msg};
}

static scg_return_status scg__return_status_success(void) {
    return (scg_return_status){SCG_FALSE, ""};
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
// scg_swap_int implementation
//

void scg_swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

//
// scg_swap_float32 implementation
//

void scg_swap_float32(float32_t *a, float32_t *b) {
    float32_t temp = *a;
    *a = *b;
    *b = temp;
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
// scg_clamp_float32 implementation
//

float32_t scg_clamp_float32(float32_t val, float32_t min, float32_t max) {
    if (val < min) {
        return min;
    }

    if (val > max) {
        return max;
    }

    return val;
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
// scg_pixel_create implementation
//

scg_pixel scg_pixel_create(uint8_t r, uint8_t g, uint8_t b) {
    scg_pixel pixel;
    pixel.color.r = r;
    pixel.color.g = g;
    pixel.color.b = b;
    pixel.color.a = 255;

    return pixel;
}

//
// scg_pixel_create_from_uint32 implementation
//

scg_pixel scg_pixel_create_from_uint32(uint32_t color) {
    scg_pixel pixel;
    pixel.packed = color;

    return pixel;
}

static unsigned char *scg__read_file(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    unsigned char *file_contents = malloc(size);
    if (file_contents == NULL) {
        return NULL;
    }

    size_t bytes_read = fread(file_contents, 1, size, file);
    if (bytes_read != size) {
        return NULL;
    }

    fclose(file);

    return file_contents;
}

static int scg__map_row_col_to_index(int col, int row, int num_cols) {
    return row * num_cols + col;
}

#pragma pack(push, 1)
typedef struct scg__tga_header {
    uint8_t id_len;
    uint8_t color_map_type;
    uint8_t image_type_code;
    uint16_t color_map_origin;
    uint16_t color_map_length;
    uint8_t color_map_num_bits;
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;
    uint8_t pixel_size;
    uint8_t descriptor_byte;
} scg__tga_header;
#pragma pack(pop)

//
// scg_image_create_from_tga implementation
//

scg_return_status scg_image_create_from_tga(scg_image *image,
                                            const char *filepath) {
    unsigned char *file_contents = scg__read_file(filepath);
    if (file_contents == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to read %s image file", filepath);
        return scg__return_status_failure(error_msg);
    }

    scg__tga_header *header = (scg__tga_header *)file_contents;

    if (header->color_map_type != 0) {
        const char *error_msg =
            scg__sprintf("Failed to parse %s image file. The color map type "
                         "must be 0, got %u",
                         filepath, header->color_map_type);
        scg__return_status_failure(error_msg);
    }
    if (header->image_type_code != 2) {
        const char *error_msg =
            scg__sprintf("Failed to parse %s image file. The image type code "
                         "must be 2, got %u",
                         filepath, header->image_type_code);
        scg__return_status_failure(error_msg);
    }
    if (header->pixel_size != 24 && header->pixel_size != 32) {
        const char *error_msg =
            scg__sprintf("Failed to parse %s image file. The pixel size must "
                         "be 24 or 32, got %u",
                         filepath, header->pixel_size);
        scg__return_status_failure(error_msg);
    }

    // Read past the image ID field and the color map data to the pixels.
    // The color map data should always be empty for the subset of tga images we
    // support.
    unsigned char *pixels =
        file_contents + sizeof(scg__tga_header) + header->id_len;
    uint32_t *image_pixels =
        malloc(header->width * header->height * sizeof(uint32_t));
    if (image_pixels == NULL) {
        const char *error_msg =
            scg__sprintf("Failed to parse %s image file. Could not allocate "
                         "memory for pixels",
                         filepath);
        scg__return_status_failure(error_msg);
    }

    const int components = header->pixel_size / 8;

    for (int y = 0; y < header->height; y++) {
        for (int x = 0; x < header->width; x++) {
            int src_index = scg__map_row_col_to_index(x, y, header->width);

            int byte_index = src_index * components;
            scg_pixel pixel =
                scg_pixel_create(pixels[byte_index + 2], pixels[byte_index + 1],
                                 pixels[byte_index]);
            if (components == 4) {
                pixel.color.a = pixels[byte_index + 3];
            }

            int dest_index = scg__map_row_col_to_index(
                x, header->height - 1 - y, header->width);
            image_pixels[dest_index] = pixel.packed;
        }
    }

    image->width = header->width;
    image->height = header->height;
    image->pixels = image_pixels;

    free(file_contents);

    return scg__return_status_success();
}

//
// scg_image_destroy implementation
//

void scg_image_destroy(scg_image *image) {
    free(image->pixels);
}

static int scg__get_monitor_refresh_rate(SDL_DisplayMode display_mode) {
    int result = display_mode.refresh_rate;

    if (result == 0) {
        return SCG_DEFAULT_REFRESH_RATE;
    }

    return result;
}

//
// scg_screen_create implementation
//

scg_return_status scg_screen_create(scg_screen *screen, const char *title,
                                    int width, int height, int scale,
                                    bool_t fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        return scg__return_status_failure(SDL_GetError());
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
        return scg__return_status_failure(SDL_GetError());
    }

    SDL_Window *sdl_window = SDL_CreateWindow(
        title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale,
        height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL) {
        return scg__return_status_failure(SDL_GetError());
    }

    if (fullscreen == SCG_TRUE) {
        SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(
        sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) {
        return scg__return_status_failure(SDL_GetError());
    }

    SDL_RenderSetLogicalSize(sdl_renderer, width, height);

    SDL_Texture *sdl_texture =
        SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    if (sdl_texture == NULL) {
        return scg__return_status_failure(SDL_GetError());
    }

    uint32_t *pixels = (uint32_t *)calloc(width * height, sizeof(*pixels));
    if (pixels == NULL) {
        int total_bytes = width * height * sizeof(*pixels);
        const char *error_msg = scg__sprintf(
            "Failed to allocate memory for pixel buffer. w=%d, h=%d, bytes=%d",
            width, height, total_bytes);
        return scg__return_status_failure(error_msg);
    }

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
    screen->sdl_texture = sdl_texture;
    screen->pixels = pixels;
    screen->total_num_pixels = screen->width * screen->height;
    screen->pitch = screen->width * sizeof(uint32_t);
    screen->blend_mode = SCG_BLEND_MODE_NONE;
    screen->frame_metrics.update_counter = scg_get_performance_counter();
    screen->is_running = SCG_TRUE;

    return scg__return_status_success();
}

//
// scg_screen_is_running implementation
//

bool_t scg_screen_is_running(scg_screen *screen) {
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        screen->is_running = SCG_FALSE;
        break;
    }

    return screen->is_running == SCG_TRUE;
}

//
// scg_screen_set_blend_mode implementation
//

void scg_screen_set_blend_mode(scg_screen *screen, scg_blend_mode blend_mode) {
    screen->blend_mode = blend_mode;
}

//
// scg_screen_set_pixel implementation
//

void scg_screen_set_pixel(scg_screen *screen, int x, int y, scg_pixel pixel) {
    if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
        int index = scg__map_row_col_to_index(x, y, screen->width);

        if (screen->blend_mode == SCG_BLEND_MODE_MASK) {
            if (pixel.color.a == 255) {
                screen->pixels[index] = pixel.packed;
            }
        }

        if (screen->blend_mode == SCG_BLEND_MODE_ALPHA) {
            scg_pixel d = scg_pixel_create_from_uint32(screen->pixels[index]);
            float32_t a = (float32_t)(pixel.color.a / 255.0f);
            float32_t c = 1.0f - a;
            float32_t r =
                a * (float32_t)pixel.color.r + c * (float32_t)d.color.r;
            float32_t g =
                a * (float32_t)pixel.color.g + c * (float32_t)d.color.g;
            float32_t b =
                a * (float32_t)pixel.color.b + c * (float32_t)d.color.b;

            screen->pixels[index] =
                scg_pixel_create((uint8_t)r, (uint8_t)g, (uint8_t)b).packed;
        }

        if (screen->blend_mode == SCG_BLEND_MODE_NONE) {
            screen->pixels[index] = pixel.packed;
        }
    }
}

//
// scg_screen_clear implementation
//

void scg_screen_clear(scg_screen *screen, scg_pixel color) {
    uint32_t pixel = color.packed;
    int num_pixels = screen->total_num_pixels;

    for (int i = 0; i < num_pixels; ++i) {
        screen->pixels[i] = pixel;
    }
}

//
// scg_screen_draw_line implementation
//

void scg_screen_draw_line(scg_screen *screen, int px0, int py0, int px1,
                          int py1, scg_pixel color) {
    int dx = abs(px1 - px0);
    int dy = abs(py1 - py0);

    // Line is vertical
    if (dx == 0) {
        if (py1 < py0) {
            scg_swap_int(&py0, &py1);
        }

        for (int y = py0; y <= py1; y++) {
            scg_screen_set_pixel(screen, px0, y, color);
        }

        return;
    }

    // Line is horizontal
    if (dy == 0) {
        if (px1 < px0) {
            scg_swap_int(&px0, &px1);
        }

        for (int x = px0; x <= px1; x++) {
            scg_screen_set_pixel(screen, x, py0, color);
        }

        return;
    }

    int step_x = px0 < px1 ? 1 : -1;
    int step_y = py0 < py1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2 = 0;

    for (;;) {
        scg_screen_set_pixel(screen, px0, py0, color);

        if (px0 == px1 && py0 == py1) {
            break;
        }

        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            px0 += step_x;
        }
        if (e2 < dy) {
            err += dx;
            py0 += step_y;
        }
    }
}

//
// scg_screen_draw_rect_implementation
//

void scg_screen_draw_rect(scg_screen *screen, int px, int py, int width,
                          int height, scg_pixel color) {
    scg_screen_draw_line(screen, px, py, px + width, py, color);
    scg_screen_draw_line(screen, px, py, px, py + height, color);
    scg_screen_draw_line(screen, px, py + height, px + width, py + height,
                         color);
    scg_screen_draw_line(screen, px + width, py, px + width, py + height,
                         color);
}

//
// scg_screen_fill_rect_implementation
//

void scg_screen_fill_rect(scg_screen *screen, int px, int py, int width,
                          int height, scg_pixel color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            scg_screen_set_pixel(screen, px + j, py + i, color);
        }
    }
}

//
// scg_polygon_create_points implementation
//

void scg_polygon_create_points(int num_points, float32_t out[num_points][2],
                               float32_t radius) {
    float32_t theta = SCG_PI * 2.0f / (float32_t)num_points;

    for (int i = 0; i < num_points; i++) {
        out[i][0] = radius * cosf(theta * i);
        out[i][1] = radius * sinf(theta * i);
    }
}

//
// scg_screen_draw_polygon implementation
//

void scg_screen_draw_polygon(scg_screen *screen, int px, int py,
                             float32_t points[][2], int num_points,
                             scg_pixel color) {
    for (int i = 0; i < num_points; i++) {
        scg_screen_draw_line(
            screen, px + (int)(points[i][0]), py + (int)(points[i][1]),
            px + (int)(points[(i + 1) % num_points][0]),
            py + (int)(points[(i + 1) % num_points][1]), color);
    }
}

//
// scg_screen_draw_image implementation
//

void scg_screen_draw_image(scg_screen *screen, scg_image image, int px,
                           int py) {
    int image_w = image.width;
    int image_h = image.height;

    for (int i = 0; i < image_h; i++) {
        for (int j = 0; j < image_w; j++) {
            scg_pixel pixel;
            pixel.packed =
                image.pixels[scg__map_row_col_to_index(j, i, image_w)];

            scg_screen_set_pixel(screen, px + j, py + i, pixel);
        }
    }
}

//
// scg_screen_draw_image_with_transform implementation
//

void scg_screen_draw_image_with_transform(scg_screen *screen, scg_image image,
                                          int px, int py, float32_t angle,
                                          float32_t sx, float32_t sy) {
    if (sx <= 0.0f)
        sx = 1.0f;
    if (sy <= 0.0f)
        sy = 1.0f;

    float32_t image_w = image.width;
    float32_t image_h = image.height;
    float32_t image_sw = image_w * sx;
    float32_t image_sh = image_h * sy;
    float32_t ratio_x = image_w / image_sw;
    float32_t ratio_y = image_h / image_sh;

    float32_t origin_x = image_w * 0.5f;
    float32_t origin_y = image_h * 0.5f;

    float32_t s = sinf(-angle);
    float32_t c = cosf(-angle);

    for (int i = 0; i < image_sh; i++) {
        for (int j = 0; j < image_sw; j++) {
            float32_t image_x = (float32_t)j * ratio_x - origin_x;
            float32_t image_y = (float32_t)i * ratio_y - origin_y;
            float32_t image_x_rot = (image_x * c - image_y * s) + origin_x;
            float32_t image_y_rot = (image_x * s + image_y * c) + origin_y;

            if (image_x_rot >= 0 && image_x_rot < image_w && image_y_rot >= 0 &&
                image_y_rot < image_h) {
                scg_pixel pixel;
                pixel.packed = image.pixels[scg__map_row_col_to_index(
                    (int)image_x_rot, (int)image_y_rot, image_w)];

                scg_screen_set_pixel(screen, px + j, py + i, pixel);
            }
        }
    }
}

static void scg__draw_char(scg_screen *screen, const char *char_bitmap, int x,
                           int y, scg_pixel color) {
    for (int i = 0; i < SCG_FONT_SIZE; i++) {
        for (int j = 0; j < SCG_FONT_SIZE; j++) {
            int set = char_bitmap[i] & 1 << j;

            if (set) {
                scg_screen_set_pixel(screen, x + j, y + i, color);
            }
        }
    }
}

static int scg__string_width(const char *str, int size) {
    return strlen(str) * size;
}

//
// scg_screen_draw_string implementation
//

void scg_screen_draw_string(scg_screen *screen, const char *str, int x, int y,
                            int anchor_to_center, scg_pixel color) {
    int current_x = x;
    int current_y = y;

    if (anchor_to_center) {
        int width = scg__string_width(str, SCG_FONT_SIZE);
        current_x = x - width / 2;
        current_y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        int char_code = (int)str[i];
        if (char_code < 0 || char_code > 127) {
            char_code = 63; // draw '?' for unknown char
        }
        scg__draw_char(screen, scg__font8x8[char_code], current_x, current_y,
                       color);

        current_x += SCG_FONT_SIZE + 1;
    }
}

//
// scg_screen_draw_fps implementation
//

void scg_screen_draw_fps(scg_screen *screen) {
    float32_t fps = screen->frame_metrics.fps;
    ssize_t bsize = snprintf(NULL, 0, "fps:%.2f", fps);
    char buffer[bsize];
    snprintf(buffer, bsize + 1, "fps:%.2f", fps);

    scg_pixel color = SCG_COLOR_GREEN;
    float32_t target_fps = (float32_t)screen->target_frames_per_sec;
    if (fps < target_fps * 0.95) {
        color = SCG_COLOR_YELLOW;
    }
    if (fps < target_fps * 0.5) {
        color = SCG_COLOR_RED;
    }

    scg_screen_draw_string(screen, buffer, 10, 10, 0, color);
}

//
// scg_screen_present implementation
//

void scg_screen_present(scg_screen *screen) {
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

    SDL_UpdateTexture(screen->sdl_texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(screen->sdl_renderer);
    SDL_RenderCopy(screen->sdl_renderer, screen->sdl_texture, NULL, NULL);
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

void scg_screen_log_info(scg_screen *screen) {
    scg_log_info("screen has w:%d, h:%d, scale:%d, target fps:%d",
                 screen->width, screen->height, screen->scale,
                 screen->target_frames_per_sec);
}

//
// scg_screen_close implementation
//

void scg_screen_close(scg_screen *screen) {
    screen->is_running = SCG_FALSE;
}

//
// scg_screen_destroy implementation
//

void scg_screen_destroy(scg_screen *screen) {
    free(screen->pixels);
    SDL_DestroyTexture(screen->sdl_texture);
    SDL_DestroyRenderer(screen->sdl_renderer);
    SDL_DestroyWindow(screen->sdl_window);
    SDL_Quit();
}

//
// scg_sound_device_create implementation
//

scg_return_status scg_sound_device_create(scg_sound_device *sound_device,
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
        return scg__return_status_failure(SDL_GetError());
    }

    if (have.format != want.format) {
        return scg__return_status_failure(
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
        return scg__return_status_failure(error_msg);
    }

    sound_device->num_sounds = 0;

    SDL_PauseAudioDevice(device_id, 0);

    return scg__return_status_success();
}

//
// scg_sound_device_log_info implementation
//

void scg_sound_device_log_info(scg_sound_device *sound_device) {
    scg_log_info("sound device has id:%d, channels:%d, samples/sec:%d, "
                 "samples/frame:%d, bytes/sample:%d",
                 sound_device->device_id, sound_device->channels,
                 sound_device->freq, sound_device->latency_sample_count,
                 sound_device->bytes_per_sample);
}

//
// scg_sound_create implementation
//

scg_return_status scg_sound_create_from_wav(scg_sound_device *sound_device,
                                            scg_sound *sound,
                                            const char *filepath, bool_t loop) {
    if (sound_device->num_sounds == SCG_MAX_SOUNDS) {
        return scg__return_status_failure("Maximum sounds reached");
    }

    SDL_AudioSpec spec;
    uint32_t length;
    uint8_t *buffer;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == NULL) {
        return scg__return_status_failure(SDL_GetError());
    }

    sound->sdl_spec = spec;
    sound->length = length;
    sound->buffer = buffer;
    sound->play_offset = 0;
    sound->end_position = buffer + length;
    sound->is_playing = SCG_FALSE;
    sound->loop = loop;

    sound_device->sounds[sound_device->num_sounds++] = sound;

    return scg__return_status_success();
}

//
// scg_sound_play implementation
//

void scg_sound_play(scg_sound *sound) {
    if (sound->is_playing == SCG_FALSE) {
        sound->is_playing = SCG_TRUE;
    }
}

//
// scg_sound_device_update implementation
//

void scg_sound_device_update(scg_sound_device *sound_device) {
    memset(sound_device->buffer, 0, sound_device->buffer_size);

    uint32_t target_queue_bytes = sound_device->buffer_size;
    uint32_t bytes_to_write =
        target_queue_bytes - SDL_GetQueuedAudioSize(sound_device->device_id);

    for (int i = 0; i < sound_device->num_sounds; i++) {
        scg_sound *sound = sound_device->sounds[i];

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

void scg_sound_device_destroy(scg_sound_device *sound_device) {
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

void scg_keyboard_create(scg_keyboard *keyboard) {
    keyboard->current_key_states = SDL_GetKeyboardState(NULL);
    memset(keyboard->previous_key_states, 0,
           sizeof(uint8_t) * SDL_NUM_SCANCODES);
}

//
// scg_keyboard_is_key_down implementation
//

int scg_keyboard_is_key_down(scg_keyboard *keyboard, scg_key_code key) {
    return keyboard->current_key_states[key] == 1;
}

//
// scg_keyboard_is_key_up implementation
//

int scg_keyboard_is_key_up(scg_keyboard *keyboard, scg_key_code key) {
    return keyboard->current_key_states[key] == 0;
}

//
// scg_keyboard_is_key_triggered implementation
//

int scg_keyboard_is_key_triggered(scg_keyboard *keyboard, scg_key_code key) {
    return keyboard->previous_key_states[key] == 0 &&
           keyboard->current_key_states[key] == 1;
}

//
// scg_keyboard_update implementation
//

void scg_keyboard_update(scg_keyboard *keyboard) {
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
