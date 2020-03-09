// Voxel Space effect implemented with help from the following references:
// - https://github.com/s-macke/VoxelSpace
// - https://en.wikipedia.org/wiki/Voxel_Space
//
// Terrain and heightmap images taken from https://github.com/s-macke/VoxelSpace

#define SCG_IMPLEMENTATION
#include "../scg.h"

typedef struct camera_t {
    float32_t x;
    float32_t y;
    float32_t angle;
    float32_t height;
    float32_t horizon;
    float32_t max_distance;
} camera_t;

typedef struct terrain_t {
    int map_w;
    int map_h;
    scg_image_t color_map;
    uint8_t *height_map;
} terrain_t;

static scg_return_status_t init(terrain_t *terrain, scg_screen_t *screen, camera_t *camera) {
    scg_image_t color_map;
    scg_return_status_t return_status =
        scg_image_create_from_bmp(&color_map, screen, "assets/color_map.bmp");
    if (return_status.is_error == SCG_TRUE) {
        return return_status;
    }

    scg_image_t height_map;
    return_status =
        scg_image_create_from_bmp(&height_map, screen, "assets/height_map.bmp");
    if (return_status.is_error == SCG_TRUE) {
        return return_status;
    }

    if (color_map.width != height_map.width ||
        color_map.height != height_map.height) {
        return scg_return_status_failure(
            "color map dimensions must match height map dimenstions");
    }

    int map_w = height_map.width;
    int map_h = height_map.height;
	uint8_t *height_map_buffer = malloc(map_w * map_h * sizeof(*height_map_buffer));

    // Convert the heightmap to greyscale values between 0..255
    for (int y = 0; y < map_h; y++) {
        for (int x = 0; x < map_w; x++) {
            int i = scg_pixel_index_from_xy(x, y, map_w);
            scg_color_t pixel = scg_image_map_uint32_to_rgba(&height_map, height_map.pixels[i]);
            height_map_buffer[i] = (uint8_t)(pixel.r * 255.0f);
        }
    }

    terrain->map_w = map_w;
    terrain->map_h = map_h;
    terrain->color_map = color_map;
    terrain->height_map = height_map_buffer;

    camera->x = 512.0f;
    camera->y = 800.0f;
    camera->angle = 0.0f;
    camera->height = 78.0f;
    camera->horizon = 100.0f;
    camera->max_distance = 600.0f;

	scg_image_destroy(&height_map);

    return scg_return_status_success();
}

// Reference: https://github.com/hughsk/glsl-fog
/* static float32_t fog_factor(float32_t dist, float32_t density) { */
/*     const float32_t log2 = -1.442695f; */
/*     float32_t d = density * dist; */

/*     return 1.0f - scg_clamp_float32(exp2f(d * d * log2), 0.0f, 1.0f); */
/* } */

static void draw(scg_screen_t *screen, terrain_t terrain, camera_t camera) {
    int screen_w = screen->width;
    int screen_h = screen->height;
    /* int map_w_period = terrain.map_w - 1; */
    /* int map_h_period = terrain.map_h - 1; */

    float32_t s = sinf(camera.angle);
    float32_t c = cosf(camera.angle);

    float32_t ybuffer[screen_w];
    for (int x = 0; x < screen_w; x++) {
        ybuffer[x] = screen_h;
    }

    float32_t dt = 1.0f;
    /* float32_t inv_max_distance = 1.0f / camera.max_distance; */

    for (float32_t z = 1; z < camera.max_distance; z += dt) {
        float32_t pleft_x = -c * z - s * z;
        float32_t pleft_y = s * z - c * z;
        float32_t pright_x = c * z - s * z;
        float32_t pright_y = -s * z - c * z;

        float32_t dx = (pright_x - pleft_x) / (float32_t)screen_w;
        float32_t dy = (pright_y - pleft_y) / (float32_t)screen_w;
        pleft_x += camera.x;
        pleft_y += camera.y;
        float32_t invz = 1.0f / z * 240.0f;

        /* float32_t fog = fog_factor(z * inv_max_distance, 2.4f); */

        for (int x = 0; x < screen_w; x++) {
            /* int i = (((int)pleft_y & map_w_period) << 10) + */
            /*         ((int)pleft_x & map_h_period); */

            /* scg_color_t color = scg_image_map_uint32_to_rgba(&terrain.color_map, terrain.color_map.pixels[i]); */
			int i = scg_pixel_index_from_xy(scg_round_float32(pleft_x), scg_round_float32(pleft_y), terrain.map_w);
            uint8_t height = terrain.height_map[i];

			/* float32_t r = color.r + fog * (255.0f - color.r); */
			/* float32_t g = color.g + fog * (255.0f - color.g); */
			/* float32_t b = color.b + fog * (255.0f - color.b); */
			/* color = scg_color_create_from_rgb(r, g, b); */

            float32_t height_on_screen = (camera.height - height) * invz + camera.horizon;

			scg_rectangle_t src_rect = scg_rectangle_create(scg_round_float32(pleft_x), scg_round_float32(pleft_y), 1, 1);
			scg_rectangle_t dest_rect = scg_rectangle_create(x, height_on_screen, 1, height_on_screen + ybuffer[x]);
			scg_screen_draw_image(screen, &terrain.color_map, src_rect, dest_rect);
			/* scg_screen_draw_line(screen, x, height_on_screen, x, ybuffer[x]); */

            if (height_on_screen < ybuffer[x]) {
                ybuffer[x] = height_on_screen;
            }

            pleft_x += dx;
            pleft_y += dy;
        }

        dt += 0.01f;
    }
}

int main(void) {
    const int width = 400;
    const int height = 240;
    const int scale = 2;
    const bool_t fullscreen = SCG_FALSE;

    scg_screen_t screen;
    scg_return_status_t return_status = scg_screen_create(
        &screen, "voxel space", width, height, scale, fullscreen);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to create screen. Error: %s",
                      return_status.error_msg);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_create(&keyboard);

    terrain_t terrain;
    camera_t camera;
    return_status = init(&terrain, &screen, &camera);
    if (return_status.is_error == SCG_TRUE) {
        scg_log_error("Failed to initialise terrain. Error: %s",
                      return_status.error_msg);
        return -1;
    }

    scg_color_t clear_color = SCG_COLOR_WHITE;

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        float32_t delta_time = screen.target_time_per_frame_secs;

        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_UP)) {
            camera.x += -120.0f * sinf(camera.angle) * delta_time;
            camera.y += -120.0f * cosf(camera.angle) * delta_time;
        }
        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_DOWN)) {
            camera.x += 120.0f * sinf(camera.angle) * delta_time;
            camera.y += 120.0f * cosf(camera.angle) * delta_time;
        }
        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_LEFT)) {
            camera.angle += 1.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_RIGHT)) {
            camera.angle += -1.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_X)) {
            camera.height += -120.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(&keyboard, SCG_KEY_C)) {
            camera.height += 120.0f * delta_time;
        }

        int map_w_period = terrain.map_w - 1;
        int map_h_period = terrain.map_h - 1;
        int map_offset = (((int)camera.y & map_w_period) << 10) +
                         ((int)camera.x & map_h_period);
        int map_height = terrain.height_map[map_offset] + 10;
        if (map_height > camera.height) {
            camera.height = map_height;
        }

		scg_screen_set_draw_color(&screen, clear_color);
        scg_screen_clear(&screen);

        draw(&screen, terrain, camera);

        scg_screen_draw_fps(&screen);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    free(terrain.height_map);
    scg_image_destroy(&terrain.color_map);
    scg_screen_destroy(&screen);

    return 0;
}
