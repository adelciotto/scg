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
} tunnel_t;

void init_tunnel(tunnel_t *tunnel, int dest_image_w, int dest_image_h, int src_image_w, int src_image_h) {
    uint32_t *distance_buffer = malloc(dest_image_w * dest_image_h * sizeof(*distance_buffer));
    uint32_t *angle_buffer = malloc(dest_image_w * dest_image_h * sizeof(*angle_buffer));
    float32_t *shade_buffer = malloc(dest_image_w * dest_image_h * sizeof(*shade_buffer));

    for (int i = 0; i < dest_image_h; i++) {
        for (int j = 0; j < dest_image_w; j++) {
            int index = scg_pixel_index_from_xy(j, i, dest_image_w);
            float32_t dx = j - dest_image_w / 2;
            float32_t dy = i - dest_image_h / 2;
            float32_t dist = sqrtf(dx * dx + dy * dy);

            distance_buffer[index] =
                (uint32_t)(32.0f * (float32_t)src_image_h / dist) % src_image_h;
            angle_buffer[index] =
                (uint32_t)(0.5f * (float32_t)src_image_w * atan2f(dy, dx) / SCG_PI);
            shade_buffer[index] =
                scg_min_float32(32.0f + dist, 255.0f) / 255.0f;
        }
    }

	tunnel->distance_buffer = distance_buffer;
	tunnel->angle_buffer = angle_buffer;
	tunnel->shade_buffer = shade_buffer;
}

static void draw_tunnel(tunnel_t tunnel, scg_image_t *src_image, scg_image_t *dest_image,
                        float32_t elapsed_time) {
    int dest_image_w = dest_image->width;
    int dest_image_h = dest_image->height;
    int src_image_w = src_image->width;
    int src_image_h = src_image->height;
    int shift_x = (int)floorf((float32_t)src_image_w * elapsed_time * 0.5f);
    int shift_y = (int)floorf((float32_t)src_image_h * elapsed_time * 0.25f);

	scg_image_lock(dest_image);

    for (int i = 0; i < dest_image_h; i++) {
        for (int j = 0; j < dest_image_w; j++) {
            int index = scg_pixel_index_from_xy(j, i, dest_image_w);
            int x = (tunnel.distance_buffer[index] + shift_x) % src_image_w;
            int y = (tunnel.angle_buffer[index] + shift_y) % src_image_h;

            scg_color_t src_color = scg_image_map_uint32_to_rgba(src_image, src_image->pixels[y * src_image_w + x]);

            float32_t shade = tunnel.shade_buffer[index];
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
    const int scale = 1;
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

    scg_image_t src_image;
    return_status = scg_image_create_from_bmp(&src_image, &screen, "assets/space.bmp");
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

	scg_image_t dest_image;
    return_status = scg_image_create(&dest_image, &screen, screen.width, screen.height, SCG_IMAGE_ACCESS_MODE_PIXEL);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create image. Error: %s",
                      return_status.error_msg);
        return -1;
    }

	tunnel_t tunnel;
    init_tunnel(&tunnel, dest_image.width, dest_image.height, src_image.width, src_image.height);

	scg_color_t clear_color = SCG_COLOR_BLACK;
    float32_t elapsed_time = 0.0f;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        elapsed_time += 1.0f * screen.target_time_per_frame_secs;

		scg_screen_set_draw_color(&screen, clear_color);
		scg_screen_clear(&screen);

        draw_tunnel(tunnel, &src_image, &dest_image, elapsed_time);
		scg_screen_draw_image_xy(&screen, &dest_image, 0, 0);

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    free(tunnel.shade_buffer);
    free(tunnel.angle_buffer);
    free(tunnel.distance_buffer);

    scg_image_destroy(&dest_image);
    scg_image_destroy(&src_image);
    scg_screen_destroy(&screen);

    return 0;
}
