#define SCG_IMPLEMENTATION
#include "../scg.h"

static uint32_t *distance_buffer;
static uint32_t *angle_buffer;
static float32_t *shade_buffer;

static void init_tunnel(int screen_w, int screen_h, int image_w, int image_h) {
    distance_buffer = malloc(screen_w * screen_h * sizeof(*distance_buffer));
    angle_buffer = malloc(screen_w * screen_h * sizeof(*angle_buffer));
    shade_buffer = malloc(screen_w * screen_h * sizeof(*shade_buffer));

    for (int i = 0; i < screen_h; i++) {
        for (int j = 0; j < screen_w; j++) {
            int index = i * screen_w + j;
            float32_t dx = j - screen_w / 2;
            float32_t dy = i - screen_h / 2;
            float32_t dist = sqrtf(dx * dx + dy * dy);

            distance_buffer[index] =
                (uint32_t)(32.0f * (float32_t)image_h / dist) % image_h;
            angle_buffer[index] =
                (uint32_t)(0.5f * (float32_t)image_w * atan2f(dy, dx) / SCG_PI);
            shade_buffer[index] = scg_min_float32(dist, 255.0f) / 255.0f;
        }
    }
}

static void draw_tunnel(scg_screen_t *screen, scg_image_t image,
                        float32_t elapsed_time) {
    int screen_w = screen->width;
    int screen_h = screen->height;
    int image_w = image.width;
    int image_h = image.height;
    int shift_x = (int)floorf((float32_t)image_w * elapsed_time * 0.5f);
    int shift_y = (int)floorf((float32_t)image_h * elapsed_time * 0.25f);

    for (int i = 0; i < screen_h; i++) {
        for (int j = 0; j < screen_w; j++) {
            int index = i * screen_w + j;
            int x = (distance_buffer[index] + shift_x) % image_w;
            int y = (angle_buffer[index] + shift_y) % image_h;

            scg_pixel_t pixel;
            pixel.packed = image.pixels[y * image_w + x];

            float32_t shade = shade_buffer[index];
            pixel.color.r = (uint8_t)((float32_t)pixel.color.r * shade);
            pixel.color.g = (uint8_t)((float32_t)pixel.color.g * shade);
            pixel.color.b = (uint8_t)((float32_t)pixel.color.b * shade);
            scg_screen_set_pixel(screen, j, i, pixel);
        }
    }
}

int main(void) {
    const int width = 400;
    const int height = 240;
    const int scale = 2;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen_t screen;
    scg_return_status_t return_status =
        scg_screen_create(&screen, "tunnel", width, height, scale, fullscreen);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_create(&keyboard);

    scg_image_t image;
    return_status = scg_image_create_from_tga(&image, "assets/space.tga");
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    init_tunnel(screen.width, screen.height, image.width, image.height);

    float32_t elapsed_time = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_time_per_frame_secs;

        draw_tunnel(&screen, image, elapsed_time);
        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    free(shade_buffer);
    free(angle_buffer);
    free(distance_buffer);

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    return 0;
}
