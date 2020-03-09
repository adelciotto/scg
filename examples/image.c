#define SCG_IMPLEMENTATION
#include "../scg.h"

int main(void) {
    const int width = 640;
    const int height = 360;
    const int scale = 1;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen_t screen;
    scg_return_status_t return_status =
        scg_screen_create(&screen, "image", width, height, scale, fullscreen);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_create(&keyboard);

    scg_image_t image;
    return_status =
        scg_image_create_from_bmp(&image, &screen, "assets/ball.bmp");
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    scg_color_t clear_color = SCG_COLOR_95_GREEN;
    scg_color_t text_color = SCG_COLOR_WHITE;

    int center_image_x = screen.width / 2 - image.width / 2;
    int center_image_y = screen.height / 2 - image.height / 2;
    int image_blend_none_x = center_image_x - image.width - 32;
    int image_blend_alpha_x = center_image_x;
    int image_blend_mask_x = center_image_x + image.width + 32;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        scg_screen_set_draw_color(&screen, clear_color);
        scg_screen_clear(&screen);

        scg_screen_set_draw_color(&screen, text_color);
        scg_screen_draw_string(&screen, "NONE",
                               image_blend_none_x + image.width / 2,
                               center_image_y + image.height + 16, SCG_TRUE);
        scg_screen_draw_string(&screen, "BLEND",
                               image_blend_alpha_x + image.width / 2,
                               center_image_y + image.height + 16, SCG_TRUE);
        scg_screen_draw_string(&screen, "MOD",
                               image_blend_mask_x + image.width / 2,
                               center_image_y + image.height + 16, SCG_TRUE);

        scg_image_set_blend_mode(&image, SCG_BLEND_MODE_NONE);
        scg_screen_draw_image_xy(&screen, &image, image_blend_none_x,
                              center_image_y);

        scg_image_set_blend_mode(&image, SCG_BLEND_MODE_BLEND);
        scg_screen_draw_image_xy(&screen, &image, image_blend_alpha_x,
                              center_image_y);

        scg_image_set_blend_mode(&image, SCG_BLEND_MODE_MOD);
        scg_screen_draw_image_xy(&screen, &image, image_blend_mask_x,
                              center_image_y);

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    return 0;
}
