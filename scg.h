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
    int space_pressed;

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
    screen->space_pressed = 0;
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
        if (event.key.keysym.sym == SDLK_SPACE) {
            screen->space_pressed = 1;
        }
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_SPACE) {
            screen->space_pressed = 0;
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
        (uint8 *)calloc(sound_device->latency_sample_count,
                        bytes_per_sample);
	if (sound_device->buffer == NULL) {
		return (scg_return_status){1, SCG_RETURN_STATUS_SOUND_BUFFER_ALLOCATION_FAILURE};
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

#endif
