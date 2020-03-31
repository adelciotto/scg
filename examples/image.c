#define SCG_IMPLEMENTATION
#include "../scg.h"

static void draw(scg_image_t *draw_target, scg_image_t *image) {
    int w = draw_target->width;
    int h = draw_target->height;
    int img_w = image->width;
    int img_h = image->height;
    int center_image_x = w / 2 - img_w / 2;
    int center_image_y = h / 2 - img_h / 2;
    int image_blend_none_x = center_image_x - img_w - 32;
    int image_blend_alpha_x = center_image_x;
    int image_blend_mask_x = center_image_x + img_w + 32;

    scg_pixel_t clear_color = SCG_COLOR_95_GREEN;
    scg_pixel_t text_color = SCG_COLOR_WHITE;

    scg_image_clear(draw_target, clear_color);

    scg_image_draw_string(draw_target, "NONE", image_blend_none_x + img_w / 2,
                          center_image_y + img_h + 16, 1, text_color);
    scg_image_draw_string(draw_target, "ALPHA", image_blend_alpha_x + img_w / 2,
                          center_image_y + img_h + 16, 1, text_color);
    scg_image_draw_string(draw_target, "MASK", image_blend_mask_x + img_w / 2,
                          center_image_y + img_h + 16, 1, text_color);

    scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_NONE);
    scg_image_draw_image(draw_target, image, image_blend_none_x,
                         center_image_y);

    scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_ALPHA);
    scg_image_draw_image(draw_target, image, image_blend_alpha_x,
                         center_image_y);

    scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_MASK);
    scg_image_draw_image(draw_target, image, image_blend_mask_x,
                         center_image_y);
}

int main(void) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Image";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *image = scg_image_new_from_bmp("assets/ball.bmp");
    if (image == NULL) {
        return -1;
    }

    while (scg_app_process_events(&app)) {
        draw(app.draw_target, image);

        scg_app_present(&app);
    }

    scg_image_free(image);
    scg_app_free(&app);

    return 0;
}
