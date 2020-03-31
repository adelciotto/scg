#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define STARFIELD_NUM_STARS 1000
#define STARFIELD_SCROLL_SPEED 90.0f
#define STARFIELD_NUM_LAYERS 10

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

static void init(starfield_t *starfield, scg_app_t *app, int num_stars,
                 int num_layers, float32_t scroll_speed) {
    int w = app->draw_target->width;
    int h = app->draw_target->height;

    starfield->num_stars = num_stars;
    starfield->num_layers = num_layers;
    starfield->scroll_speed = scroll_speed;
    starfield->star_size = 1;
    starfield->stars = malloc(num_stars * sizeof(star_t));

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        bool is_super_fast = i % num_stars == 1;

        current->x = (float32_t)(rand() % w);
        current->y = (float32_t)(rand() % h);
        current->layer_index = (is_super_fast) ? 10 : (i % num_layers) + 1;
        current->layer_modifier =
            (float32_t)current->layer_index / (float32_t)num_layers;
        current->is_super_fast = is_super_fast;
    }
}

static void update(starfield_t *starfield, scg_app_t *app) {
    int w = app->draw_target->width;
    int h = app->draw_target->height;
    float32_t delta_time = app->delta_time;
    int num_stars = starfield->num_stars;

    for (int i = 0; i < num_stars; i++) {
        star_t *current = &starfield->stars[i];
        float32_t speed_modifier =
            (current->is_super_fast) ? 3.0f : current->layer_modifier;
        current->y += (starfield->scroll_speed * speed_modifier) * delta_time;

        if (current->y > (h + starfield->star_size)) {
            current->x = (float32_t)(rand() % w);
            current->y = -(float32_t)starfield->star_size * 4.0f;
        }
    }
}

static void draw(scg_image_t *draw_target, starfield_t *starfield) {
    scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_ALPHA);
    scg_image_clear(draw_target, SCG_COLOR_BLACK);

    int num_stars = starfield->num_stars;

    for (int i = 0; i < num_stars; i++) {
        scg_pixel_t star_color = SCG_COLOR_WHITE;
        star_t *current = &starfield->stars[i];

        star_color.data.a =
            (uint8_t)((float32_t)star_color.data.a * current->layer_modifier);

        if (current->is_super_fast) {
            scg_image_fill_rect(draw_target, (int)current->x, (int)current->y,
                                starfield->star_size, starfield->star_size * 2,
                                star_color);
        } else {
            scg_image_set_pixel(draw_target, (int)current->x, (int)current->y,
                                star_color);
        }
    }
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Starfield";

    scg_app_t app;
    scg_app_init(&app, config);

    srand(scg_get_performance_counter());

    starfield_t starfield;
    init(&starfield, &app, STARFIELD_NUM_STARS, STARFIELD_NUM_LAYERS,
         STARFIELD_SCROLL_SPEED);

    while (scg_app_process_events(&app)) {
        update(&starfield, &app);

        draw(app.draw_target, &starfield);

        scg_app_present(&app);
    }

    scg_app_free(&app);

    return 0;
}
