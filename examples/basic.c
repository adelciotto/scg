// Music in this demo is 'Arcade Music Loop.wav' by joshuaempyre.
// Music link: https://freesound.org/people/joshuaempyre/sounds/251461/

#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw_background(scg_image_t *image) {
    int w = image->width;
    int h = image->height;

    scg_return_status_t return_status = scg_image_lock(image);
    if (return_status.is_error) {
        scg_log_warn("Background image texture cannot be locked. Error: %s",
                     return_status.error_msg);
        return;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            scg_color_t color = scg_color_create_from_rgb(
                (float32_t)x / (float32_t)w, (float32_t)y / (float32_t)h, 0.5f);

            int i = scg_pixel_index_from_xy(x, y, w);
            image->pixels[i] = scg_image_map_rgba_to_uint32(image, color);
        }
    }

    scg_image_unlock(image);
}

int main(void) {
    const int width = 640;
    const int height = 360;
    const int scale = 1;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen_t screen;
    scg_return_status_t return_status =
        scg_screen_create(&screen, "basic", width, height, scale, fullscreen);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_image_t background_img;
    return_status = scg_image_create(&background_img, &screen, width, height,
                                     SCG_IMAGE_ACCESS_MODE_PIXEL);
    if (return_status.is_error) {
        scg_log_error("Failed to create background image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    draw_background(&background_img);

    scg_keyboard_t keyboard;
    scg_keyboard_create(&keyboard);

    scg_sound_device_t sound_device;
    return_status =
        scg_sound_device_create(&sound_device, screen.target_frames_per_sec);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to open sound device. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_sound_device_log_info(&sound_device);

    scg_sound_t music;
    return_status = scg_sound_create_from_wav(
        &sound_device, &music, "assets/arcade-music-loop-joshuaempyre.wav", 1);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create sound. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    scg_sound_play(&music);

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        scg_screen_clear(&screen);
        scg_screen_draw_image_xy(&screen, &background_img, 0, 0);
        scg_screen_draw_string(&screen, "Hello, World!", screen.width / 2,
                               screen.height / 2, 1);

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_sound_device_update(&sound_device);
        scg_screen_present(&screen);
    }

    scg_sound_device_destroy(&sound_device);
    scg_image_destroy(&background_img);
    scg_screen_destroy(&screen);
    return 0;
}
