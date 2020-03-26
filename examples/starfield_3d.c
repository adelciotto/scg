#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define STARFIELD_NUM_STARS 4000
#define STARFIELD_WORLD_SPEED 120.0f
#define STARFIELD_MAX_DISTANCE 300.0f

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

static void init(starfield_t *starfield, scg_app_t *app, int num_stars,
                 float32_t max_distance, float32_t world_speed) {
    int w = app->draw_target->width;
    int h = app->draw_target->height;

    float32_t horizontal_fov = 100.0f * SCG_PI / 180.0f;
    float32_t vertical_fov = 80.0f * SCG_PI / 180.0f;

    starfield->num_stars = num_stars;
    starfield->star_size = 1;
    starfield->max_distance = max_distance;
    starfield->world_speed = world_speed;
    starfield->horizontal_view_distance =
        (float32_t)(w / 2.0f) / tanf(horizontal_fov / 2.0f);
    starfield->vertical_view_distance =
        (float32_t)(h / 2.0f) / tanf(vertical_fov / 2.0f);
    starfield->stars = malloc(num_stars * sizeof(star_t));

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        bool is_super_fast = i % num_stars == 1;

        current->x = (float32_t)(rand() % w) - (float32_t)w / 2.0f;
        current->y = (float32_t)h / 2.0f - (float32_t)(rand() % h);
        current->z = (float32_t)(rand() % (int)max_distance);
        current->is_super_fast = is_super_fast;
    }

    return;
}

static void update(starfield_t *starfield, scg_app_t *app) {
    int w = app->draw_target->width;
    int h = app->draw_target->height;
    float32_t delta_time = app->delta_time;
    int num_stars = starfield->num_stars;

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        float32_t speed_modifier = (current->is_super_fast) ? 3.0f : 1.0f;
        current->z -= (starfield->world_speed * speed_modifier) * delta_time;

        if (current->z <= 0) {
            current->x = (float32_t)(rand() % w) - (float32_t)w / 2.0f;
            current->y = (float32_t)h / 2.0f - (float32_t)(rand() % h);
            current->z = starfield->max_distance + (float32_t)(rand() % 10);
        }
    }

    return;
}

static void draw(scg_image_t *draw_target, starfield_t *starfield) {
    scg_image_clear(draw_target, SCG_COLOR_BLACK);

    int w = draw_target->width;
    int h = draw_target->height;
    int num_stars = starfield->num_stars;

    for (int i = 0; i < num_stars; i++) {
        scg_pixel_t star_color = SCG_COLOR_WHITE;
        star_t *current = &starfield->stars[i];

        float32_t shade =
            scg_max_float32(1.0f - current->z / starfield->max_distance, 0.0f);

        star_color.data.r = (uint8_t)((float32_t)star_color.data.r * shade);
        star_color.data.g = (uint8_t)((float32_t)star_color.data.g * shade);
        star_color.data.b = (uint8_t)((float32_t)star_color.data.b * shade);

        float32_t z = scg_max_float32(current->z, 1.0f);
        float32_t x = (current->x * starfield->horizontal_view_distance) / z;
        x += (float32_t)w / 2.0f;
        float32_t y = (float32_t)h / 2.0f -
                      ((current->y * starfield->vertical_view_distance) / z);

        scg_image_set_pixel(draw_target, (int)x, (int)y, star_color);
    }

    return;
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Starfield 3D";

    scg_app_t app;
    scg_app_init(&app, config);

    srand(scg_get_performance_counter());

    starfield_t starfield;
    init(&starfield, &app, STARFIELD_NUM_STARS, STARFIELD_MAX_DISTANCE,
         STARFIELD_WORLD_SPEED);

    while (app.running) {
        scg_app_begin_frame(&app);

        update(&starfield, &app);

        draw(app.draw_target, &starfield);

        scg_app_end_frame(&app);
    }

    scg_app_shutdown(&app);

    return 0;
}
