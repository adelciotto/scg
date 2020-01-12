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
// custom config files
// implement plasma demo
// custom color type
// draw text
// basic drawing functions
// keyboard input
// mouse input
// simple audio - maybe rethink api?
//	- pause sound
//	- pause device
// gamepad input

#ifndef INCLUDE_SCG_H
#define INCLUDE_SCG_H

#include <SDL2/SDL.h>

#define uint8 Uint8
#define uint16 Uint16
#define uint32 Uint32
#define uint64 Uint64
#define sint8 Sint8
#define sint16 Sint16
#define sint32 Sint32
#define sint64 Sint64
#define float32 float
#define float64 double

#define scg_log_error(...)                                                     \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_warn(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define scg_log_info(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

typedef struct scg_return_status scg_return_status;
extern const char *scg_return_status_get_error(scg_return_status return_status);

extern int scg_min(int val, int min);

extern uint64 scg_get_performance_counter(void);
extern uint64 scg_get_performance_frequency(void);
extern float64 scg_get_elapsed_time_secs(uint64 end, uint64 start);
extern float64 scg_get_elapsed_time_millisecs(uint64 end, uint64 start);

typedef struct scg_screen scg_screen;
scg_return_status scg_screen_create(scg_screen *screen, const char *title,
                                    int width, int height, int scale,
                                    int fullscreen);
extern int scg_screen_is_running(scg_screen *screen);
extern void scg_screen_lock(scg_screen *screen);
extern void scg_screen_unlock(scg_screen *screen);
extern void scg_screen_set_pixel(scg_screen *screen, int x, int y, uint8 r,
                                 uint8 g,
                                 uint8 b); // TODO: create custom color type
extern void scg_screen_clear(scg_screen *screen, uint8 r, uint8 g,
                             uint8 b); // TODO: create custom color type
extern void scg_screen_draw_string(scg_screen *screen, const char *str, int x,
                                   int y, int center, uint8 r, uint8 g,
                                   uint8 b);
extern void scg_screen_present(scg_screen *screen);
extern void scg_screen_log_info(scg_screen *screen);
extern void scg_screen_destroy(scg_screen *screen);

typedef struct scg_sound_device scg_sound_device;
typedef struct scg_sound scg_sound;
extern scg_return_status scg_sound_device_create(scg_sound_device *sound_device,
                                                 int frames_per_sec);
extern void scg_sound_device_log_info(scg_sound_device *sound_device);
extern scg_return_status
scg_sound_create_from_wav(scg_sound_device *sound_device, scg_sound *sound,
                          const char *filepath, int loop);
extern void scg_sound_play(scg_sound *sound);
extern void scg_sound_device_update(scg_sound_device *sound_device);
extern void scg_sound_device_destroy(scg_sound_device *sound_device);

///////////////
//
// Everything below here is implementation details
//

#define SCG_MAX_SOUNDS 16 // TODO: use linked list for sounds?

enum scg_return_status_code {
    SCG_RETURN_STATUS_SUCCESS,
    SCG_RETURN_STATUS_SDL_ERROR,
    SCG_RETURN_STATUS_SDL_AUDIO_FORMAT_NOT_SUPPORTED,
    SCG_RETURN_STATUS_SOUND_BUFFER_ALLOCATION_FAILURE,
    SCG_RETURN_STATUS_MAX_SOUNDS_REACHED
    // ...
};

struct scg_return_status {
    int is_error;
    enum scg_return_status_code code;
    // TODO: maybe store some generic error related properties?
};

extern const char scg__font8x8[128][8];

struct scg_screen {
    const char *title;
    int width;
    int height;
    int scale;

    int target_frames_per_sec;
    float64 target_time_per_frame_secs;
    uint64 last_frame_counter;
    struct frame_metrics {
        float64 time_per_frame_secs;
        float64 time_per_frame_millisecs;
        float64 fps;
    } frame_metrics;

    int is_running;
    int is_locked;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
    SDL_Surface *sdl_surface;
};

struct scg_sound {
    int device_id;
    SDL_AudioSpec sdl_spec;
    uint32 length;
    uint8 *buffer;
    int play_offset;
    uint8 *end_position;
    int is_playing;
    int loop;
};

struct scg_sound_device {
    SDL_AudioDeviceID device_id;
    int freq;
    uint8 channels;
    uint16 samples;
    int bytes_per_sample;
    int latency_sample_count;
    uint32 buffer_size;
    uint8 *buffer;

    scg_sound *sounds[SCG_MAX_SOUNDS];
    int num_sounds;
};

#endif

//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#ifdef SCG_IMPLEMENTATION

#define SCG_DEFAULT_REFRESH_RATE 60
#define SCG_FONT_SIZE 8

//
// scg_return_status_get_error implementation
//

