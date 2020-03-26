#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw(scg_image_t *draw_target) {
    int w = draw_target->width;
    int h = draw_target->height;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float32_t r =
                scg_min_float32((float32_t)x / (float32_t)w * 255.0f, 255.0f);
            float32_t g =
                scg_min_float32((float32_t)y / (float32_t)h * 255.0f, 255.0f);

            scg_pixel_t color = scg_pixel_new_rgb((uint8_t)r, (uint8_t)g, 128);
            scg_image_set_pixel(draw_target, x, y, color);
        }
    }

    scg_image_draw_string(draw_target, "Hello, World!", w / 2, h / 2, 1,
                          SCG_COLOR_WHITE);

    return;
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Basic";

    scg_app_t app;
    scg_app_init(&app, config);

    while (app.running) {
        scg_app_begin_frame(&app);

        draw(app.draw_target);

        scg_app_end_frame(&app);
    }

    scg_app_shutdown(&app);

    return 0;
}
