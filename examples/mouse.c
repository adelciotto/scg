#define SCG_IMPLEMENTATION
#include "../scg.h"

int main(int arcg, char *argv[]) {
    scg_config_t config = scg_config_new_default();
    config.video.title = "SCG Example: Mouse";

    scg_app_t app;
    scg_app_init(&app, config);

    scg_image_t *cursor = scg_image_new_from_bmp("assets/mouse_cursor.bmp");
    if (cursor == NULL) {
        return -1;
    }

    while (scg_app_process_events(&app)) {
        scg_image_clear(app.draw_target, SCG_COLOR_BLUE);

        scg_image_t *draw_target = app.draw_target;
        scg_mouse_t *mouse = app.mouse;

        int w = draw_target->width;
        int h = draw_target->height;
        int half_w = w / 2;
        int half_h = h / 2;
        scg_pixel_t text_color = SCG_COLOR_WHITE;

        scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_NONE);
        scg_image_draw_string(draw_target, "Mouse State", half_w,
                              half_h - SCG_FONT_SIZE * 4, true, text_color);

        {
            char str[1024];
            scg_sprintf(str, "X: %d, Y: %d, Win X: %d, Win Y: %d", mouse->x,
                        mouse->y, mouse->window_x, mouse->window_y);
            scg_image_draw_string(app.draw_target, str, half_w, half_h, true,
                                  text_color);
        }

        {
            bool left_down =
                scg_mouse_is_button_down(mouse, SCG_MOUSE_BUTTON_LEFT);
            bool right_down =
                scg_mouse_is_button_down(mouse, SCG_MOUSE_BUTTON_RIGHT);
            bool middle_down =
                scg_mouse_is_button_down(mouse, SCG_MOUSE_BUTTON_MIDDLE);

            char str[1024];
            scg_sprintf(str,
                        "Left Button: %d, Right Button: %d, Middle Button: %d",
                        left_down, right_down, middle_down);
            scg_image_draw_string(draw_target, str, half_w,
                                  half_h + SCG_FONT_SIZE * 2, true, text_color);
        }

        scg_image_set_blend_mode(draw_target, SCG_BLEND_MODE_ALPHA);
        scg_image_draw_image(draw_target, cursor, mouse->x, mouse->y);

        scg_app_present(&app);
    }

    scg_app_free(&app);

    return 0;
}