const char *scg_return_status_get_error(scg_return_status return_status) {
    switch (return_status.code) {
    case SCG_RETURN_STATUS_SDL_ERROR:
        return SDL_GetError();
    case SCG_RETURN_STATUS_SDL_AUDIO_FORMAT_NOT_SUPPORTED:
        return "AUDIO_F32 format not supported";
    case SCG_RETURN_STATUS_SOUND_BUFFER_ALLOCATION_FAILURE:
        return "failed to allocate memory for sound buffer";
    case SCG_RETURN_STATUS_MAX_SOUNDS_REACHED:
        return "the maximum of 16 sounds has been reached";
    case SCG_RETURN_STATUS_SUCCESS:
    default:
        return "";
    }
}

//
// scg_min
//

int scg_min(int val, int min) {
    return val < min ? val : min;
}

//
// scg_get_performance_counter
//

Uint64 scg_get_performance_counter(void) {
    return SDL_GetPerformanceCounter();
}

//
// scg_get_performance_frequency
//

Uint64 scg_get_performance_frequency(void) {
    return SDL_GetPerformanceFrequency();
}

//
// scg_get_elapsed_time_secs
//

float64 scg_get_elapsed_time_secs(uint64 end, uint64 start) {
    return (float64)(end - start) / scg_get_performance_frequency();
}

//
// scg_get_elapsed_time_millisecs
//

