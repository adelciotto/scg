// Rotozoom effect implemented with help from the following references:
// - https://seancode.com/demofx/

#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw(scg_image_t *draw_target, scg_image_t *src_image,
                 float32_t angle, float32_t scale) {
    scg_image_clear(draw_target, SCG_COLOR_BLACK);

    int w = draw_target->width;
    int h = draw_target->height;
    const int src_w = src_image->width;
    const int src_h = src_image->height;

    float32_t s = sinf(angle);
    float32_t c = cosf(angle);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float32_t tx = (x * c - y * s) * scale;
            float32_t ty = (x * s + y * c) * scale;

            // No need for mod because image width is 256px.
            int src_x = (int)tx & 255;

            int src_y = (int)ty % src_h;
            while (src_y < 0) {
                src_y += src_h;
            }

            int src_i = scg_pixel_index_from_xy(src_x, src_y, src_w);
            scg_pixel_t color = scg_pixel_new_uint32(src_image->pixels[src_i]);

            int dest_i = scg_pixel_index_from_xy(x, y, w);
            draw_target->pixels[dest_i] = color.packed;
        }
    }
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Rotozoom";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *src_image = scg_image_new_from_bmp("assets/2ndreal.bmp");
    if (src_image == NULL) {
        return -1;
    }

    uint64_t start_time = scg_get_performance_counter();

    while (scg_app_process_events(&app)) {
        float32_t elapsed_time = scg_get_elapsed_time_secs(
            scg_get_performance_counter(), start_time);

        float32_t scale = 0.5f + sinf(elapsed_time * 0.5f) * 2.0f;
        float32_t angle = elapsed_time;
        draw(app.draw_target, src_image, angle, scale);

        scg_app_present(&app);
    }

    scg_image_free(src_image);
    scg_app_free(&app);

    return 0;
}
