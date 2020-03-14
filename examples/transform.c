#define SCG_IMPLEMENTATION
#include "../scg.h"

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
    err = scg_screen_new(&screen, "SCG Example: Transform", &back_buffer,
                         window_scale, fullscreen);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_image_t image;
    err = scg_image_new_from_bmp(&image, "assets/ball.bmp");
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

        elapsed_time += 1.2f * screen.target_time_per_frame_secs;

        scg_image_clear(&back_buffer, clear_color);

        scg_image_set_blend_mode(&back_buffer, SCG_BLEND_MODE_ALPHA);
        scg_image_draw_image_transform(
            &back_buffer, &image, screen.width / 2 - image.width,
            screen.height / 2 - image.height, elapsed_time, 2.0f, 2.0f);

        scg_image_draw_fps(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