float64 scg_get_elapsed_time_millisecs(uint64 end, uint64 start) {
    return (float64)((end - start) * 1000) / scg_get_performance_frequency();
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
                                    int fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    SDL_Window *sdl_window = SDL_CreateWindow(
        title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale,
        height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    if (fullscreen) {
        SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(
        sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    SDL_RenderSetLogicalSize(sdl_renderer, width, height);

    SDL_Texture *sdl_texture =
        SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    if (sdl_texture == NULL) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    SDL_Surface *sdl_surface = SDL_CreateRGBSurfaceWithFormat(
        0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (sdl_surface == NULL) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    screen->title = title;
    screen->width = width;
    screen->height = height;
    screen->scale = scale;
    screen->target_frames_per_sec = scg__get_monitor_refresh_rate(display_mode);
    screen->target_time_per_frame_secs =
        1.0 / (float64)screen->target_frames_per_sec;
    screen->last_frame_counter = scg_get_performance_counter();
    screen->is_locked = 0;
    screen->sdl_window = sdl_window;
    screen->sdl_renderer = sdl_renderer;
    screen->sdl_texture = sdl_texture;
    screen->sdl_surface = sdl_surface;
    screen->is_running = 1;

    return (scg_return_status){0, SCG_RETURN_STATUS_SUCCESS};
}

//
// scg_screen_is_running implementation
//

int scg_screen_is_running(scg_screen *screen) {
    SDL_Event event;

    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        screen->is_running = 0;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            screen->is_running = 0;
        }
        break;
    }

    return screen->is_running;
}

//
// scg_screen_lock implementation
//

void scg_screen_lock(scg_screen *screen) {
    if (SDL_MUSTLOCK(screen->sdl_surface)) {
        SDL_LockSurface(screen->sdl_surface);
        screen->is_locked = 1;
    }
}

//
// scg_screen_unlock implementation
//

void scg_screen_unlock(scg_screen *screen) {
    if (screen->is_locked) {
        SDL_UnlockSurface(screen->sdl_surface);
        screen->is_locked = 0;
    }
}

static int scg__pixelxy_to_index(int x, int y, int pitch) {
    return (y * pitch / 4) + x;
}

//
// scg_screen_set_pixel implementation
// TODO: make custom color type

void scg_screen_set_pixel(scg_screen *screen, int x, int y, uint8 r, uint8 g,
                          uint8 b) {
    if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
        uint32 color = (r << 24) + (g << 16) + (b << 8) + 255;
        uint32 *pixels = (uint32 *)screen->sdl_surface->pixels;

        int index = scg__pixelxy_to_index(x, y, screen->sdl_surface->pitch);
        pixels[index] = color;
    }
}

//
// scg_screen_clear implementation
// TODO: make custom color type

void scg_screen_clear(scg_screen *screen, uint8 r, uint8 g, uint8 b) {
    scg_screen_lock(screen);

    for (int y = 0; y < screen->height; y++) {
        for (int x = 0; x < screen->width; x++) {
            scg_screen_set_pixel(screen, x, y, r, g, b);
        }
    }

    scg_screen_unlock(screen);
}

static void scg_draw_char(scg_screen *screen, const char *char_bitmap,
                          int screen_x, int screen_y, uint8 r, uint8 g,
                          uint8 b) {
    for (int y = 0; y < SCG_FONT_SIZE; y++) {
        for (int x = 0; x < SCG_FONT_SIZE; x++) {
            int set = char_bitmap[y] & 1 << x;

            if (set) {
                scg_screen_set_pixel(screen, screen_x + x, screen_y + y, r, g,
                                     b);
            }
        }
    }
}

//
// scg_screen_draw_string
//

void scg_screen_draw_string(scg_screen *screen, const char *str, int x, int y,
                            int center, uint8 r, uint8 g, uint8 b) {
    int current_x = x;

    if (center) {
        int width = strlen(str) * SCG_FONT_SIZE;
        current_x = x - width / 2;
        y -= SCG_FONT_SIZE / 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        int char_code = (int)str[i];
        if (char_code < 0 || char_code > 127) {
            char_code = 63; // draw '?' for unknown char
        }
        scg_draw_char(screen, scg__font8x8[char_code], current_x, y, r, g, b);
        current_x += SCG_FONT_SIZE;
    }
}

//
// scg_screen_present implementation
//

void scg_screen_present(scg_screen *screen) {
    // Wait until we have reached the target amount of time per frame.
    // We wait by just spinning in a while loop, because we should only
    // be waiting a small amount of milliseconds per frame to sync with
    // the monitors refresh rate (e.g 60hz, ~16ms).
    while (scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                     screen->last_frame_counter) <
           screen->target_time_per_frame_secs) {
    }

    Uint64 end_frame_counter = scg_get_performance_counter();

    SDL_UpdateTexture(screen->sdl_texture, NULL, screen->sdl_surface->pixels,
                      screen->sdl_surface->pitch);
    SDL_RenderClear(screen->sdl_renderer);
    SDL_RenderCopy(screen->sdl_renderer, screen->sdl_texture, NULL, NULL);
    SDL_RenderPresent(screen->sdl_renderer);

    screen->frame_metrics.time_per_frame_secs = scg_get_elapsed_time_secs(
        end_frame_counter, screen->last_frame_counter);
    screen->frame_metrics.time_per_frame_millisecs =
        scg_get_elapsed_time_millisecs(end_frame_counter,
                                       screen->last_frame_counter);
    screen->frame_metrics.fps =
        (float64)scg_get_performance_frequency() /
        (float64)(end_frame_counter - screen->last_frame_counter);

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
// scg_screen_destroy implementation
//

void scg_screen_destroy(scg_screen *screen) {
    SDL_FreeSurface(screen->sdl_surface);
    SDL_DestroyTexture(screen->sdl_texture);
    SDL_DestroyRenderer(screen->sdl_renderer);
    SDL_DestroyWindow(screen->sdl_window);
    SDL_Quit();
}

//
// scg_sound_device_create
//

scg_return_status scg_sound_device_create(scg_sound_device *sound_device,
                                          int frames_per_sec) {
    SDL_AudioSpec want, have;

    int channels = 2;
    size_t bytes_per_sample = sizeof(sint16) * channels;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = 48000;
    want.format = AUDIO_S16LSB;
    want.channels = channels;
    want.samples = want.freq * bytes_per_sample / frames_per_sec;
    want.callback = NULL;

    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(
        NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (device_id == 0) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    if (have.format != want.format) {
        return (scg_return_status){
            1, SCG_RETURN_STATUS_SDL_AUDIO_FORMAT_NOT_SUPPORTED};
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
        (uint8 *)calloc(sound_device->latency_sample_count, bytes_per_sample);
    if (sound_device->buffer == NULL) {
        return (scg_return_status){
            1, SCG_RETURN_STATUS_SOUND_BUFFER_ALLOCATION_FAILURE};
    }

    sound_device->num_sounds = 0;

    SDL_PauseAudioDevice(device_id, 0);

    return (scg_return_status){0, SCG_RETURN_STATUS_SUCCESS};
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
// scg_sound_create
//

scg_return_status scg_sound_create_from_wav(scg_sound_device *sound_device,
                                            scg_sound *sound,
                                            const char *filepath, int loop) {
    if (sound_device->num_sounds == SCG_MAX_SOUNDS) {
        return (scg_return_status){1, SCG_RETURN_STATUS_MAX_SOUNDS_REACHED};
    }

    SDL_AudioSpec spec;
    uint32 length;
    uint8 *buffer;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == NULL) {
        return (scg_return_status){1, SCG_RETURN_STATUS_SDL_ERROR};
    }

    sound->sdl_spec = spec;
    sound->length = length;
    sound->buffer = buffer;
    sound->play_offset = 0;
    sound->end_position = buffer + length;
    sound->is_playing = 0;
    sound->loop = loop;

    sound_device->sounds[sound_device->num_sounds++] = sound;

    return (scg_return_status){0, SCG_RETURN_STATUS_SUCCESS};
}

//
// scg_sound_play
//

void scg_sound_play(scg_sound *sound) {
    if (!sound->is_playing) {
        sound->is_playing = 1;
    }
}

//
// scg_sound_device_update
//

void scg_sound_device_update(scg_sound_device *sound_device) {
    SDL_memset(sound_device->buffer, 0, sound_device->buffer_size);

    uint32 target_queue_bytes = sound_device->buffer_size;
    uint32 bytes_to_write =
        target_queue_bytes - SDL_GetQueuedAudioSize(sound_device->device_id);

    for (int i = 0; i < sound_device->num_sounds; i++) {
        scg_sound *sound = sound_device->sounds[i];

        if (sound->is_playing) {
            if (sound->buffer + sound->play_offset >= sound->end_position) {
                if (!sound->loop) {
                    sound->is_playing = 0;
                }
                sound->play_offset = 0;
            } else {
                uint32 temp_len =
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
// scg_sound_device_destroy_sounds
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

#endif
