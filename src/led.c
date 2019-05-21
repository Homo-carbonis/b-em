#include "b-em.h"
#include "led.h"
#include "video_render.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define BOX_WIDTH (64)
#define BOX_HEIGHT (32)
#define LED_REGION_HEIGHT (12)

int led_ticks = 0;

ALLEGRO_BITMAP *led_bitmap;

static ALLEGRO_FONT *font;

typedef struct {
    led_name_t led_name;
    const char *label;
    int index;
    int turn_off_at;
} led_details_t;

static led_details_t led_details[] = {
    {LED_CASSETTE_MOTOR, "cassette\nmotor", 0, 0},
    {LED_CAPS_LOCK, "caps\nlock", 1, 0},
    {LED_SHIFT_LOCK, "shift\nlock", 2, 0},
    {LED_DRIVE_0, "drive 0", 3, 0},
    {LED_DRIVE_1, "drive 1", 4, 0},
    {LED_VDFS, "VDFS", 5, 0} // SFTODO: MIGHT BE NICE TO HIDE VDFS LED IF VDFS DISABLED
};

static void draw_led(const led_details_t *led_details, bool b)
{
    const int x1 = led_details->index * BOX_WIDTH;
    const int y1 = 0;
    const int led_width = 16;
    const int led_height = 4;
    const int led_x1 = x1 + (BOX_WIDTH - led_width) / 2;
    const int led_y1 = y1 + (LED_REGION_HEIGHT - led_height) / 2;
    if (!led_bitmap)
        return; // SFTODO!?
    al_set_target_bitmap(led_bitmap);
    ALLEGRO_COLOR color = al_map_rgb(b ? 255 : 0, 0, 0);
    al_draw_filled_rectangle(led_x1, led_y1, led_x1 + led_width, led_y1 + led_height, color);
}

static void draw_led_full(const led_details_t *led_details, bool b)
{
    const ALLEGRO_COLOR label_colour = al_map_rgb(128, 128, 128);
    const int text_region_height = BOX_HEIGHT - LED_REGION_HEIGHT;
    const int x1 = led_details->index * BOX_WIDTH;
    const int y1 = 0;
    const char *label = led_details->label;
    if (!led_bitmap)
        return; // SFTODO!?
    al_set_target_bitmap(led_bitmap);
    al_draw_filled_rectangle(x1, y1, x1 + BOX_WIDTH - 1, y1 + BOX_HEIGHT - 1, al_map_rgb(64,64,64));
    draw_led(led_details, b);
    const char *label_newline = strchr(label, '\n');
    if (!label_newline) {
        const int text_height = al_get_font_ascent(font);
        const int text_y1 = y1 + LED_REGION_HEIGHT + (text_region_height - text_height) / 2;
        //al_draw_text(font, al_map_rgb(255, 255, 255), x1 + BOX_WIDTH / 2, y1 + LED_REGION_HEIGHT + text_region_height / 2, ALLEGRO_ALIGN_CENTRE, label);
        al_draw_text(font, label_colour, x1 + BOX_WIDTH / 2, text_y1, ALLEGRO_ALIGN_CENTRE, label);
    }
    else {
        char *label1 = malloc(label_newline - label + 1);
        memcpy(label1, label, label_newline - label);
        label1[label_newline - label] = '\0';
        const char *label2 = label_newline + 1;
        const int text_height = al_get_font_line_height(font);
        const int line_space = 2;
        const int text_y1 = y1 + LED_REGION_HEIGHT + (text_region_height - 2 * text_height - line_space) / 2;
        al_draw_text(font, label_colour, x1 + BOX_WIDTH / 2, text_y1, ALLEGRO_ALIGN_CENTRE, label1);
        al_draw_text(font, label_colour, x1 + BOX_WIDTH / 2, text_y1 + text_height + line_space, ALLEGRO_ALIGN_CENTRE, label2);
    }
}

void led_init()
{
    led_bitmap = al_create_bitmap(832, 32); // SFTODO!!!
    al_set_target_bitmap(led_bitmap);
    al_clear_to_color(al_map_rgb(0, 0, 64)); // sFTODO!?
    al_init_primitives_addon();
    al_init_font_addon();
    font = al_create_builtin_font();
    assert(font); // SFTODO: ERROR/DISABLE LEDS (??) IF FONT IS NULL - ASSERT IS TEMP HACK
    // SFTODO: MIGHT BE NICE TO HAVE SET OF LEDS VARY BY MACHINE, EG MASTER HAS
    // NO CASSETTE MOTOR LED
    // SFTODO THE FOLLOWING LOOP SEEMS TO HAVE NO EFFECT! - I THINK THE MAIN
    // CODE IS NOT DRAWING THE BITMAP UNTIL THE CURSOR MOVES OVER IT, THIS LOOP
    // ITSELF IS PROBABLY FINE
    for (int i = 0; i < sizeof(led_details)/sizeof(led_details[0]); i++)
        draw_led_full(&led_details[i], false);
    // SFTODO;
}

extern int framesrun; // SFTODO BIT OF A HACK
void led_update(led_name_t led_name, bool b, int ticks)
{
    // SFTODO: INEFFICIENT!
    for (int i = 0; i < sizeof(led_details)/sizeof(led_details[0]); i++) {
        if (led_details[i].led_name == led_name) {
            draw_led(&led_details[i], b);
            if (!b || (ticks == 0))
                led_details[i].turn_off_at = 0;
            else {
                led_details[i].turn_off_at = framesrun + ticks;
                if ((led_ticks == 0) || (ticks < led_ticks))
                    led_ticks = ticks;
            }
            return;
        }
    }
}

void led_timer_fired(void)
{
    for (int i = 0; i < sizeof(led_details)/sizeof(led_details[0]); i++) {
        if (led_details[i].turn_off_at != 0) {
            if (framesrun >= led_details[i].turn_off_at) {
                draw_led(&led_details[i], false);
                led_details[i].turn_off_at = 0;
            }
            else {
                int ticks = led_details[i].turn_off_at - framesrun;
                assert(ticks > 0); // SFTODO TEMP
                if ((led_ticks == 0) || (ticks < led_ticks))
                    led_ticks = ticks;
            }
        }
    }
}
