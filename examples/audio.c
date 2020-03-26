// Music in this demo is 'Arcade Music Loop.wav' by joshuaempyre.
// Music link: https://freesound.org/people/joshuaempyre/sounds/251461/

#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw(scg_image_t *draw_target, float32_t music_progress) {
    int w = draw_target->width;
    int h = draw_target->height;
    scg_pixel_t clear_color = SCG_COLOR_WHITE;
    scg_pixel_t progress_bar_color = SCG_COLOR_BLACK;

    scg_image_clear(draw_target, clear_color);

    scg_image_draw_string(draw_target, "Playing: arcade-music-loop.wav", w / 2,
                          h / 2 - 20, true, progress_bar_color);

    int progress_bar_max_width = w - 20;
    int progress_bar_width =
        (int)((float32_t)progress_bar_max_width * music_progress);

    scg_image_draw_rect(draw_target, 10, h / 2 + 20, progress_bar_max_width, 10,
                        progress_bar_color);
    scg_image_fill_rect(draw_target, 10, h / 2 + 20, progress_bar_width, 10,
                        progress_bar_color);

    return;
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Audio";
    config.audio.enabled = true;

    scg_app_t app;
    scg_app_init(&app, config);

    scg_sound_t *music =
        scg_sound_new_from_wav(app.audio, "assets/arcade-music-loop.wav", true);
    if (music == NULL) {
        return -1;
    }
    scg_sound_play(music);

    while (app.running) {
        scg_app_begin_frame(&app);

        float32_t play_position = scg_sound_get_position(music);
        draw(app.draw_target, play_position);

        scg_app_end_frame(&app);
    }

    scg_app_shutdown(&app);

    return 0;
}
