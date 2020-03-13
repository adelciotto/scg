#include <math.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define PLASMA_SCALE SCG_PI * 4.0f
#define PLASMA_SCALE_HALF PLASMA_SCALE * 0.5

static void draw_plasma(scg_image_t *plasma_buffer, float32_t t) {
    int w = plasma_buffer->width;
    int h = plasma_buffer->height;

    for (int yi = 0; yi < h; yi++) {
        float32_t y = (0.5f + yi / (float32_t)h - 1.0f) * PLASMA_SCALE -
                      PLASMA_SCALE_HALF;

        for (int xi = 0; xi < w; xi++) {
            float32_t x = (0.5f + xi / (float32_t)w - 1.0f) * PLASMA_SCALE -
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

int main(void) {
    const int width = 640;
    const int height = 480;
    const int window_scale = 1;
    const bool fullscreen = false;

    scg_error_t err = scg_init();
    if (!err.nil) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, width, height);
    if (!err.nil) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Plasma", &back_buffer,
                         window_scale, fullscreen);
    if (!err.nil) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_image_t plasma_buffer;
    err = scg_image_new(&plasma_buffer, 128, 128);
    if (!err.nil) {
        scg_log_error("Failed to create plasma buffer image. Error: %s",
                      err.message);
        return -1;
    }

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_pixel_t clear_color = SCG_COLOR_WHITE;
    float32_t elapsed_time = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 0.6f * screen.target_time_per_frame_secs;

        draw_plasma(&plasma_buffer, elapsed_time);

        scg_image_clear(&back_buffer, clear_color);

        int x = width / 2 - plasma_buffer.width / 2;
        int y = height / 2 - plasma_buffer.height / 2;
        scg_image_draw_image_transform(&back_buffer, &plasma_buffer, x, y,
                                       elapsed_time, 1.0f, 1.0f);

        scg_image_draw_fps(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&plasma_buffer);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
