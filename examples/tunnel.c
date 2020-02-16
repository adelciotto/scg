#define SCG_IMPLEMENTATION
#include "../scg.h"

int main(void) {
    const int width = 512;
    const int height = 512;
    const int scale = 1;
    const int fullscreen = 1;

    scg_screen screen;
    scg_return_status return_status =
        scg_screen_create(&screen, "tunnel", width, height, scale, fullscreen);
    if (return_status.is_error) {
        scg_log_error("failed to create screen, %s", return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard keyboard;
    scg_keyboard_create(&keyboard);

    scg_image image;
    return_status = scg_image_create_from_tga(&image, "assets/space.tga");
    if (return_status.is_error) {
        scg_log_error("failed to create image, %s", return_status.error_msg);
        return -1;
    }

    float32_t elapsed_time = 0.0f;
    scg_color clear_color = SCG_COLOR_BLACK;

	int *distance_buffer = malloc(width * height * sizeof(distance_buffer));
	int *angle_buffer = malloc(width * height * sizeof(angle_buffer));
	float32_t *shade_buffer = malloc(width * height * sizeof(shade_buffer));
	int image_w = image.width;
	int image_h = image.height;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int index = i * width + j;
			float64_t dx = j - (width / 2);
			float64_t dy = i - (height / 2);
			float64_t dist = sqrtf(dx*dx + dy*dy);

			distance_buffer[index] = (int)roundf(32.0f * (float32_t)image_h / dist) % image_h;
			angle_buffer[index] = (int)roundf((float32_t)image_w * atan2f(dy, dx) / SCG_PI);
			shade_buffer[index] = scg_min_float32(roundf(dist), 255.0f) / 255.0f;
		}
	}

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

		elapsed_time += screen.target_time_per_frame_secs;

		int shift_x = (int)floorf((float32_t)image_w * elapsed_time);
		int shift_y = (int)floorf((float32_t)image_h * elapsed_time * 0.25f);

        scg_screen_clear(&screen, clear_color);

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				int x = (distance_buffer[index] + shift_x) & 0xFF;
				int y = (angle_buffer[index] + shift_y) % image_h;
				while (y < 0) {
				  y += image_h;
				}

				scg_pixel pixel;
				pixel.packed = image.pixels[y * image_w + x];

				float32_t shade = shade_buffer[index];
				pixel.color.r = (uint8_t)((float32_t)pixel.color.r * shade);
				pixel.color.g = (uint8_t)((float32_t)pixel.color.g * shade);
				pixel.color.b = (uint8_t)((float32_t)pixel.color.b * shade);
				scg_screen_set_pixel(&screen, i, j, pixel);
			}
		}

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
