#define SCG_IMPLEMENTATION
#include "../scg.h"

#define NUM_TWEENS 7

typedef struct tween_example_t {
    scg_tween_t tween;
    const char *name;
    scg_pixel_t color;
} tween_example_t;

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Tween";
    config.video.show_frame_metrics = false;

    scg_app_t app;
    scg_app_init(&app, config);

    const float32_t start_x = 10.0f;
    const float32_t end_x = app.draw_target->width - 64.0f;
    const float32_t duration = 3.0f;
    const bool loop = true;
    const bool yoyo = true;

    tween_example_t tween_examples[NUM_TWEENS] = {
        (tween_example_t){scg_tween_new(scg_tween_definition_new(
                              (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                              scg_tween_linear, duration, loop, yoyo)),
                          "Linear", SCG_COLOR_BLACK},
        (tween_example_t){
            scg_tween_new(scg_tween_definition_new(
                (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                scg_tween_exponential_ease_in, duration, loop, yoyo)),
            "Exponential Ease-In", SCG_COLOR_RED},
        (tween_example_t){
            scg_tween_new(scg_tween_definition_new(
                (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                scg_tween_exponential_ease_out, duration, loop, yoyo)),
            "Exponential Ease-Out", SCG_COLOR_GREEN},
        (tween_example_t){
            scg_tween_new(scg_tween_definition_new(
                (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                scg_tween_exponential_ease_in_out, duration, loop, yoyo)),
            "Exponential Ease-In-Out", SCG_COLOR_BLUE},
        (tween_example_t){scg_tween_new(scg_tween_definition_new(
                              (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                              scg_tween_elastic_ease_in, duration, loop, yoyo)),
                          "Elastic Ease-In", SCG_COLOR_BROWN},
        (tween_example_t){
            scg_tween_new(scg_tween_definition_new(
                (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                scg_tween_elastic_ease_out, duration, loop, yoyo)),
            "Elastic Ease-Out", SCG_COLOR_MAGENTA},
        (tween_example_t){
            scg_tween_new(scg_tween_definition_new(
                (scg_vec2f_t[]){scg_vec2f_new(start_x, end_x)}, 1,
                scg_tween_elastic_ease_in_out, duration, loop, yoyo)),
            "Elastic Ease-In-Out", SCG_COLOR_95_GREEN}};

    for (int i = 0; i < NUM_TWEENS; i++) {
        scg_tween_start(&tween_examples[i].tween, app.elapsed_time);
    }
    bool paused = false;

    while (scg_app_process_events(&app)) {
        if (scg_keyboard_is_key_triggered(app.keyboard, SCG_KEY_P)) {
            for (int i = 0; i < NUM_TWEENS; i++) {
                if (paused) {
                    scg_tween_resume(&tween_examples[i].tween,
                                     app.elapsed_time);
                } else {
                    for (int i = 0; i < NUM_TWEENS; i++) {
                        scg_tween_pause(&tween_examples[i].tween,
                                        app.elapsed_time);
                    }
                }
            }
            paused = !paused;
        }

        scg_image_clear(app.draw_target, SCG_COLOR_WHITE);

        const int window_size = (app.draw_target->height - 16) / NUM_TWEENS;
        int current_y = 16;
        float32_t values[1];

        for (int i = 0; i < NUM_TWEENS; i++) {
            tween_example_t *example = &tween_examples[i];

            scg_tween_update(&example->tween, values, app.elapsed_time);

            scg_image_fill_rect(app.draw_target, values[0],
                                current_y - 14 + window_size / 2, 32, 32,
                                example->color);
            scg_image_draw_string(app.draw_target, example->name, 10, current_y,
                                  false, example->color);
            scg_image_draw_line(app.draw_target, 0, current_y + 12,
                                app.draw_target->width, current_y + 12,
                                example->color);
            current_y += window_size;
        }

        const char *str = paused ? "Press P to resume" : "Press P to pause";
        int w = strlen(str) * SCG_FONT_SIZE;
        scg_image_draw_string(app.draw_target, str,
                              app.draw_target->width - w - 10, 10, false,
                              SCG_COLOR_BLACK);

        scg_app_present(&app);
    }

    scg_app_free(&app);
    return 0;
}
