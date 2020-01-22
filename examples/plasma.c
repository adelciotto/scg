#include <math.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define PI 3.1415926535897932384626433832795
#define PLASMA_SCALE 20.0
#define PLASMA_SCALE_HALF PLASMA_SCALE * 0.5

static void update_and_draw(scg_screen *screen, float32_t t) {
    const int width = screen->width;
    const int height = screen->height;

    for (int yi = 0; yi < height; yi++) {
        float32_t y = (0.5f + yi / (float32_t)height - 1.0f) * PLASMA_SCALE -
                      PLASMA_SCALE_HALF;

        for (int xi = 0; xi < width; xi++) {
            float32_t x = (0.5f + xi / (float32_t)width - 1.0f) * PLASMA_SCALE -
                          PLASMA_SCALE_HALF;

            float32_t val = sinf(y + t);
            val += sinf((x + t) * 0.5f);
            val += sinf((x + y + t) * 0.5f);
            float32_t cx = x + PLASMA_SCALE_HALF * (sinf(t * 0.33f));
            float32_t cy = y + PLASMA_SCALE_HALF * (cosf(t * 0.5f));
            val += sinf(sqrt(cx * cx + cy * cy + 1.0f) + t);
            val *= 0.5f;

            float32_t r = sinf(val * PI) * 0.5f + 0.5f;
            float32_t g = sinf(val * PI + 2.0f * PI * 0.33f) * 0.5f + 0.5f;
            float32_t b = sinf(val * PI + 4.0f * PI * 0.33f) * 0.5f + 0.5f;

            scg_pixel pixel = scg_pixel_create((uint8_t)scg_min(r * 255, 255),
                                               (uint8_t)scg_min(g * 255, 255),
                                               (uint8_t)scg_min(b * 255, 255));
            scg_screen_set_pixel(screen, xi, yi, pixel);
        }
    }
}

int main(void) {
    const int width = 256;
    const int height = 240;
    const int scale = 2;
    const int fullscreen = 0;

    scg_screen screen;
    scg_return_status return_status =
        scg_screen_create(&screen, "plasma", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("failed to create screen, %s",
                      scg_return_status_get_error(return_status));
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    scg_pixel clear_color = SCG_PIXEL_BLACK;
    float32_t t = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }
        scg_keyboard_update(&keyboard);

        t += 0.01f;
        scg_screen_clear(&screen, clear_color);
        update_and_draw(&screen, t);
        scg_screen_draw_fps(&screen);

        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    return 0;
}
