// Music in this demo is 'Arcade Music Loop.wav' by joshuaempyre.
// Music link: https://freesound.org/people/joshuaempyre/sounds/251461/

#define SCG_IMPLEMENTATION
#include "../scg.h"

int main(void) {
    const int width = 256;
    const int height = 256;
    const int scale = 2;
    const int fullscreen = 0;

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
    return_status = scg_image_create_from_bmp(&image, "assets/rgb32.bmp");
    if (return_status.is_error) {
        scg_log_error("failed to create image, %s", return_status.error_msg);
        return -1;
    }

    scg_color clear_color = SCG_COLOR_BLACK;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }
        scg_keyboard_update(&keyboard);

        scg_screen_clear(&screen, clear_color);
        scg_screen_draw_image(&screen, screen.width / 2 - image.width / 2,
                              screen.height / 2 - image.height / 2, &image);
        scg_screen_draw_fps(&screen);

        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    return 0;
}
