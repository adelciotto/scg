#include <math.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define PLASMA_SCALE SCG_PI * 3.0f
#define PLASMA_SCALE_HALF PLASMA_SCALE * 0.5

static void update_and_draw(scg_screen_t *screen, float32_t t) {
    const int width = screen->width;
    const int height = screen->height;

    for (int yi = 0; yi < height; yi += 4) {
        float32_t y = (0.5f + yi / (float32_t)height - 1.0f) * PLASMA_SCALE -
                      PLASMA_SCALE_HALF;

        for (int xi = 0; xi < width; xi += 4) {
            float32_t x = (0.5f + xi / (float32_t)width - 1.0f) * PLASMA_SCALE -
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

            scg_color_t color = scg_color_create_from_rgb(r, g, b);

            scg_screen_set_draw_color(screen, color);
            scg_screen_fill_rect(screen, xi, yi, 4, 4);
        }
    }
}

int main(void) {
    const int width = 640;
    const int height = 360;
    const int scale = 2;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen_t screen;
    scg_return_status_t return_status =
        scg_screen_create(&screen, "plasma", width, height, scale, fullscreen);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_create(&keyboard);

    scg_color_t clear_color = SCG_COLOR_BLACK;
    float32_t elapsed_time = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_time_per_frame_secs;

        scg_screen_set_draw_color(&screen, clear_color);
        scg_screen_clear(&screen);

        update_and_draw(&screen, elapsed_time);

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    return 0;
}
