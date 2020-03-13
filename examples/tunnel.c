// Space image in this demo is by webtreats.
// Profile link:
// http://www.everystockphoto.com/photographer.php?photographer_id=71737 Image
// link: http://www.everystockphoto.com/photo.php?imageId=9268254

// Tunnel effect implemented with help from the following references:
// - https://seancode.com/demofx/
// - https://lodev.org/cgtutor/tunnel.html

#define SCG_IMPLEMENTATION
#include "../scg.h"

typedef struct tunnel_t {
    uint32_t *distance_buffer;
    uint32_t *angle_buffer;
    float32_t *shade_buffer;
    scg_image_t *src_image;
} tunnel_t;

static void init_tunnel(tunnel_t *tunnel, scg_image_t *src_image, int screen_w,
                        int screen_h) {
    uint32_t *distance_buffer =
        malloc(screen_w * screen_h * sizeof(*distance_buffer));
    uint32_t *angle_buffer =
        malloc(screen_w * screen_h * sizeof(*angle_buffer));
    float32_t *shade_buffer =
        malloc(screen_w * screen_h * sizeof(*shade_buffer));

    int image_w = src_image->width;
    int image_h = src_image->height;

    for (int y = 0; y < screen_h; y++) {
        for (int x = 0; x < screen_w; x++) {
            int i = scg_pixel_index_from_xy(x, y, screen_w);
            float32_t dx = x - screen_w / 2;
            float32_t dy = y - screen_h / 2;
            float32_t dist = sqrtf(dx * dx + dy * dy);

            distance_buffer[i] =
                (uint32_t)(32.0f * (float32_t)image_h / dist) % image_h;
            angle_buffer[i] =
                (uint32_t)(0.5f * (float32_t)image_w * atan2f(dy, dx) / SCG_PI);
            shade_buffer[i] = scg_min_float32(16.0f + dist, 255.0f) / 255.0f;
        }
    }

    tunnel->distance_buffer = distance_buffer;
    tunnel->angle_buffer = angle_buffer;
    tunnel->shade_buffer = shade_buffer;
    tunnel->src_image = src_image;
}

static void draw_tunnel(scg_image_t *back_buffer, tunnel_t *tunnel,
                        float32_t elapsed_time) {
    int screen_w = back_buffer->width;
    int screen_h = back_buffer->height;
    int image_w = tunnel->src_image->width;
    int image_h = tunnel->src_image->height;
    int shift_x = (int)floorf((float32_t)image_w * elapsed_time * 0.5f);
    int shift_y = (int)floorf((float32_t)image_h * elapsed_time * 0.25f);

    for (int i = 0; i < screen_h; i++) {
        for (int j = 0; j < screen_w; j++) {
            int dest_i = scg_pixel_index_from_xy(j, i, screen_w);
            int x = (tunnel->distance_buffer[dest_i] + shift_x) % image_w;
            int y = (tunnel->angle_buffer[dest_i] + shift_y) % image_h;

            int src_i = scg_pixel_index_from_xy(x, y, image_w);
            scg_pixel_t color =
                scg_pixel_new_uint32(tunnel->src_image->pixels[src_i]);

            float32_t shade = tunnel->shade_buffer[dest_i];
            color.data.r = (uint8_t)((float32_t)color.data.r * shade);
            color.data.g = (uint8_t)((float32_t)color.data.g * shade);
            color.data.b = (uint8_t)((float32_t)color.data.b * shade);

            back_buffer->pixels[dest_i] = color.packed;
        }
    }
}

int main(void) {
    const int width = 640;
    const int height = 480;
    const int window_scale = 1;
    const bool fullscreen = false;

    scg_error_t err = scg_init();
    if (!err.nil) {
        scg_log_error("Failed to initialise scg. Error: %s", err.message);
        return -1;
    }

    scg_image_t back_buffer;
    err = scg_image_new(&back_buffer, width, height);
    if (!err.nil) {
        scg_log_error("Failed to create back buffer. Error: %s", err.message);
        return -1;
    }

    scg_screen_t screen;
    err = scg_screen_new(&screen, "SCG Example: Tunnel", &back_buffer,
                         window_scale, fullscreen);
    if (!err.nil) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    scg_image_t image;
    err = scg_image_new_from_bmp(&image, "assets/space.bmp");
    if (!err.nil) {
        scg_log_error("Failed to create image. Error: %s", err.message);
        return -1;
    }

    tunnel_t tunnel;
    init_tunnel(&tunnel, &image, back_buffer.width, back_buffer.height);

    float32_t elapsed_time = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_time_per_frame_secs;

        draw_tunnel(&back_buffer, &tunnel, elapsed_time);

        scg_image_draw_fps(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    free(tunnel.shade_buffer);
    free(tunnel.angle_buffer);
    free(tunnel.distance_buffer);
    scg_image_destroy(&image);

    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
