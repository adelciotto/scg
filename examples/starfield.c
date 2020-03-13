#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

const int gscreen_w = 640;
const int gscreen_h = 480;

typedef struct star_t {
    float32_t x;
    float32_t y;
    int layer_index;
    float32_t layer_modifier;
    bool is_super_fast;
} star_t;

typedef struct starfield_t {
    int num_stars;
    star_t *stars;
    int num_layers;
    int star_size;
    float32_t scroll_speed;
} starfield_t;

static void init_starfield(starfield_t *starfield, int num_stars,
                           int num_layers, float32_t scroll_speed) {
    starfield->num_stars = num_stars;
    starfield->num_layers = num_layers;
    starfield->scroll_speed = scroll_speed;
    starfield->star_size = 1;
    starfield->stars = malloc(num_stars * sizeof(star_t));

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        bool is_super_fast = i % num_stars == 1;

        current->x = (float32_t)(rand() % gscreen_w);
        current->y = (float32_t)(rand() % gscreen_h);
        current->layer_index = (is_super_fast) ? 10 : (i % num_layers) + 1;
        current->layer_modifier =
            (float32_t)current->layer_index / (float32_t)num_layers;
        current->is_super_fast = is_super_fast;
    }
}

static void update_starfield(starfield_t starfield, float32_t animation_time) {
    for (int i = 0; i < starfield.num_stars; i++) {
        star_t *current = &starfield.stars[i];
        float32_t speed_modifier =
            (current->is_super_fast) ? 3.0f : current->layer_modifier;
        current->y +=
            (starfield.scroll_speed * speed_modifier) * animation_time;

        if (current->y > gscreen_h + starfield.star_size) {
            current->x = (float32_t)(rand() % gscreen_w);
            current->y = -(float32_t)starfield.star_size * 4.0f;
        }
    }
}

static void draw_starfield(scg_image_t *back_buffer, starfield_t starfield) {
    for (int i = 0; i < starfield.num_stars; i++) {
        scg_pixel_t star_color = SCG_COLOR_WHITE;
        star_t *current = &starfield.stars[i];

        star_color.data.a =
            (uint8_t)((float32_t)star_color.data.a * current->layer_modifier);

        if (current->is_super_fast) {
            scg_image_fill_rect(back_buffer, (int)current->x, (int)current->y,
                                starfield.star_size, starfield.star_size * 2,
                                star_color);
        } else {
            scg_image_set_pixel(back_buffer, (int)current->x, (int)current->y,
                                star_color);
        }
    }
}

int main(void) {
    const int window_scale = 1;
    const bool fullscreen = false;

    scg_error_t err = scg_init();
    if (!err.nil) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, gscreen_w, gscreen_h);
    if (!err.nil) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Starfield", &back_buffer,
                         window_scale, fullscreen);
    if (!err.nil) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    srand(scg_get_performance_counter());

    const int num_stars = 1000;
    const float32_t scroll_speed = 90.0f;
    const int num_layers = 10;

    starfield_t starfield;
    init_starfield(&starfield, num_stars, num_layers, scroll_speed);

    scg_pixel_t clear_color = SCG_COLOR_BLACK;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        update_starfield(starfield, screen.target_time_per_frame_secs);

        scg_image_clear(&back_buffer, clear_color);

        scg_image_set_blend_mode(&back_buffer, SCG_BLEND_MODE_ALPHA);
        draw_starfield(&back_buffer, starfield);

        scg_image_draw_fps(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    free(starfield.stars);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
