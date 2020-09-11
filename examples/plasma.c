#include <math.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define PLASMA_BUFFER_WIDTH 128
#define PLASMA_BUFFER_HEIGHT 128
#define PLASMA_SCALE SCG_PI * 4.0f
#define PLASMA_SCALE_HALF PLASMA_SCALE * 0.5

static void draw_plasma(scg_image_t *plasma_buffer, float32_t t) {
    for (int yi = 0; yi < PLASMA_BUFFER_HEIGHT; yi++) {
        float32_t y = (0.5f + yi / (float32_t)PLASMA_BUFFER_HEIGHT - 1.0f) *
                          PLASMA_SCALE -
                      PLASMA_SCALE_HALF;

        for (int xi = 0; xi < PLASMA_BUFFER_WIDTH; xi++) {
            float32_t x = (0.5f + xi / (float32_t)PLASMA_BUFFER_WIDTH - 1.0f) *
                              PLASMA_SCALE -
                          PLASMA_SCALE_HALF;

            float32_t val = sinf(y + t);
            val += sinf((x + t) * 0.5f);
            val += sinf((x + y + t) * 0.5f);
            float32_t cx = x + PLASMA_SCALE_HALF * (sinf(t * 0.33f));
            float32_t cy = y + PLASMA_SCALE_HALF * (cosf(t * 0.5f));
            val += sinf(sqrt(cx * cx + cy * cy + 1.0f) + t);
            val *= 0.5f;

            float32_t r = fabs(sinf(val * SCG_PI) * 0.5f + 0.5f);
            float32_t g =
                fabs(sinf(val * SCG_PI + 2.0f * SCG_PI * 0.33f) * 0.5f + 0.5f);
            float32_t b =
                fabs(sinf(val * SCG_PI + 4.0f * SCG_PI * 0.33f) * 0.5f + 0.5f);

            scg_pixel_t color =
                scg_pixel_new_rgb((uint8_t)scg_min_float32(r * 255.0f, 255.0f),
                                  (uint8_t)scg_min_float32(g * 255.0f, 255.0f),
                                  (uint8_t)scg_min_float32(b * 255.0f, 255.0f));

            scg_image_set_pixel(plasma_buffer, xi, yi, color);
        }
    }
}

static void draw(scg_image_t *draw_target, scg_image_t *plasma_buffer,
                 float32_t elapsed_time) {
    scg_image_clear(draw_target, SCG_COLOR_WHITE);

    int w = draw_target->width;
    int h = draw_target->height;
    int x = w / 2 - PLASMA_BUFFER_WIDTH / 2;
    int y = h / 2 - PLASMA_BUFFER_HEIGHT / 2;
    scg_image_draw_image_rotate(draw_target, plasma_buffer, x, y, elapsed_time);
}

int main(int arcg, char *argv[]) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Plasma";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *plasma_buffer =
        scg_image_new(PLASMA_BUFFER_WIDTH, PLASMA_BUFFER_HEIGHT);
    if (plasma_buffer == NULL) {
        return -1;
    }

    while (scg_app_process_events(&app)) {
        draw_plasma(plasma_buffer, app.elapsed_time);
        draw(app.draw_target, plasma_buffer, app.elapsed_time);

        scg_app_present(&app);
    }

    scg_image_free(plasma_buffer);
    scg_app_free(&app);

    return 0;
}
