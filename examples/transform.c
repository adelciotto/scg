#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw(scg_image_t *draw_target, scg_image_t *image,
                 float32_t elapsed_time) {
    scg_image_clear(draw_target, SCG_COLOR_95_GREEN);

    int w = draw_target->width;
    int h = draw_target->height;

    scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_ALPHA);
    scg_image_draw_image_rotate_scale(draw_target, image, w / 2 - image->height,
                                      h / 2 - image->width, elapsed_time, 2.0f,
                                      2.0f);

    return;
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Transform";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *image = scg_image_new_from_bmp("assets/ball.bmp");
    if (image == NULL) {
        return -1;
    }

    uint64_t start_time = scg_get_performance_counter();

    while (app.running) {
        scg_app_begin_frame(&app);

        float32_t elapsed_time = scg_get_elapsed_time_secs(
            scg_get_performance_counter(), start_time);
        draw(app.draw_target, image, elapsed_time);

        scg_app_end_frame(&app);
    }

    scg_image_free(image);
    scg_app_shutdown(&app);

    return 0;
}
