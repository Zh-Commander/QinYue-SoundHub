/* source/core/sgl_misc.c
 *
 * MIT License
 *
 * Copyright(c) 2023-present All contributors of SGL  
 * Document reference link: https://sgl-docs.readthedocs.io
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sgl.h>

#if (CONFIG_SGL_BOOT_LOGO)

typedef struct sgl_logo {
    sgl_obj_t       obj;
    uint8_t         alpha;
}sgl_logo_t;

#define SGL_BOOT_GLYPH_W       5
#define SGL_BOOT_GLYPH_H       7
#define SGL_BOOT_GLYPH_SPACE   1
#define SGL_BOOT_TEXT_MARGIN_X 16
#define SGL_BOOT_TEXT_MARGIN_Y 18
#define SGL_BOOT_LOGO_HOLD_MS  1000U

static const uint8_t sgl_boot_glyph_a[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F};
static const uint8_t sgl_boot_glyph_b[SGL_BOOT_GLYPH_H] = {0x10, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x1E};
static const uint8_t sgl_boot_glyph_c[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x0E, 0x10, 0x10, 0x10, 0x0E};
static const uint8_t sgl_boot_glyph_e[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E};
static const uint8_t sgl_boot_glyph_h[SGL_BOOT_GLYPH_H] = {0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x11};
static const uint8_t sgl_boot_glyph_i[SGL_BOOT_GLYPH_H] = {0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x0E};
static const uint8_t sgl_boot_glyph_l[SGL_BOOT_GLYPH_H] = {0x0C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
static const uint8_t sgl_boot_glyph_n[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11};
static const uint8_t sgl_boot_glyph_o[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E};
static const uint8_t sgl_boot_glyph_s[SGL_BOOT_GLYPH_H] = {0x00, 0x00, 0x0F, 0x10, 0x0E, 0x01, 0x1E};
static const uint8_t sgl_boot_glyph_t[SGL_BOOT_GLYPH_H] = {0x04, 0x04, 0x1F, 0x04, 0x04, 0x04, 0x03};
static const uint8_t sgl_boot_glyph_C[SGL_BOOT_GLYPH_H] = {0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F};
static const uint8_t sgl_boot_glyph_F[SGL_BOOT_GLYPH_H] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};

static const uint8_t *sgl_boot_get_glyph(char ch)
{
    switch(ch)
    {
    case 'C': return sgl_boot_glyph_C;
    case 'F': return sgl_boot_glyph_F;
    case 'a': return sgl_boot_glyph_a;
    case 'b': return sgl_boot_glyph_b;
    case 'c': return sgl_boot_glyph_c;
    case 'e': return sgl_boot_glyph_e;
    case 'h': return sgl_boot_glyph_h;
    case 'i': return sgl_boot_glyph_i;
    case 'l': return sgl_boot_glyph_l;
    case 'n': return sgl_boot_glyph_n;
    case 'o': return sgl_boot_glyph_o;
    case 's': return sgl_boot_glyph_s;
    case 't': return sgl_boot_glyph_t;
    default: return NULL;
    }
}

static int16_t sgl_boot_strlen(const char *text)
{
    int16_t len = 0;

    while((text != NULL) && (text[len] != '\0'))
    {
        len++;
    }

    return len;
}

static int16_t sgl_boot_get_text_width(const char *text, int16_t scale)
{
    int16_t len = sgl_boot_strlen(text);

    if(len <= 0)
    {
        return 0;
    }

    return (int16_t)(((len * SGL_BOOT_GLYPH_W) + ((len - 1) * SGL_BOOT_GLYPH_SPACE)) * scale);
}

static int16_t sgl_boot_get_fit_scale(sgl_area_t *area, const char *text)
{
    int16_t len = sgl_boot_strlen(text);
    int16_t area_w = (int16_t)(area->x2 - area->x1 + 1 - (SGL_BOOT_TEXT_MARGIN_X * 2));
    int16_t area_h = (int16_t)(area->y2 - area->y1 + 1 - (SGL_BOOT_TEXT_MARGIN_Y * 2));
    int16_t unit_w;
    int16_t scale_w;
    int16_t scale_h;
    int16_t scale;

    if((len <= 0) || (area_w <= 0) || (area_h <= 0))
    {
        return 1;
    }

    unit_w = (int16_t)((len * SGL_BOOT_GLYPH_W) + ((len - 1) * SGL_BOOT_GLYPH_SPACE));
    scale_w = (int16_t)(area_w / unit_w);
    scale_h = (int16_t)(area_h / SGL_BOOT_GLYPH_H);
    scale = sgl_min(scale_w, scale_h);

    return (scale > 0) ? scale : 1;
}

static void sgl_boot_draw_centered_text(sgl_surf_t *surf,
                                        sgl_area_t *clip,
                                        sgl_area_t *area,
                                        const char *text,
                                        sgl_color_t color,
                                        uint8_t alpha)
{
    int16_t scale = sgl_boot_get_fit_scale(area, text);
    int16_t text_w = sgl_boot_get_text_width(text, scale);
    int16_t text_h = (int16_t)(SGL_BOOT_GLYPH_H * scale);
    int16_t area_w = (int16_t)(area->x2 - area->x1 + 1);
    int16_t area_h = (int16_t)(area->y2 - area->y1 + 1);
    int16_t start_x = (int16_t)(area->x1 + ((area_w - text_w) / 2));
    int16_t start_y = (int16_t)(area->y1 + ((area_h - text_h) / 2));
    int16_t cursor_x = start_x;
    sgl_area_t dot;

    while((text != NULL) && (*text != '\0'))
    {
        const uint8_t *glyph = sgl_boot_get_glyph(*text);

        if(glyph != NULL)
        {
            uint8_t row;
            for(row = 0U; row < SGL_BOOT_GLYPH_H; row++)
            {
                uint8_t col;
                for(col = 0U; col < SGL_BOOT_GLYPH_W; col++)
                {
                    if((glyph[row] & (uint8_t)(1U << (SGL_BOOT_GLYPH_W - 1U - col))) != 0U)
                    {
                        dot.x1 = (int16_t)(cursor_x + ((int16_t)col * scale));
                        dot.y1 = (int16_t)(start_y + ((int16_t)row * scale));
                        dot.x2 = (int16_t)(dot.x1 + scale - 1);
                        dot.y2 = (int16_t)(dot.y1 + scale - 1);
                        sgl_draw_fill_rect(surf, clip, &dot, 0, color, alpha);
                    }
                }
            }
        }

        cursor_x = (int16_t)(cursor_x + ((SGL_BOOT_GLYPH_W + SGL_BOOT_GLYPH_SPACE) * scale));
        text++;
    }
}

static void sgl_logo_construct_cb(sgl_surf_t *surf, sgl_obj_t* obj, sgl_event_t *evt)
{
    sgl_logo_t *logo = (sgl_logo_t*)obj;

    if(evt->type == SGL_EVENT_DRAW_MAIN) {
        sgl_area_t clip;
        sgl_area_t top_area;
        sgl_area_t bottom_area;
        int16_t mid_y;

        if (!sgl_surf_clip(surf, &obj->area, &clip)) {
            return;
        }

        mid_y = (int16_t)(obj->area.y1 + ((obj->area.y2 - obj->area.y1 + 1) / 2));
        top_area = obj->area;
        top_area.y2 = (int16_t)(mid_y - 1);
        bottom_area = obj->area;
        bottom_area.y1 = mid_y;

        sgl_draw_fill_rect(surf, &clip, &top_area, 0, SGL_COLOR_WHITE, logo->alpha);
        sgl_draw_fill_rect(surf, &clip, &bottom_area, 0, SGL_COLOR_BLACK, logo->alpha);
        sgl_boot_draw_centered_text(surf, &clip, &top_area, "Chinese", SGL_COLOR_BLACK, logo->alpha);
        sgl_boot_draw_centered_text(surf, &clip, &bottom_area, "Football", SGL_COLOR_WHITE, logo->alpha);
    }
}

sgl_obj_t* sgl_logo_create(sgl_obj_t* parent)
{
    sgl_logo_t *logo = sgl_malloc(sizeof(sgl_logo_t));
    if (logo == NULL) {
        SGL_LOG_ERROR("sgl_logo_create: malloc failed");
        return NULL;
    }

    /* set object all member to zero */
    memset(logo, 0, sizeof(sgl_logo_t));

    sgl_obj_t *obj = &logo->obj;
    sgl_obj_init(&logo->obj, parent);
    obj->construct_fn = sgl_logo_construct_cb;
    logo->alpha = SGL_ALPHA_MAX;
    sgl_obj_set_border_width(obj, 0);
    return obj;
}

/**
 * @brief to show the sgl logo after sgl init
 * @param none
 * @return none
 * @note: you can call this function in your main function to show the sgl logo
 */
void sgl_boot_logo(void)
{
    sgl_obj_t *logo = sgl_logo_create(NULL);
    uint32_t start_tick;

    sgl_obj_set_size(logo, SGL_SCREEN_WIDTH, SGL_SCREEN_HEIGHT);
    sgl_obj_set_pos(logo, 0, 0);
    sgl_obj_set_radius(logo, 0);

    sgl_task_handler_sync();
    start_tick = sgl_tick_get();
    while((sgl_tick_get() - start_tick) < SGL_BOOT_LOGO_HOLD_MS)
    {
        sgl_task_handler();
    }

    sgl_obj_delete(logo);
}

#endif // !CONFIG_SGL_BOOT_LOGO
