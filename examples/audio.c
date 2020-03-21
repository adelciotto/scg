// Music in this demo is 'Arcade Music Loop.wav' by joshuaempyre.
// Music link: https://freesound.org/people/joshuaempyre/sounds/251461/

#define SCG_IMPLEMENTATION
#include "../scg.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_SCALE 1
#define FULLSCREEN false
#define SCREENSHOT_FILEPATH "screenshots/audio.bmp"

int main(void) {
    scg_error_t err = scg_init();
    if (!err.none) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!err.none) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Audio", &back_buffer,
                         WINDOW_SCALE, FULLSCREEN);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_sound_device_t sound_device;
    err = scg_sound_device_new(&sound_device, screen.target_fps);
    if (!err.none) {
        scg_log_error("Failed to open sound device. Error: %s", err.message);
        return -1;
    }
    scg_sound_device_log_info(&sound_device);

    scg_sound_t music;
    err = scg_sound_new_from_wav(&sound_device, &music,
                                 "assets/arcade-music-loop.wav", 1);
    if (!err.none) {
        scg_log_error("Failed to create sound. Error: %s", err.message);
        return -1;
    }

    scg_sound_play(&music);

    scg_pixel_t clear_color = SCG_COLOR_WHITE;
    scg_pixel_t progress_bar_color = SCG_COLOR_BLACK;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_C)) {
            scg_error_t err =
                scg_image_save_to_bmp(&back_buffer, SCREENSHOT_FILEPATH);
            if (!err.none) {
                scg_log_warn("Failed to save screenshot to %s. Error: %s",
                             SCREENSHOT_FILEPATH, err.message);
            }

            scg_log_info("Screenshot saved to %s", SCREENSHOT_FILEPATH);
        }

        scg_image_clear(&back_buffer, clear_color);

        scg_image_draw_string(&back_buffer, "Playing: arcade-music-loop.wav",
                              SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, true,
                              progress_bar_color);

        float32_t music_progress = scg_sound_get_position(&music);
        int progress_bar_max_width = SCREEN_WIDTH - 20;
        int progress_bar_width =
            (int)((float32_t)progress_bar_max_width * music_progress);

        scg_image_draw_rect(&back_buffer, 10, SCREEN_HEIGHT / 2 + 20,
                            progress_bar_max_width, 10, progress_bar_color);
        scg_image_fill_rect(&back_buffer, 10, SCREEN_HEIGHT / 2 + 20,
                            progress_bar_width, 10, progress_bar_color);

        scg_image_draw_frame_metrics(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_sound_device_update(&sound_device);
        scg_screen_present(&screen);
    }

    scg_sound_device_destroy(&sound_device);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
