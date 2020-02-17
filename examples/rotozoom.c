#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw_image_with_rotozoom(scg_screen *screen, scg_image image,
                                     float32_t angle, float32_t scale) {
    int screen_w = screen->width;
    int screen_h = screen->height;
    int image_w = image.width;
    int image_h = image.height;

    float32_t s = sinf(angle);
    float32_t c = cosf(angle);

    for (int y = 0; y < screen_h; y++) {
        for (int x = 0; x < screen_w; x++) {
            float32_t tx = (x * c - y * s) * scale;
            float32_t ty = (x * s + y * c) * scale;

            int col = (int)(fabs(tx)) % image_w;
            int row = (int)(fabs(ty)) % image_h;

            scg_pixel pixel;
            pixel.packed = image.pixels[row * image_w + col];
            scg_screen_set_pixel(screen, x, y, pixel);
        }
    }
}

int main(void) {
    const int width = 640;
    const int height = 512;
    const int scale = 1;
    const int fullscreen = 1;

    scg_screen screen;
    scg_return_status return_status = scg_screen_create(
        &screen, "rotozoom", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    scg_image image;
    return_status = scg_image_create_from_tga(&image, "assets/alpha.tga");
    if (return_status.is_error) {
        scg_log_error("Failed to create image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    float32_t elapsed_time = 0.0f;
    scg_color clear_color = SCG_COLOR_95_GREEN;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_time_per_frame_secs;

        scg_screen_clear(&screen, clear_color);

        scg_screen_set_blend_mode(&screen, SCG_BLEND_MODE_ALPHA);
        draw_image_with_rotozoom(&screen, image, elapsed_time,
                                 sinf(elapsed_time * 0.5f));

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&image);
    scg_screen_destroy(&screen);
    return 0;
}
