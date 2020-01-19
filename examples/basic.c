// Music in this demo is 'Arcade Music Loop.wav' by joshuaempyre.
// Music link: https://freesound.org/people/joshuaempyre/sounds/251461/

#define SCG_IMPLEMENTATION
#include "../scg.h"

static void update_and_draw(scg_screen *screen) {
    for (int y = 0; y < screen->height; y++) {
        for (int x = 0; x < screen->width; x++) {
            scg_pixel pixel = scg_pixel_create((uint8_t)scg_min(x, 255),
                                               (uint8_t)scg_min(y, 255), 128);
            scg_screen_set_pixel(screen, x, y, pixel);
        }
    }

    scg_pixel text_color = SCG_PIXEL_WHITE;
    scg_screen_draw_string(screen, "Hello, World!", screen->width / 2,
                           screen->height / 2, 1, text_color);
}

int main(void) {
    const int width = 256;
    const int height = 240;
    const int scale = 2;
    const int fullscreen = 1;

    scg_screen screen;
    scg_return_status return_status =
        scg_screen_create(&screen, "basic", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("failed to create screen, %s",
                      scg_return_status_get_error(return_status));
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    scg_sound_device sound_device;
    return_status =
        scg_sound_device_create(&sound_device, screen.target_frames_per_sec);
    if (return_status.is_error) {
        scg_log_error("failed to open sound device, %s",
                      scg_return_status_get_error(return_status));
        return -1;
    }
    scg_sound_device_log_info(&sound_device);

    scg_sound music;
    return_status = scg_sound_create_from_wav(
        &sound_device, &music, "assets/arcade-music-loop-joshuaempyre.wav", 1);
    if (return_status.is_error) {
        scg_log_error("failed to create sound, %s",
                      scg_return_status_get_error(return_status));
        return -1;
    }

    scg_sound_play(&music);

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }
        scg_keyboard_update(&keyboard);

        scg_sound_device_update(&sound_device);

        update_and_draw(&screen);
        scg_screen_draw_fps(&screen);

        scg_screen_present(&screen);
    }

    scg_sound_device_destroy(&sound_device);
    scg_screen_destroy(&screen);
    return 0;
}
