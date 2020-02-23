#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

const int gscreen_w = 400;
const int gscreen_h = 240;

const int gnum_stars = 1000;
const int gstar_size = 1;
const float32_t gscroll_speed = 90.0f;
const int gnum_layers = 10;

typedef struct star {
    float32_t x;
    float32_t y;
    int layer_index;
    float32_t layer_modifier;
    bool_t is_super_fast;
} star;

static void init_stars(int num_stars, star stars[num_stars]) {
    for (int i = 0; i < num_stars; i++) {
        star *current = &stars[i];
        bool_t is_super_fast = i % num_stars == 1;

        current->x = (float32_t)(rand() % gscreen_w);
        current->y = (float32_t)(rand() % gscreen_h);
        current->layer_index = (is_super_fast) ? 10 : (i % gnum_layers) + 1;
        current->layer_modifier =
            (float32_t)current->layer_index / (float32_t)gnum_layers;
        current->is_super_fast = is_super_fast;
    }
}

static void update_stars(int num_stars, star stars[num_stars],
                         float32_t animation_time) {
    for (int i = 0; i < num_stars; i++) {
        star *current = &stars[i];
        float32_t speed_modifier =
            (current->is_super_fast) ? 3.0f : current->layer_modifier;
        current->y += (gscroll_speed * speed_modifier) * animation_time;

        if (current->y > gscreen_h + gstar_size) {
            current->x = (float32_t)(rand() % gscreen_w);
            current->y = -(float32_t)gstar_size * 4.0f;
        }
    }
}

static void draw_stars(scg_screen *screen, int num_stars,
                       star stars[num_stars]) {
    for (int i = 0; i < num_stars; i++) {
        scg_pixel star_color = SCG_COLOR_WHITE;
        star *current = &stars[i];

        star_color.color.a =
            (uint8_t)((float32_t)star_color.color.a * current->layer_modifier);

        if (current->is_super_fast == SCG_TRUE) {
            scg_screen_fill_rect(screen, current->x, current->y, gstar_size,
                                 gstar_size * 2, star_color);
        } else {
            scg_screen_set_pixel(screen, current->x, current->y, star_color);
        }
    }
}

int main(void) {
    const int screen_scale = 2;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen screen;
    scg_return_status return_status = scg_screen_create(
        &screen, "starfield", gscreen_w, gscreen_h, screen_scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    srand(scg_get_performance_counter());

    star stars[gnum_stars];
    init_stars(gnum_stars, stars);

    scg_pixel clear_color = SCG_COLOR_BLACK;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        update_stars(gnum_stars, stars, screen.target_time_per_frame_secs);

        scg_screen_clear(&screen, clear_color);

        scg_screen_set_blend_mode(&screen, SCG_BLEND_MODE_ALPHA);
        draw_stars(&screen, gnum_stars, stars);

        scg_screen_set_blend_mode(&screen, SCG_BLEND_MODE_NONE);
        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    return 0;
}
