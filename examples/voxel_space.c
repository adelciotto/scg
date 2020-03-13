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
    scg_image_t height_map;
} terrain_t;

static scg_error_t init(terrain_t *terrain, camera_t *camera) {
    scg_image_t color_map;
    scg_error_t err =
        scg_image_new_from_bmp(&color_map, "assets/color_map.bmp");
    if (!err.nil) {
        return err;
    }

    scg_image_t height_map;
    err = scg_image_new_from_bmp(&height_map, "assets/height_map.bmp");
    if (!err.nil) {
        return err;
    }

    if (color_map.width != height_map.width ||
        color_map.height != height_map.height) {
        return scg_error_new(
            "color map dimensions must match height map dimenstions");
    }

    int map_w = height_map.width;
    int map_h = height_map.height;

    // Convert the heightmap to greyscale values between 0..255
    for (int y = 0; y < map_h; y++) {
        for (int x = 0; x < map_w; x++) {
            int i = scg_pixel_index_from_xy(x, y, map_w);

            scg_pixel_t color = scg_pixel_new_uint32(height_map.pixels[i]);
            color.packed = color.data.r;

            height_map.pixels[i] = color.packed;
        }
    }

    terrain->map_w = map_w;
    terrain->map_h = map_h;
    terrain->color_map = color_map;
    terrain->height_map = height_map;

    camera->x = 512.0f;
    camera->y = 800.0f;
    camera->angle = 0.0f;
    camera->height = 78.0f;
    camera->horizon = 120.0f;
    camera->max_distance = 400.0f;

    return scg_error_nil();
}

// Reference: https://github.com/hughsk/glsl-fog
static inline float32_t fog_factor(float32_t dist, float32_t density) {
    const float32_t log2 = -1.442695f;
    float32_t d = density * dist;

    return 1.0f - scg_clamp_float32(exp2f(d * d * log2), 0.0f, 1.0f);
}

static inline scg_pixel_t shade_pixel(scg_pixel_t src, float32_t t) {
    float32_t r = (float32_t)src.data.r;
    float32_t g = (float32_t)src.data.g;
    float32_t b = (float32_t)src.data.b;

    r += t * (float32_t)(255.0f - r);
    g += t * (float32_t)(255.0f - g);
    b += t * (float32_t)(255.0f - b);

    return scg_pixel_new_rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

static inline void draw_vertical_line(scg_image_t *back_buffer, int x0, int y0,
                                      int y1, scg_pixel_t color) {
    if (y0 < 0)
        y0 = 0;
    if (y0 > y1)
        return;

    int screen_w = back_buffer->width;
    uint32_t pixel = color.packed;

    int i = scg_pixel_index_from_xy(x0, y0, screen_w);
    for (int y = y0; y < y1; y++) {
        back_buffer->pixels[i] = pixel;
        i += screen_w;
    }
}

static void draw(scg_image_t *back_buffer, terrain_t terrain, camera_t camera) {
    int screen_w = back_buffer->width;
    int screen_h = back_buffer->height;
    int map_w_period = terrain.map_w - 1;
    int map_h_period = terrain.map_h - 1;

    float32_t s = sinf(camera.angle);
    float32_t c = cosf(camera.angle);

    float32_t ybuffer[screen_w];
    for (int x = 0; x < screen_w; x++) {
        ybuffer[x] = screen_h;
    }

    float32_t dt = 1.0f;
    float32_t inv_max_distance = 1.0f / camera.max_distance;

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

        float32_t fog = fog_factor(z * inv_max_distance, 2.2f);

        for (int x = 0; x < screen_w; x++) {
            int i = (((int)pleft_y & map_w_period) << 10) +
                    ((int)pleft_x & map_h_period);

            scg_pixel_t color_map_pixel =
                scg_pixel_new_uint32(terrain.color_map.pixels[i]);
            scg_pixel_t height_map_pixel =
                scg_pixel_new_uint32(terrain.height_map.pixels[i]);

            color_map_pixel = shade_pixel(color_map_pixel, fog);

            float32_t height_on_screen =
                (camera.height - height_map_pixel.packed) * invz +
                camera.horizon;
            draw_vertical_line(back_buffer, x, height_on_screen, ybuffer[x],
                               color_map_pixel);

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
    err = scg_screen_new(&screen, "SCG Example: Voxel Space", &back_buffer,
                         window_scale, fullscreen);
    if (!err.nil) {
        scg_log_error("Failed to create screen. Error: %s", err.message);
        return -1;
    }
    scg_screen_log_info(&screen);

    scg_keyboard_t keyboard;
    scg_keyboard_new(&keyboard);

    terrain_t terrain;
    camera_t camera;
    err = init(&terrain, &camera);
    if (!err.nil) {
        scg_log_error("Failed to initialise terrain. Error: %s", err.message);
        return -1;
    }

    scg_pixel_t clear_color = SCG_COLOR_WHITE;
    uint64_t delta_time_counter = scg_get_performance_counter();

    while (scg_screen_is_running(&screen)) {
        if (scg_keyboard_is_key_triggered(&keyboard, SCG_KEY_ESCAPE)) {
            scg_screen_close(&screen);
        }

        uint64_t now = scg_get_performance_counter();
        float32_t delta_time =
            scg_get_elapsed_time_secs(now, delta_time_counter);
        delta_time_counter = now;

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
        int map_height =
            scg_pixel_new_uint32(terrain.height_map.pixels[map_offset]).packed +
            10;
        if (map_height > camera.height) {
            camera.height = map_height;
        }

        scg_image_clear(&back_buffer, clear_color);

        draw(&back_buffer, terrain, camera);

        scg_image_draw_fps(&back_buffer, screen.frame_metrics);

        scg_keyboard_update(&keyboard);
        scg_screen_present(&screen);
    }

    scg_image_destroy(&terrain.height_map);
    scg_image_destroy(&terrain.color_map);
    scg_screen_destroy(&screen);
    scg_image_destroy(&back_buffer);
    scg_quit();

    return 0;
}
