#define SCG_IMPLEMENTATION
#include "../scg.h"

// Rotozoom effect implemented with help from the following references:
// - https://seancode.com/demofx/

static void draw_image_with_rotozoom(scg_image_t *back_buffer, scg_image_t *src,
                                     float32_t angle, float32_t scale) {
    int w = back_buffer->width;
    int h = back_buffer->height;
    const int src_w = src->width;
    const int src_h = src->height;

    float32_t s = sinf(angle);
    float32_t c = cosf(angle);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float32_t tx = (x * c - y * s) * scale;
            float32_t ty = (x * s + y * c) * scale;

            // no need for mod because image width is 256px
            int src_x = (int)tx & 255;

            int src_y = (int)ty % src_h;
            while (src_y < 0) {
                src_y += src_h;
            }

            int src_i = scg_pixel_index_from_xy(src_x, src_y, src_w);
            scg_pixel_t color = scg_pixel_new_uint32(src->pixels[src_i]);

            int dest_i = scg_pixel_index_from_xy(x, y, w);
            back_buffer->pixels[dest_i] = color.packed;
        }
    }
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
    err = scg_screen_new(&screen, "SCG Example: Rotozoom", &back_buffer,
                         window_scale, fullscreen);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_image_t image;
    err = scg_image_new_from_bmp(&image, "assets/2ndreal.bmp");
    if (!err.none) {
        scg_log_error("Failed to create image. Error: %s", err.message);
        return -1;
    }

    float32_t elapsed_time = 0.0f;
    scg_pixel_t clear_color = SCG_COLOR_95_GREEN;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_frame_time_secs;

        scg_image_clear(&back_buffer, clear_color);

        float32_t scale = 0.5f + sinf(elapsed_time * 0.5f) * 2.0f;
        float32_t angle = elapsed_time;
        draw_image_with_rotozoom(&back_buffer, &image, angle, scale);

        scg_image_draw_frame_metrics(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
