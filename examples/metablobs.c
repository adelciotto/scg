// Metablobs effect implemented with help from the following references:
// - https://seancode.com/demofx/

#define SCG_IMPLEMENTATION
#include "../scg.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define NUM_BLOBS 3
#define METABLOBS_CONST_A 300.0f
#define METABLOBS_CONST_B 1024.0f

typedef struct lissajous_t {
    float32_t scale_x;
    float32_t scale_y;
    float32_t a;
    float32_t b;
    float32_t delta;
} lissajous_t;

typedef struct blob_t {
    float32_t x;
    float32_t y;
    lissajous_t lissajous;
    float32_t speed_modifier;
} blob_t;

typedef struct metablobs_t {
    blob_t blobs[NUM_BLOBS];
    float32_t const_a;
    float32_t const_b;
} metablobs_t;

static void lissajous_get_point(lissajous_t lissajous, float32_t *out_x,
                                float32_t *out_y, float32_t t) {
    // Reference:
    // https://en.wikipedia.org/wiki/Parametric_equation#Lissajous_Curve
    *out_x = lissajous.scale_x * cosf(lissajous.a * t);
    *out_y = lissajous.scale_y * sinf(lissajous.b * t);
}

static void init_metablobs(metablobs_t *metablobs, float32_t const_a,
                           float32_t const_b) {
    blob_t *blobs = metablobs->blobs;

    blobs[0].lissajous =
        (lissajous_t){.scale_x = 60.0f, .scale_y = 60.0f, .a = 3.0f, .b = 2.0f};
    lissajous_get_point(blobs[0].lissajous, &blobs[0].x, &blobs[0].y, 0.0f);
    blobs[0].speed_modifier = 1.0f;

    blobs[1].lissajous =
        (lissajous_t){.scale_x = 80.0f, .scale_y = 80.0f, .a = 1.0f, .b = 2.0f};
    lissajous_get_point(blobs[1].lissajous, &blobs[1].x, &blobs[1].y, 0.0f);
    blobs[1].speed_modifier = 0.5f;

    blobs[2].lissajous =
        (lissajous_t){.scale_x = 50.0f, .scale_y = 50.0f, .a = 3.0f, .b = 4.0f};
    lissajous_get_point(blobs[2].lissajous, &blobs[2].x, &blobs[2].y, 0.0f);
    blobs[2].speed_modifier = 0.33f;

    metablobs->const_a = const_a;
    metablobs->const_b = const_b;
}

static void update_metablobs(metablobs_t *metablobs, float32_t t) {
    for (int i = 0; i < NUM_BLOBS; i++) {
        blob_t *blob = &metablobs->blobs[i];
        lissajous_get_point(blob->lissajous, &blob->x, &blob->y,
                            t * blob->speed_modifier);
    }
}

static void draw_metablobs(scg_image_t *back_buffer, metablobs_t *metablobs) {
    const float32_t origin_x = (float32_t)SCREEN_WIDTH / 2.0f;
    const float32_t origin_y = (float32_t)SCREEN_HEIGHT / 2.0f;
    blob_t *blobs = metablobs->blobs;
    float32_t const_a = metablobs->const_a;
    float32_t const_b = metablobs->const_b;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            float32_t distance_product = 1.0f;

            for (int i = 0; i < NUM_BLOBS; i++) {
                float32_t blob_x = origin_x + blobs[i].x;
                float32_t blob_y = origin_y + blobs[i].y;
                float32_t dx = (float32_t)x - blob_x;
                float32_t dy = (float32_t)y - blob_y;
                distance_product *= sqrtf((dx * dx) + (dy * dy));
            }

            float32_t result = const_b - distance_product / const_a;
            uint8_t clamped = 255 - (uint8_t)scg_clamp_float32(result, 0, 255);
            scg_pixel_t color = scg_pixel_new_rgb(clamped, clamped, clamped);

            int i = scg_pixel_index_from_xy(x, y, SCREEN_WIDTH);
            back_buffer->pixels[i] = color.packed;
        }
    }
}

int main(void) {
    scg_error_t err = scg_init();
    if (!err.none) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!err.none) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Metablobs", &back_buffer, 2,
                         false);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    metablobs_t metablobs;
    init_metablobs(&metablobs, METABLOBS_CONST_A, METABLOBS_CONST_B);

    uint64_t delta_time_counter = scg_get_performance_counter();
    float32_t t = 0.0f;
    scg_pixel_t clear_color = SCG_COLOR_WHITE;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        uint64_t now = scg_get_performance_counter();
        float32_t delta_time =
            scg_get_elapsed_time_secs(now, delta_time_counter);
        delta_time_counter = now;

        t += 1.0f * delta_time;
        update_metablobs(&metablobs, t);

        scg_image_clear(&back_buffer, clear_color);

        draw_metablobs(&back_buffer, &metablobs);

        scg_image_draw_frame_metrics(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
