// This example is a port of a program I created called
// 'Seabug of Paradise' which was written with processing.
// Link: https://adelciotto.me/2019-05-30-digital-art-seabug-of-paradise.html

#define SCG_IMPLEMENTATION
#include "../scg.h"

#define SEABUG_ANIMATION_SPEED 6.0f
#define SEABUG_NUM_LEGS 32
#define SEABUG_LEFT_SIDE_GRADIENT_START scg_pixel_new_rgb(0, 242, 96)
#define SEABUG_LEFT_SIDE_GRADIENT_END scg_pixel_new_rgb(5, 117, 230)
#define SEABUG_RIGHT_SIDE_GRADIENT_START scg_pixel_new_rgb(255, 106, 0)
#define SEABUG_RIGHT_SIDE_GRADIENT_END scg_pixel_new_rgb(238, 9, 121)

#define BACKGROUND_GRADIENT_START scg_pixel_new_rgb(13, 71, 161)
#define BACKGROUND_GRADIENT_END scg_pixel_new_rgb(13, 17, 38)

typedef struct seabug_leg_t {
    int point_radius;
    float32_t x0, y0;
    float32_t x1, y1;
    float32_t x2, y2;
    scg_pixel_t left_side_color;
    scg_pixel_t right_side_color;
} seabug_leg_t;

typedef struct seabug_t {
    float32_t x, y;
    float32_t origin_x, origin_y;
    seabug_leg_t legs[SEABUG_NUM_LEGS];
    scg_pixel_t left_side_colors[SEABUG_NUM_LEGS];
    scg_pixel_t right_side_colors[SEABUG_NUM_LEGS];
} seabug_t;

static void seabug_leg_update(seabug_leg_t *leg, float32_t t) {
    leg->x0 = sinf(t / 10.0f) * 100.0f + sinf(t / 5.0f) * 20.0f +
              cosf(t / 2.0f) * 3.0f;
    leg->y0 = cosf(t / 10.0f) * 100.0f + sinf(t / 5.0f) * 50.0f;

    leg->x1 = sinf(t / 10.0f) * 200.0f + sinf(t / 4.0f) * 2.0f;
    leg->y1 = -sinf(t / 10.0f) * 200.0f + sinf(t / 12.0f) * 20.0f;

    leg->x2 = sinf(t / 10.0f) * 200.0f + sinf(t / 4.0f) * 2.0f;
    leg->y2 = cosf(t / 10.0f) * 200.0f + sinf(t / 12.0f) * 20.0f;
}

static void seabug_leg_draw(scg_image_t *draw_target, float32_t tx,
                            float32_t ty, seabug_leg_t *leg) {
    float32_t x0 = tx + leg->x0;
    float32_t y0 = ty + leg->y0;
    float32_t x1 = tx + leg->x1;
    float32_t y1 = ty + leg->y1;
    float32_t x2 = tx + leg->x2;
    float32_t y2 = ty + leg->y2;

    scg_pixel_t left_side_color = leg->left_side_color;
    scg_pixel_t right_side_color = leg->right_side_color;

    scg_image_draw_line(draw_target, x0, y0, x1, y1, left_side_color);
    scg_image_draw_line(draw_target, x0, y0, x2, y2, right_side_color);

    int r = leg->point_radius;
    scg_image_fill_circle(draw_target, x0, y0, r, SCG_COLOR_WHITE);
    scg_image_fill_circle(draw_target, x1, y1, r, left_side_color);
    scg_image_fill_circle(draw_target, x2, y2, r, right_side_color);
}

static void linear_gradient_colors(scg_pixel_t *out, int stops,
                                   scg_pixel_t start, scg_pixel_t end) {
    float32_t step_factor = 1.0f / (float32_t)(stops - 1);

    for (int i = 0; i < stops; i++) {
        float32_t t = step_factor * (float32_t)i;
        out[i] = scg_pixel_lerp_rgb(start, end, t);
    }
}

static void init(scg_app_t *app, seabug_t *seabug) {
    seabug->x = 0.0f;
    seabug->y = 0.0f;
    seabug->origin_x = app->draw_target->width / 2;
    seabug->origin_y = app->draw_target->height / 2;

    linear_gradient_colors(seabug->left_side_colors, SEABUG_NUM_LEGS,
                           SEABUG_LEFT_SIDE_GRADIENT_START,
                           SEABUG_LEFT_SIDE_GRADIENT_END);
    linear_gradient_colors(seabug->right_side_colors, SEABUG_NUM_LEGS,
                           SEABUG_RIGHT_SIDE_GRADIENT_START,
                           SEABUG_RIGHT_SIDE_GRADIENT_END);

    for (int i = 0; i < SEABUG_NUM_LEGS; i++) {
        seabug->legs[i] =
            (seabug_leg_t){.x0 = 0.0f,
                           .y0 = 0.0f,
                           .x1 = 0.0f,
                           .y1 = 0.0f,
                           .x2 = 0.0f,
                           .y2 = 0.0f,
                           .point_radius = 2,
                           .left_side_color = seabug->left_side_colors[i],
                           .right_side_color = seabug->right_side_colors[i]};
    }
}

static void update(seabug_t *seabug, float32_t animation_time) {
    const float32_t lissajous_scale = 100.0f;
    const float32_t lissajous_a = 3.0f;
    const float32_t lissajous_b = 4.0f;
    float32_t lissajous_t = animation_time * 0.01f;

    seabug->x = lissajous_scale * cosf(lissajous_a * lissajous_t);
    seabug->y = lissajous_scale * sinf(lissajous_b * lissajous_t);

    for (int i = 0; i < SEABUG_NUM_LEGS; i++) {
        seabug_leg_update(&seabug->legs[i], animation_time + i);
    }
}

static void draw_background(scg_image_t *draw_target, float32_t cx,
                            float32_t cy) {
    float32_t w = draw_target->width;
    float32_t h = draw_target->height;

    float32_t d = sqrtf(w * w + h * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float32_t dx = cx - (float32_t)x;
            float32_t dy = cy - (float32_t)y;
            float32_t dist = sqrtf(dx * dx + dy * dy);

            float32_t t = (dist / d) * 2.0f;
            scg_pixel_t color = scg_pixel_lerp_rgb(BACKGROUND_GRADIENT_START,
                                                   BACKGROUND_GRADIENT_END, t);

            int i = scg_pixel_index_from_xy(x, y, w);
            draw_target->pixels[i] = color.packed;
        }
    }
}

static void draw(scg_image_t *draw_target, seabug_t seabug) {
    float32_t x = seabug.origin_x + seabug.x;
    float32_t y = seabug.origin_y + seabug.y;

    draw_background(draw_target, x, y);

    for (int i = 0; i < SEABUG_NUM_LEGS; i++) {
        seabug_leg_draw(draw_target, x, y, &seabug.legs[i]);
    }
}

int main(int arcg, char *argv[]) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Seabug";

    scg_app_t app;
    scg_app_init(&app, config);

    seabug_t seabug;
    init(&app, &seabug);

    float32_t animation_time = 0.0f;

    while (scg_app_process_events(&app)) {
        animation_time += SEABUG_ANIMATION_SPEED * app.delta_time;
        update(&seabug, animation_time);

        draw(app.draw_target, seabug);

        scg_app_present(&app);
    }

    scg_app_free(&app);

    return 0;
}
