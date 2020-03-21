#include <stdlib.h>
#define SCG_IMPLEMENTATION
#include "../scg.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_SCALE 1
#define FULLSCREEN false
#define SCREENSHOT_FILEPATH "screenshots/matrix.bmp"

#define CODE_STRING_CHAR_SIZE SCG_FONT_SIZE
#define CODE_STRING_MAX_CHARS 90
#define MATRIX_NUM_COLUMNS SCREEN_WIDTH / CODE_STRING_CHAR_SIZE
#define MATRIX_NUM_ROWS SCREEN_HEIGHT / CODE_STRING_CHAR_SIZE
#define MATRIX_NUM_CODE_STRINGS MATRIX_NUM_COLUMNS * 2

typedef struct code_string_t {
    float32_t x;
    float32_t y;
    float32_t speed;
    int num_chars;
    wchar_t chars[CODE_STRING_MAX_CHARS];
} code_string_t;

typedef struct matrix_t {
    code_string_t code_strings[MATRIX_NUM_CODE_STRINGS];
} matrix_t;

static inline wchar_t random_wchar(void) {
    // One in ten chance of being an english ASCII character.
    if ((rand() % 10) == 0) {
        return (wchar_t)(rand() % (126 - 33) + 33);
    }

    // Otherwise return a Hiragana character.
    return (wchar_t)(rand() % (0x3094 - 0x3041) + 0x3041);
}

static void init_code_string(code_string_t *code_string) {
    float32_t x =
        (float32_t)((rand() % MATRIX_NUM_COLUMNS) * CODE_STRING_CHAR_SIZE);
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

static void init_matrix(matrix_t *matrix) {
    for (int i = 0; i < MATRIX_NUM_CODE_STRINGS; i++) {
        init_code_string(&matrix->code_strings[i]);
    }
}

static void update_matrix(matrix_t *matrix, float32_t delta_time) {
    for (int i = 0; i < MATRIX_NUM_CODE_STRINGS; i++) {
        code_string_t *code_string = &matrix->code_strings[i];

        code_string->y += code_string->speed * delta_time;

        int num_chars = code_string->num_chars;
        float32_t code_string_end_y =
            code_string->y - (float32_t)(CODE_STRING_CHAR_SIZE * num_chars);

        if (code_string_end_y >= SCREEN_HEIGHT) {
            init_code_string(code_string);
        }
    }
}

static inline scg_pixel_t shade_pixel(scg_pixel_t color, float32_t shade) {
    uint8_t r = (uint8_t)((float32_t)color.data.r * shade);
    uint8_t g = (uint8_t)((float32_t)color.data.g * shade);
    uint8_t b = (uint8_t)((float32_t)color.data.b * shade);

    return scg_pixel_new_rgb(r, g, b);
}

static void draw_matrix(scg_image_t *back_buffer, matrix_t *matrix) {
    for (int i = 0; i < MATRIX_NUM_CODE_STRINGS; i++) {
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
            scg_image_draw_wchar(back_buffer, code_string->chars[char_i],
                                 (int)code_string->x, (int)y, color);

            if (rand() % 1000 < 5) {
                code_string->chars[j] = random_wchar();
            }
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
    err = scg_screen_new(&screen, "SCG Example: Matrix", &back_buffer,
                         WINDOW_SCALE, FULLSCREEN);
    if (!err.none) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    srand(scg_get_performance_counter());

    matrix_t matrix;
    init_matrix(&matrix);

    scg_pixel_t clear_color = SCG_COLOR_BLACK;

    uint64_t delta_time_counter = scg_get_performance_counter();

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_C)) {
            scg_error_t err =
                scg_image_save_to_bmp(&back_buffer, SCREENSHOT_FILEPATH);
            if (!err.none) {
                scg_log_warn("Failed to save screenshot to %s. Error: %s",
                             SCREENSHOT_FILEPATH, err.message);
            }

            scg_log_info("Screenshot saved to %s", SCREENSHOT_FILEPATH);
        }

        uint64_t now = scg_get_performance_counter();
        float32_t delta_time =
            scg_get_elapsed_time_secs(now, delta_time_counter);
        delta_time_counter = now;

        update_matrix(&matrix, delta_time);

        scg_image_clear(&back_buffer, clear_color);

        draw_matrix(&back_buffer, &matrix);

        scg_image_draw_frame_metrics(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
