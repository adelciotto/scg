// Metablobs effect implemented with help from the following references:
// - https://seancode.com/demofx/

#define SCG_IMPLEMENTATION
#include "../scg.h"

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

static void metablobs_init(metablobs_t *metablobs, float32_t const_a,
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

static void update(metablobs_t *metablobs, float32_t t) {
    for (int i = 0; i < NUM_BLOBS; i++) {
        blob_t *blob = &metablobs->blobs[i];
        lissajous_get_point(blob->lissajous, &blob->x, &blob->y,
                            t * blob->speed_modifier);
    }
}

static void draw(scg_image_t *draw_target, metablobs_t *metablobs) {
    scg_image_clear(draw_target, SCG_COLOR_WHITE);

    int w = draw_target->width;
    int h = draw_target->height;

    const float32_t origin_x = (float32_t)w / 2.0f;
    const float32_t origin_y = (float32_t)h / 2.0f;
    blob_t *blobs = metablobs->blobs;
    float32_t const_a = metablobs->const_a;
    float32_t const_b = metablobs->const_b;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
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

            int i = scg_pixel_index_from_xy(x, y, w);
            draw_target->pixels[i] = color.packed;
        }
    }
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Metablobs";

    scg_app_t app;
    scg_app_init(&app, config);

    metablobs_t metablobs;
    metablobs_init(&metablobs, METABLOBS_CONST_A, METABLOBS_CONST_B);

    uint64_t start_time = scg_get_performance_counter();

    while (app.running) {
        scg_app_begin_frame(&app);

        float32_t elapsed_time = scg_get_elapsed_time_secs(
            scg_get_performance_counter(), start_time);
        update(&metablobs, elapsed_time);

        draw(app.draw_target, &metablobs);

        scg_app_end_frame(&app);
    }

    scg_app_shutdown(&app);

    return 0;
}
