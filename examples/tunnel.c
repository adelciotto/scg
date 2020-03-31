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

static void init(scg_app_t *app, tunnel_t *tunnel, scg_image_t *src_image) {
    int w = app->draw_target->width;
    int h = app->draw_target->height;

    uint32_t *distance_buffer = malloc(w * h * sizeof(*distance_buffer));
    uint32_t *angle_buffer = malloc(w * h * sizeof(*angle_buffer));
    float32_t *shade_buffer = malloc(w * h * sizeof(*shade_buffer));

    int image_w = src_image->width;
    int image_h = src_image->height;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = scg_pixel_index_from_xy(x, y, w);
            float32_t dx = x - w / 2;
            float32_t dy = y - h / 2;
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

static void draw(scg_image_t *draw_target, tunnel_t *tunnel,
                 float32_t elapsed_time) {
    int w = draw_target->width;
    int h = draw_target->height;
    int image_w = tunnel->src_image->width;
    int image_h = tunnel->src_image->height;
    int shift_x = (int)floorf((float32_t)image_w * elapsed_time * 0.5f);
    int shift_y = (int)floorf((float32_t)image_h * elapsed_time * 0.25f);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int dest_i = scg_pixel_index_from_xy(j, i, w);
            int x = (tunnel->distance_buffer[dest_i] + shift_x) % image_w;
            int y = (tunnel->angle_buffer[dest_i] + shift_y) % image_h;

            int src_i = scg_pixel_index_from_xy(x, y, image_w);
            scg_pixel_t color =
                scg_pixel_new_uint32(tunnel->src_image->pixels[src_i]);

            float32_t shade = tunnel->shade_buffer[dest_i];
            color.data.r = (uint8_t)((float32_t)color.data.r * shade);
            color.data.g = (uint8_t)((float32_t)color.data.g * shade);
            color.data.b = (uint8_t)((float32_t)color.data.b * shade);

            draw_target->pixels[dest_i] = color.packed;
        }
    }
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Tunnel";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *image = scg_image_new_from_bmp("assets/space.bmp");
    if (image == NULL) {
        return -1;
    }

    tunnel_t tunnel;
    init(&app, &tunnel, image);

    uint64_t start_time = scg_get_performance_counter();

    while (scg_app_process_events(&app)) {
        float32_t elapsed_time = scg_get_elapsed_time_secs(
            scg_get_performance_counter(), start_time);
        draw(app.draw_target, &tunnel, elapsed_time);

        scg_app_present(&app);
    }

    free(tunnel.shade_buffer);
    free(tunnel.angle_buffer);
    free(tunnel.distance_buffer);

    scg_image_free(image);
    scg_app_free(&app);

    return 0;
}
