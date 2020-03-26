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
    scg_image_t *color_map;
    scg_image_t *height_map;
} terrain_t;

static bool init(terrain_t *terrain, camera_t *camera) {
    scg_image_t *color_map = scg_image_new_from_bmp("assets/color_map.bmp");
    if (color_map == NULL) {
        return false;
    }

    scg_image_t *height_map = scg_image_new_from_bmp("assets/height_map.bmp");
    if (height_map == NULL) {
        return false;
    }

    if (color_map->width != height_map->width ||
        color_map->height != height_map->height) {
        scg_log_error("color map dimensions must match height map dimenstions");
        return false;
    }

    int map_w = height_map->width;
    int map_h = height_map->height;

    // Convert the heightmap to greyscale values between 0..255
    for (int y = 0; y < map_h; y++) {
        for (int x = 0; x < map_w; x++) {
            int i = scg_pixel_index_from_xy(x, y, map_w);

            scg_pixel_t color = scg_pixel_new_uint32(height_map->pixels[i]);
            color.packed = color.data.r;

            height_map->pixels[i] = color.packed;
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

    return true;
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

static inline void draw_vertical_line(scg_image_t *draw_target, int x0, int y0,
                                      int y1, scg_pixel_t color) {
    if (y0 < 0)
        y0 = 0;
    if (y0 > y1)
        return;

    int w = draw_target->width;
    uint32_t pixel = color.packed;

    int i = scg_pixel_index_from_xy(x0, y0, w);
    for (int y = y0; y < y1; y++) {
        draw_target->pixels[i] = pixel;
        i += w;
    }
}

static void draw(scg_image_t *draw_target, terrain_t terrain, camera_t camera) {
    scg_image_clear(draw_target, SCG_COLOR_WHITE);

    int w = draw_target->width;
    int h = draw_target->height;
    int map_w_period = terrain.map_w - 1;
    int map_h_period = terrain.map_h - 1;

    float32_t s = sinf(camera.angle);
    float32_t c = cosf(camera.angle);

    float32_t ybuffer[w];
    for (int x = 0; x < w; x++) {
        ybuffer[x] = h;
    }

    float32_t dt = 1.0f;
    float32_t inv_max_distance = 1.0f / camera.max_distance;

    for (float32_t z = 1; z < camera.max_distance; z += dt) {
        float32_t pleft_x = -c * z - s * z;
        float32_t pleft_y = s * z - c * z;
        float32_t pright_x = c * z - s * z;
        float32_t pright_y = -s * z - c * z;

        float32_t dx = (pright_x - pleft_x) / (float32_t)w;
        float32_t dy = (pright_y - pleft_y) / (float32_t)w;
        pleft_x += camera.x;
        pleft_y += camera.y;
        float32_t invz = 1.0f / z * 240.0f;

        float32_t fog = fog_factor(z * inv_max_distance, 2.2f);

        for (int x = 0; x < w; x++) {
            int i = (((int)pleft_y & map_w_period) << 10) +
                    ((int)pleft_x & map_h_period);

            scg_pixel_t color_map_pixel =
                scg_pixel_new_uint32(terrain.color_map->pixels[i]);
            scg_pixel_t height_map_pixel =
                scg_pixel_new_uint32(terrain.height_map->pixels[i]);

            color_map_pixel = shade_pixel(color_map_pixel, fog);

            float32_t height_on_screen =
                (camera.height - height_map_pixel.packed) * invz +
                camera.horizon;
            draw_vertical_line(draw_target, x, height_on_screen, ybuffer[x],
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
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Voxel Space";

    scg_app_t app;
    scg_app_init(&app, config);

    terrain_t terrain;
    camera_t camera;
    bool success = init(&terrain, &camera);
    if (!success) {
        return -1;
    }

    while (app.running) {
        scg_app_begin_frame(&app);

        float32_t delta_time = app.delta_time;

        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_UP)) {
            camera.x += -120.0f * sinf(camera.angle) * delta_time;
            camera.y += -120.0f * cosf(camera.angle) * delta_time;
        }
        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_DOWN)) {
            camera.x += 120.0f * sinf(camera.angle) * delta_time;
            camera.y += 120.0f * cosf(camera.angle) * delta_time;
        }
        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_LEFT)) {
            camera.angle += 1.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_RIGHT)) {
            camera.angle += -1.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_X)) {
            camera.height += -120.0f * delta_time;
        }
        if (scg_keyboard_is_key_down(app.keyboard, SCG_KEY_Z)) {
            camera.height += 120.0f * delta_time;
        }

        int map_w_period = terrain.map_w - 1;
        int map_h_period = terrain.map_h - 1;
        int map_offset = (((int)camera.y & map_w_period) << 10) +
                         ((int)camera.x & map_h_period);
        int map_height =
            scg_pixel_new_uint32(terrain.height_map->pixels[map_offset])
                .packed +
            10;
        if (map_height > camera.height) {
            camera.height = map_height;
        }

        draw(app.draw_target, terrain, camera);

        scg_app_end_frame(&app);
    }

    scg_image_free(terrain.height_map);
    scg_image_free(terrain.color_map);
    scg_app_shutdown(&app);

    return 0;
}
