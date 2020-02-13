#define SCG_IMPLEMENTATION
#include "../scg.h"

int main(void) {
    const int width = 512;
    const int height = 480;
    const int scale = 1;
    const int fullscreen = 1;

    scg_screen screen;
    scg_return_status return_status =
        scg_screen_create(&screen, "image", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("failed to create screen, %s", return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    scg_image image;
    return_status = scg_image_create_from_tga(&image, "assets/alpha.tga");
    if (return_status.is_error) {
        scg_log_error("failed to create image, %s", return_status.error_msg);
        return -1;
    }

    scg_color clear_color = SCG_COLOR_95_GREEN;

    float32_t image_angle = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        image_angle += 0.01f;

        scg_screen_set_blend_mode(&screen, SCG_BLEND_MODE_NONE);
        scg_screen_clear(&screen, clear_color);
        scg_screen_draw_fps(&screen);

        scg_screen_set_blend_mode(&screen, SCG_BLEND_MODE_ALPHA);
        scg_screen_draw_image_with_transform(&screen, screen.width * 0.5f,
                                             screen.height * 0.5f, image_angle,
                                             2.0f, 2.0f, 0.5f, 0.5f, &image);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    return 0;
}
