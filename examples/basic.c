#include <stdio.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

static void update_and_draw(scg_screen *screen) {
    scg_screen_lock(screen);

    for (int y = 0; y < screen->height; y++) {
        for (int x = 0; x < screen->width; x++) {
            scg_screen_set_pixel(screen, x, y, (uint8)scg_min(x, 255),
                                 (uint8)scg_min(y, 255), 128);
        }
    }

    scg_screen_unlock(screen);
}

int main(void) {
    const int width = 256;
    const int height = 256;
    const int scale = 2;
    const int fullscreen = 0;

    scg_screen screen;
    scg_return_status return_status =
        scg_screen_create(&screen, "basic", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("failed to create screen, %s",
                      scg_return_status_get_error(return_status));
        return -1;
    }
    scg_screen_log_info(&screen);

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

    uint64 last_frame_performance_counter = scg_get_performance_counter();
    while (scg_screen_is_running(&screen)) {
        scg_sound_device_update(&sound_device);

        update_and_draw(&screen);
        scg_screen_present(&screen);

        if (scg_get_elapsed_time_secs(scg_get_performance_counter(),
                                      last_frame_performance_counter) > 1.0) {
            printf("fps:%f, s/f:%f, ms/f:%f\r", screen.frame_metrics.fps,
                   screen.frame_metrics.time_per_frame_secs,
                   screen.frame_metrics.time_per_frame_millisecs);
            fflush(stdout);
            last_frame_performance_counter = scg_get_performance_counter();
        }
    }

    scg_sound_device_destroy(&sound_device);
    scg_screen_destroy(&screen);
    return 0;
}
