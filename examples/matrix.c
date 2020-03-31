#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define CODE_STRING_CHAR_SIZE SCG_FONT_SIZE
#define CODE_STRING_MAX_CHARS 90

typedef struct code_string_t {
    float32_t x;
    float32_t y;
    float32_t speed;
    int num_chars;
    wchar_t chars[CODE_STRING_MAX_CHARS];
} code_string_t;

typedef struct matrix_t {
    int num_columns;
    int num_rows;
    int num_code_strings;
    code_string_t *code_strings;
} matrix_t;

static inline wchar_t random_wchar(void) {
    // One in ten chance of being an english ASCII character.
    if ((rand() % 10) == 0) {
        return (wchar_t)(rand() % (126 - 33) + 33);
    }

    // Otherwise return a Hiragana character.
    return (wchar_t)(rand() % (0x3094 - 0x3041) + 0x3041);
}

static void init_code_string(code_string_t *code_string, int num_columns) {
    float32_t x = (float32_t)((rand() % num_columns) * CODE_STRING_CHAR_SIZE);
    float32_t y = -10.0f;
    float32_t speed = (float32_t)((rand() % 40) + 5);
    int num_chars = (rand() % (CODE_STRING_MAX_CHARS - 10)) + 10;

    for (int i = 0; i < num_chars; i++) {
        code_string->chars[i] = random_wchar();
    }

    code_string->x = x;
    code_string->y = y;
    code_string->speed = speed;
    code_string->num_chars = num_chars;
}

static void matrix_init(matrix_t *matrix, int num_columns, int num_rows) {
    int num_code_strings = num_columns * 2;
    code_string_t *code_strings =
        malloc(num_code_strings * sizeof(*code_strings));

    for (int i = 0; i < num_code_strings; i++) {
        init_code_string(&code_strings[i], num_columns);
    }

    matrix->num_columns = num_columns;
    matrix->num_rows = num_rows;
    matrix->num_code_strings = num_code_strings;
    matrix->code_strings = code_strings;
}

static void update(matrix_t *matrix, scg_app_t *app) {
    int num_code_strings = matrix->num_code_strings;
    int h = app->draw_target->height;
    float32_t delta_time = app->delta_time;

    for (int i = 0; i < num_code_strings; i++) {
        code_string_t *code_string = &matrix->code_strings[i];

        code_string->y += code_string->speed * delta_time;

        int num_chars = code_string->num_chars;
        float32_t code_string_end_y =
            code_string->y - (float32_t)(CODE_STRING_CHAR_SIZE * num_chars);

        if (code_string_end_y >= h) {
            init_code_string(code_string, matrix->num_columns);
        }
    }
}

static inline scg_pixel_t shade_pixel(scg_pixel_t color, float32_t shade) {
    uint8_t r = (uint8_t)((float32_t)color.data.r * shade);
    uint8_t g = (uint8_t)((float32_t)color.data.g * shade);
    uint8_t b = (uint8_t)((float32_t)color.data.b * shade);

    return scg_pixel_new_rgb(r, g, b);
}

static void draw(scg_image_t *draw_target, matrix_t *matrix) {
    scg_image_clear(draw_target, SCG_COLOR_BLACK);

    int num_code_strings = matrix->num_code_strings;

    for (int i = 0; i < num_code_strings; i++) {
        code_string_t *code_string = &matrix->code_strings[i];
        int num_chars = code_string->num_chars;
        float32_t speed = code_string->speed;

        for (int j = 0; j < num_chars; j++) {
            scg_pixel_t color = SCG_COLOR_GREEN;
            if (j == 0) {
                color = SCG_COLOR_WHITE;
            } else if (j <= 3) {
                color = SCG_COLOR_GRAY;
            } else {
                float32_t shade = 1.0f - (float32_t)j / (float32_t)num_chars;
                color = shade_pixel(color, shade);
            }

            float32_t shade = scg_max_float32(0.1f, speed / 45.0f);
            color = shade_pixel(color, shade);

            int char_i =
                abs(j - (int)(code_string->y / CODE_STRING_CHAR_SIZE)) %
                code_string->num_chars;
            int y = (int)(code_string->y - (CODE_STRING_CHAR_SIZE * j));
            scg_image_draw_wchar(draw_target, code_string->chars[char_i],
                                 (int)code_string->x, (int)y, color);

            if (rand() % 1000 < 5) {
                code_string->chars[j] = random_wchar();
            }
        }
    }
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Matrix";

    scg_app_t app;
    scg_app_init(&app, config);

    srand(scg_get_performance_counter());

    int num_columns = app.draw_target->width / CODE_STRING_CHAR_SIZE;
    int num_rows = app.draw_target->height / CODE_STRING_CHAR_SIZE;
    matrix_t matrix;
    matrix_init(&matrix, num_columns, num_rows);

    while (scg_app_process_events(&app)) {
        update(&matrix, &app);

        draw(app.draw_target, &matrix);

        scg_app_present(&app);
    }

    free(matrix.code_strings);
    scg_app_free(&app);

    return 0;
}
