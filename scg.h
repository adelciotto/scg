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
// before you include this file in *one* C++ file to create the
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

//////////////////////////////////////////////////////////////////////////////
//
//   INTERFACE
//

#ifndef INCLUDE_SCG_H
#define INCLUDE_SCG_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace scg {
const float PI = 3.1415926535f;
const float PI_2 = PI * 2.0f;

class Exception : virtual public std::runtime_error {
  public:
    Exception(const std::string &msg) : std::runtime_error(msg) {
    }
};

template <typename... Args>
void log_errorf(const std::string &fmt, Args... args) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, args...);
}
template <typename... Args>
void log_warnf(const std::string &fmt, Args... args) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, fmt, args...);
}
template <typename... Args>
void log_infof(const std::string &fmt, Args... args) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, args...);
}

struct Pixel {
    union {
        uint32_t packed;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        struct {
            uint8_t a, r, g, b;
        };
#else
        struct {
            uint8_t b, g, r, a;
        };
#endif
    }

    Pixel();
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Pixel(uint32_t packed);

    enum Mode { NORMAL, MASK, ALPHA };
};

const Pixel WHITE(255, 255, 255), GRAY(128, 128, 128), BLACK(0, 0, 0),
    RED(255, 0, 0), GREEN(0, 255, 0), BLUE(0, 0, 255), YELLOW(255, 255, 0),
    MAGENTA(255, 0, 255), BROWN(150, 75, 0), GREEN_95(0, 128, 128),
    ICE_BLUE(153, 255, 255), SKY_BLUE(135, 206, 235);

class Image {
  public:
    Image(int width, int height);
    Image(const std::string &file_path);
    ~Image();

  public:
    int width() const;
    int height() const;
    int pitch() const;
    uint32_t *pixels() const;

  private:
    int m_width;
    int m_height;
    int m_pitch;
    uint32_t *m_pixels;
    Pixel::Mode blend_mode;
};

struct FrameMetrics {
    int target_fps;
    int fps;
    float frame_time_secs;
    float frame_time_millisecs;
};

class Keyboard {
    enum Key {
        UP = SDL_SCANCODE_UP,
        DOWN = SDL_SCANCODE_DOWN,
        LEFT = SDL_SCANCODE_LEFT,
        RIGHT = SDL_SCANCODE_RIGHT,
        X = SDL_SCANCODE_X,
        C = SDL_SCANCODE_C,
        Z = SDL_SCANCODE_Z,
        P = SDL_SCANCODE_P,
        SPACE = SDL_SCANCODE_SPACE,
        ESCAPE = SDL_SCANCODE_ESCAPE
    };

  public:
    bool key_triggered(Key key) const;
    bool key_down(Key key) const;
    bool key_up(Key key) const;
    void update_key_states();

  private:
    bool m_current_key_states[SDL_NUM_SCANCODES];
    bool m_last_frame_key_states[SDL_NUM_SCANCODES];
};

class Mouse {
    enum Button {
        LEFT = SDL_BUTTON_LEFT,
        MIDDLE = SDL_BUTTON_MIDDLE,
        RIGHT = SDL_BUTTON_RIGHT
    };

  public:
    bool button_down(Button button) const;
    bool button_up(Button button) const;

  public:
    void set_position(int x, int y);
    void set_window_position(int x, int y);
    void set_button_state(uint32_t button_state);

  private:
    int m_x, m_y;
    int m_window_x, m_window_y;
    uint32_t m_button_state;
};

class Application {
    struct Config {
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

        Config();
    };

  public:
    Application(Config config);
    ~Application();

  public:
    virtual void create() = 0;
    virtual void update(float delta_time) = 0;
    virtual void draw() = 0;

  public:
    void start();

  protected:
    int m_width;
    int m_height;
    Keyboard m_keyboard;
    Mouse m_mouse;
    Image m_draw_target;

  private:
    Config m_config;
    int m_target_fps int m_target_frame_time_secs;
    float m_delta_time;
    float m_elapsed_time;
    bool m_running;

    uint64_t m_last_frame_counter;
    float64_t m_frame_metrics_update_counter;
    FrameMetrics m_frame_metrics;
    bool m_vsync;
    bool m_lock_fps;

    SDL_Window *m_sdl_window;
    SDL_Renderer *m_sdl_renderer;
    SDL_Texture *m_sdl_texture;
};
} // namespace scg

#endif // INCLUDE_SCG_H

//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#ifdef SCG_IMPLEMENTATION

#include <cstring>

namespace scg {
//
// Pixel implementation
//

Pixel::Pixel() : r(0), g(0), b(0), a(255) {
}

Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    : r(r), g(g), b(b), a(a) {
}

Pixel::Pixel(uint32_t packed) : packed(packed) {
}

//
// Image implementation
//

Image::Image(int width, int height)
    : m_width(width), m_height(height), m_pitch(width * 4),
      m_pixels(new uint32_t[width * height]),
      m_blend_mode(Pixel::Mode::NORMAL) {
    std::memset(m_pixels, 0, m_height * m_pitch);
}

Image::Image(const std::string &file_path) {
    SDL_Surface *surface = SDL_LoadBMP(file_path.c_str());
    if (surface == NULL) {
        throw(Exception("Failed to load image file at " +
                        std::string(filepath) + ", " +
                        std::string(SDL_GetError())));
    }

    // Convert the surface to RGBA32 pixel format.
    SDL_PixelFormat *pixel_format = SDL_AllocFormat(SCG__IMAGE_PIXEL_FORMAT);
    if (pixel_format == NULL) {
        SDL_FreeSurface(surface);

        throw(Exception("Failed to allocate pixel format for image file at " +
                        std::string(filepath) + ", " +
                        std::string(SDL_GetError())));
    }

    SDL_Surface *converted_surface =
        SDL_ConvertSurface(surface, pixel_format, 0);
    if (converted_surface == NULL) {
        SDL_FreeFormat(pixel_format);
        SDL_FreeSurface(surface);

        throw(Exception("Failed to convert surface for image file at " +
                        std::string(filepath) + ", " +
                        std::string(SDL_GetError())));
    }

    // We no longer need the original surface.
    SDL_FreeFormat(pixel_format);
    SDL_FreeSurface(surface);

    int surface_w = converted_surface->w;
    int surface_h = converted_surface->h;
    int surface_pitch = converted_surface->pitch;
    uint32_t *surface_pixels = (uint32_t *)converted_surface->pixels;

    // Allocate new pixel buffer and copy over the converted surface pixel data.
    m_pixels = new uint32_t[surface_w * surface_h];
    std::memcpy(m_pixels, surface_pixels, surface_pitch * surface_h);

    m_width = surface_w;
    m_height = surface_h;
    m_pitch = surface_pitch;
    m_blend_mode = Pixel::Mode::Normal;

    // We no longer need the converted surface.
    SDL_FreeSurface(converted_surface);
}

Image::~Image() {
    delete[] m_pixels;
}

int Image::width() const {
    return m_width;
}

int Image::height() const {
    return m_height;
}

int Image::pitch() const {
    return m_pitch;
}

uint32_t *Image::pixels() const {
    return m_pixels;
}

//
// Application implementation
//

Application::Application(Application::Config config) : m_config(config) {

}

Application::~Application() {

}

} // namespace scg

#endif // SCG_IMPLEMENTATION
