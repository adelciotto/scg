#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

const int gscreen_w = 640;
const int gscreen_h = 480;

typedef struct star_t {
    float32_t x;
    float32_t y;
    float32_t z;
    bool is_super_fast;
} star_t;

typedef struct starfield_t {
    int num_stars;
    star_t *stars;
    int star_size;
    float32_t max_distance;
    float32_t world_speed;
    float32_t horizontal_view_distance;
    float32_t vertical_view_distance;
} starfield_t;

static void init_starfield(starfield_t *starfield, int num_stars,
                           float32_t max_distance, float32_t world_speed) {
    float32_t horizontal_fov = 100.0f * SCG_PI / 180.0f;
    float32_t vertical_fov = 80.0f * SCG_PI / 180.0f;

    starfield->num_stars = num_stars;
    starfield->star_size = 1;
    starfield->max_distance = max_distance;
    starfield->world_speed = world_speed;
    starfield->horizontal_view_distance =
        (float32_t)(gscreen_w / 2.0f) / tanf(horizontal_fov / 2.0f);
    starfield->vertical_view_distance =
        (float32_t)(gscreen_h / 2.0f) / tanf(vertical_fov / 2.0f);
    starfield->stars = malloc(num_stars * sizeof(star_t));

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        bool is_super_fast = i % num_stars == 1;

        current->x =
            (float32_t)(rand() % gscreen_w) - (float32_t)gscreen_w / 2.0f;
        current->y =
            (float32_t)gscreen_h / 2.0f - (float32_t)(rand() % gscreen_h);
        current->z = (float32_t)(rand() % (int)max_distance);
        current->is_super_fast = is_super_fast;
    }
}

static void update_starfield(starfield_t starfield, float32_t animation_time) {
    for (int i = 0; i < starfield.num_stars; i++) {
        star_t *current = &starfield.stars[i];
        float32_t speed_modifier = (current->is_super_fast) ? 3.0f : 1.0f;
        current->z -= (starfield.world_speed * speed_modifier) * animation_time;

        if (current->z <= 0) {
            current->x =
                (float32_t)(rand() % gscreen_w) - (float32_t)gscreen_w / 2.0f;
            current->y =
                (float32_t)gscreen_h / 2.0f - (float32_t)(rand() % gscreen_h);
            current->z = starfield.max_distance + (float32_t)(rand() % 10);
        }
    }
}

static void draw_starfield(scg_image_t *back_buffer, starfield_t starfield) {
    for (int i = 0; i < starfield.num_stars; i++) {
        scg_pixel_t star_color = SCG_COLOR_WHITE;
        star_t *current = &starfield.stars[i];

        float32_t shade =
            scg_max_float32(1.0f - current->z / starfield.max_distance, 0.0f);

        star_color.data.r = (uint8_t)((float32_t)star_color.data.r * shade);
        star_color.data.g = (uint8_t)((float32_t)star_color.data.g * shade);
        star_color.data.b = (uint8_t)((float32_t)star_color.data.b * shade);

        float32_t z = scg_max_float32(current->z, 1.0f);
        float32_t x = (current->x * starfield.horizontal_view_distance) / z;
        x += (float32_t)gscreen_w / 2.0f;
        float32_t y = (float32_t)gscreen_h / 2.0f -
                      ((current->y * starfield.vertical_view_distance) / z);

        scg_image_set_pixel(back_buffer, (int)x, (int)y, star_color);
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

    const int num_stars = 4000;
    const float32_t world_speed = 120.0f;
    const float32_t max_distance = 300.0f;

    starfield_t starfield;
    init_starfield(&starfield, num_stars, max_distance, world_speed);

    scg_pixel_t clear_color = SCG_COLOR_BLACK;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        update_starfield(starfield, screen.target_time_per_frame_secs);

        scg_image_clear(&back_buffer, clear_color);

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
