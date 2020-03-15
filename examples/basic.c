#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw_frame(scg_image_t *back_buffer) {
    int w = back_buffer->width;
    int h = back_buffer->height;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint8_t r = scg_min_int(x, 255);
            uint8_t g = scg_min_int(y, 255);

            scg_pixel_t color = scg_pixel_new_rgb(r, g, 128);
            scg_image_set_pixel(back_buffer, x, y, color);
        }
    }

    scg_pixel_t color = SCG_COLOR_WHITE;
    scg_image_draw_string(back_buffer, "Hello, World!", w / 2, h / 2, 1, color);
}

int main(void) {
    const int width = 640;
    const int height = 480;
    const int window_scale = 1;
    const bool fullscreen = false;

    scg_error_t err = scg_init();
    if (!err.none) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, width, height);
    if (!err.none) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Basic", &back_buffer,
                         window_scale, fullscreen);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_pixel_t clear_color = SCG_COLOR_WHITE;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        scg_image_clear(&back_buffer, clear_color);

        draw_frame(&back_buffer);

        scg_image_draw_frame_metrics(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
