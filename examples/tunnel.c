// Space image in this demo is by webtreats.
// Profile link:
// http://www.everystockphoto.com/photographer.php?photographer_id=71737 Image
// link: http://www.everystockphoto.com/photo.php?imageId=9268254

// Tunnel effect implemented with help from the following references:
// - https://seancode.com/demofx/
// - https://lodev.org/cgtutor/tunnel.html

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
            int index = scg_pixel_index_from_xy(j, i, screen_w);
            float32_t dx = j - screen_w / 2;
            float32_t dy = i - screen_h / 2;
            float32_t dist = sqrtf(dx * dx + dy * dy);

            distance_buffer[index] =
                (uint32_t)(32.0f * (float32_t)image_h / dist) % image_h;
            angle_buffer[index] =
                (uint32_t)(0.5f * (float32_t)image_w * atan2f(dy, dx) / SCG_PI);
            shade_buffer[index] =
                scg_min_float32(16.0f + dist, 255.0f) / 255.0f;
        }
    }
}

static void draw_tunnel(scg_screen_t *screen, scg_image_t *src_image, scg_image_t *dest_image,
                        float32_t elapsed_time) {
    int screen_w = screen->width;
    int screen_h = screen->height;
    int image_w = image.width;
    int image_h = image.height;
    int shift_x = (int)floorf((float32_t)image_w * elapsed_time * 0.5f);
    int shift_y = (int)floorf((float32_t)image_h * elapsed_time * 0.25f);

	scg_image_lock(dest_image);

    for (int i = 0; i < screen_h; i++) {
        for (int j = 0; j < screen_w; j++) {
            int index = scg_pixel_index_from_xy(j, i, screen_w);
            int x = (distance_buffer[index] + shift_x) % image_w;
            int y = (angle_buffer[index] + shift_y) % image_h;

            scg_color_t src_color = scg_color_from_uint32(src_image->pixels[y * image_w + x]);

            float32_t shade = shade_buffer[index];
            src_color.r *= shade;
            src_color.g *= shade;
            src_color.b *= shade;

			dest_image->pixels[index] = scg_image_map_rgba_to_uint32(dest_image, src_color);
        }
    }

	scg_image_unlock(dest_image);
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
    return_status = scg_image_create_from_bmp(&image, "assets/space.bmp");
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
