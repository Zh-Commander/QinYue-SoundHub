/* source/widgets/sgl_box.c
 *
 * MIT License
 *
 * Copyright(c) 2023-present All contributors of SGL  
 * Document reference link: docs directory
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

#include <sgl_core.h>
#include <sgl_draw.h>
#include <sgl_math.h>
#include <sgl_log.h>
#include <sgl_mm.h>
#include <sgl_cfgfix.h>
#include <sgl_theme.h>
#include "sgl_box.h"


#define  SGL_BOX_SCROLL_WIDTH                  (4)

static int16_t sgl_box_get_inner_clip_height(sgl_obj_t *obj, sgl_box_t *box)
{
    return (int16_t)(obj->coords.y2 - obj->coords.y1 + 1 - 2 * box->bg.radius);
}

static bool sgl_box_get_inner_clip_rect(sgl_obj_t *obj, sgl_box_t *box, sgl_area_t *clip)
{
    if((obj == NULL) || (box == NULL) || (clip == NULL))
    {
        return false;
    }

    *clip = sgl_obj_get_fill_rect(obj);
    clip->x1 += box->bg.radius;
    clip->y1 += box->bg.radius;
    clip->x2 -= box->bg.radius;
    clip->y2 -= box->bg.radius;

    return (clip->x1 <= clip->x2) && (clip->y1 <= clip->y2);
}

static void sgl_box_update_scroll_exposed_area(sgl_obj_t *obj, sgl_box_t *box, int16_t ofs_x, int16_t ofs_y)
{
    sgl_area_t clip;
    sgl_area_t area;

    if(!sgl_box_get_inner_clip_rect(obj, box, &clip))
    {
        return;
    }

    if(ofs_y != 0)
    {
        area = clip;
        if(ofs_y > 0)
        {
            area.y1 = clip.y1;
            area.y2 = sgl_min((int16_t)(clip.y1 + ofs_y - 1), clip.y2);
        }
        else
        {
            area.y1 = sgl_max((int16_t)(clip.y2 + ofs_y + 1), clip.y1);
            area.y2 = clip.y2;
        }

        if(area.y1 <= area.y2)
        {
            sgl_obj_update_area(&area);
        }
    }

    if(ofs_x != 0)
    {
        area = clip;
        if(ofs_x > 0)
        {
            area.x1 = clip.x1;
            area.x2 = sgl_min((int16_t)(clip.x1 + ofs_x - 1), clip.x2);
        }
        else
        {
            area.x1 = sgl_max((int16_t)(clip.x2 + ofs_x + 1), clip.x1);
            area.x2 = clip.x2;
        }

        if(area.x1 <= area.x2)
        {
            sgl_obj_update_area(&area);
        }
    }
}


static void sgl_box_move_child_pos_silent(sgl_obj_t *obj, int16_t ofs_x, int16_t ofs_y)
{
    sgl_obj_t *stack[SGL_OBJ_DEPTH_MAX];
    sgl_obj_t *child;
    sgl_area_t viewport_clip;
    int top = 0;
    bool has_viewport_clip;

    if((obj == NULL) || (obj->child == NULL) || ((ofs_x == 0) && (ofs_y == 0)))
    {
        return;
    }

    has_viewport_clip = sgl_box_get_inner_clip_rect(obj, sgl_container_of(obj, sgl_box_t, obj), &viewport_clip);
    stack[top++] = obj->child;
    while(top > 0)
    {
        sgl_area_t fill_area;

        SGL_ASSERT(top < SGL_OBJ_DEPTH_MAX);
        child = stack[--top];

        child->coords.x1 += ofs_x;
        child->coords.x2 += ofs_x;
        child->coords.y1 += ofs_y;
        child->coords.y2 += ofs_y;

        if(child->parent == obj)
        {
            if(!has_viewport_clip)
            {
                sgl_area_init(&child->area);
            }
            else if(!sgl_area_clip(&viewport_clip, &child->coords, &child->area))
            {
                sgl_area_init(&child->area);
            }
        }
        else
        {
            fill_area = sgl_obj_get_fill_rect(child->parent);
            if(!sgl_area_clip(&fill_area, &child->coords, &child->area))
            {
                sgl_area_init(&child->area);
            }
        }

        if(child->sibling != NULL)
        {
            stack[top++] = child->sibling;
        }

        if(child->child != NULL)
        {
            stack[top++] = child->child;
        }
    }
}


static bool sgl_box_get_v_scrollbar_area(sgl_obj_t *obj, sgl_box_t *box, int16_t v_scroll_limit, int16_t y_offset, sgl_area_t *area)
{
    int16_t height;
    int16_t scroll_height;
    int16_t max_scroll_pos;
    float scroll_ratio;

    if((obj == NULL) || (box == NULL) || (area == NULL))
    {
        return false;
    }

    if((box->scroll_enable == 0) || (box->show_v_scrollbar == 0))
    {
        return false;
    }

    if(((box->scroll_mode & SGL_BOX_SCROLL_VERTICAL_ONLY) == 0) && ((box->scroll_mode & SGL_BOX_SCROLL_BOTH) == 0))
    {
        return false;
    }

    height = sgl_box_get_inner_clip_height(obj, box);
    scroll_height = sgl_max(height / 8, SGL_BOX_SCROLL_WIDTH);

    area->x1 = obj->coords.x2 - SGL_BOX_SCROLL_WIDTH - box->bg.radius;
    area->y1 = obj->coords.y1 + box->bg.radius;
    area->x2 = obj->coords.x2 - box->bg.radius;

    if(v_scroll_limit > 0)
    {
        scroll_ratio = (float)(-y_offset) / v_scroll_limit;
        scroll_ratio = sgl_clamp(scroll_ratio, 0.0f, 1.0f);
        max_scroll_pos = height - scroll_height;
        area->y1 = obj->coords.y1 + box->bg.radius + (int16_t)(max_scroll_pos * scroll_ratio);
        area->y2 = area->y1 + scroll_height - 1;
    }
    else
    {
        area->y2 = area->y1 + scroll_height - 1;
    }

    return true;
}



static void sgl_box_update_scrollbar_area(sgl_obj_t *obj, sgl_box_t *box, int16_t v_scroll_limit, int16_t h_scroll_limit)
{
    sgl_area_t area;

    if(sgl_box_get_v_scrollbar_area(obj, box, v_scroll_limit, box->y_offset, &area))
    {
        sgl_obj_update_area(&area);
    }

    (void)h_scroll_limit;
}


static bool sgl_box_get_content_rect(sgl_obj_t *obj, sgl_rect_t *content_rect)
{
    sgl_obj_t *child;
    bool has_child = false;

    SGL_ASSERT(obj != NULL);
    SGL_ASSERT(content_rect != NULL);

    for(child = obj->child; child != NULL; child = child->sibling) {
        if (unlikely(sgl_obj_is_hidden(child))) {
            continue;
        }

        if (!has_child) {
            *content_rect = child->coords;
            has_child = true;
        }
        else {
            content_rect->x1 = sgl_min(content_rect->x1, child->coords.x1);
            content_rect->y1 = sgl_min(content_rect->y1, child->coords.y1);
            content_rect->x2 = sgl_max(content_rect->x2, child->coords.x2);
            content_rect->y2 = sgl_max(content_rect->y2, child->coords.y2);
        }
    }

    if (!has_child) {
        content_rect->x1 = obj->coords.x1;
        content_rect->y1 = obj->coords.y1;
        content_rect->x2 = obj->coords.x2;
        content_rect->y2 = obj->coords.y2;
    }

    return has_child;
}

static int16_t sgl_box_get_v_scroll_limit(sgl_obj_t *obj, sgl_box_t *box)
{
    sgl_rect_t content_rect;
    int16_t viewport_bottom;
    int16_t content_bottom;

    if (!sgl_box_get_content_rect(obj, &content_rect)) {
        return 0;
    }

    viewport_bottom = obj->coords.y2 - box->bg.radius;
    content_bottom = content_rect.y2 - box->y_offset;
    return sgl_max((int16_t)(content_bottom - viewport_bottom), 0);
}

static void sgl_box_center_children_horizontally(sgl_obj_t *obj, sgl_box_t *box)
{
    sgl_area_t clip;
    sgl_rect_t content_rect;
    int16_t clip_width;
    int16_t content_width;
    int16_t target_x1;
    int16_t delta_x;

    if((obj == NULL) || (box == NULL))
    {
        return;
    }

    if(!sgl_box_get_inner_clip_rect(obj, box, &clip))
    {
        return;
    }

    if(!sgl_box_get_content_rect(obj, &content_rect))
    {
        return;
    }

    clip_width = (int16_t)(clip.x2 - clip.x1 + 1);
    content_width = (int16_t)(content_rect.x2 - content_rect.x1 + 1);
    if((clip_width <= 0) || (content_width <= 0))
    {
        return;
    }

    if(content_width < clip_width)
    {
        target_x1 = (int16_t)(clip.x1 + (clip_width - content_width) / 2);
    }
    else
    {
        target_x1 = clip.x1;
    }

    delta_x = (int16_t)(target_x1 - content_rect.x1);
    if(delta_x != 0)
    {
        sgl_box_move_child_pos_silent(obj, delta_x, 0);
    }

    box->x_offset = 0;
}

static void sgl_box_construct_cb(sgl_surf_t *surf, sgl_obj_t* obj, sgl_event_t *evt)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    int16_t height = sgl_box_get_inner_clip_height(obj, box);
    int16_t scroll_height = sgl_max(height / 8, SGL_BOX_SCROLL_WIDTH);
    int16_t v_scroll_limit = sgl_box_get_v_scroll_limit(obj, box);
    sgl_rect_t area;

    if(evt->type == SGL_EVENT_DRAW_MAIN) {
        sgl_box_center_children_horizontally(obj, box);
        area.x1 = obj->coords.x1 + box->bg.radius;
        area.y1 = obj->coords.y1 + box->bg.radius;
        area.x2 = obj->coords.x2 - box->bg.radius;
        area.y2 = obj->coords.y2 - box->bg.radius;

        sgl_draw_rect(surf, &obj->area, &obj->coords, &box->bg);

        if(box->scroll_enable) {
            if (((box->scroll_mode & SGL_BOX_SCROLL_VERTICAL_ONLY) || (box->scroll_mode & SGL_BOX_SCROLL_BOTH))
                && box->show_v_scrollbar) {
                area.x1 = obj->coords.x2 - SGL_BOX_SCROLL_WIDTH - box->bg.radius;
                area.y1 = obj->coords.y1 + box->bg.radius;
                area.x2 = obj->coords.x2 - box->bg.radius;

                if(v_scroll_limit > 0) {
                    float scroll_ratio = (float)(-box->y_offset) / v_scroll_limit;
                    scroll_ratio = sgl_clamp(scroll_ratio, 0.0f, 1.0f);

                    int16_t max_scroll_pos = height - scroll_height;
                    area.y1 = obj->coords.y1 + box->bg.radius + (int16_t)(max_scroll_pos * scroll_ratio);
                    area.y2 = area.y1 + scroll_height - 1;
                }
                else {
                    area.y2 = area.y1 + scroll_height - 1;
                }

                sgl_draw_fill_rect(surf, &obj->area, &area, SGL_BOX_SCROLL_WIDTH / 2, box->scroll_color, 128);
            }
        }
    }
    else if(evt->type == SGL_EVENT_MOVE_UP || evt->type == SGL_EVENT_MOVE_DOWN) {
        if((box->scroll_mode & SGL_BOX_SCROLL_VERTICAL_ONLY) || (box->scroll_mode & SGL_BOX_SCROLL_BOTH)) {
            int16_t distance = evt->type == SGL_EVENT_MOVE_UP ? -evt->distance : evt->distance;
            int16_t old_offset = box->y_offset;
            int16_t new_offset = old_offset + distance;
            int16_t min_elastic = -(v_scroll_limit + box->elastic_scroll_down);
            int16_t max_elastic = box->elastic_scroll_up;
            int16_t constrained_new_offset = sgl_clamp(new_offset, min_elastic, max_elastic);
            int16_t offset_delta = constrained_new_offset - old_offset;

            if(offset_delta != 0) {
                box->y_offset = constrained_new_offset;
                sgl_box_move_child_pos_silent(obj, 0, offset_delta);
                sgl_obj_set_dirty(obj);
                sgl_box_update_scroll_exposed_area(obj, box, 0, offset_delta);
                sgl_box_update_scrollbar_area(obj, box, v_scroll_limit, 0);
            }
        }
    }
    else if (evt->type == SGL_EVENT_PRESSED) {
        if(box->scroll_enable == 0) {
            box->scroll_enable = 1;
            sgl_box_update_scrollbar_area(obj, box, v_scroll_limit, 0);
        }
    }
    else if (evt->type == SGL_EVENT_RELEASED) {
        if(box->scroll_enable != 0) {
            sgl_box_update_scrollbar_area(obj, box, v_scroll_limit, 0);
            box->scroll_enable = 0;
        }
    }
}


/**
 * @brief create a box object
 * @param parent parent of the box
 * @return pointer to the box object
 */
sgl_obj_t* sgl_box_create(sgl_obj_t* parent)
{
    sgl_box_t *box = sgl_malloc(sizeof(sgl_box_t));
    if(box == NULL) {
        SGL_LOG_ERROR("sgl_box_create: malloc failed");
        return NULL;
    }

    /* set object all member to zero */
    memset(box, 0, sizeof(sgl_box_t));

    sgl_obj_t *obj = &box->obj;
    sgl_obj_init(&box->obj, parent);
    obj->construct_fn = sgl_box_construct_cb;
    sgl_obj_set_border_width(obj, SGL_THEME_BORDER_WIDTH);
    obj->focus = 1;

    sgl_obj_set_clickable(obj);
    sgl_obj_set_movable(obj);

    box->bg.alpha = SGL_THEME_ALPHA;
    box->bg.color = SGL_THEME_COLOR;
    box->bg.radius = 10;
    box->bg.border = 1;
    sgl_obj_set_border_width(obj, 1);
    box->bg.border_color = SGL_THEME_BORDER_COLOR;
    box->scroll_color = SGL_THEME_SCROLL_FG_COLOR;

    box->x_offset = 0;
    box->y_offset = 0;
    box->scroll_enable = 0;
    box->show_v_scrollbar = 1;
    box->show_h_scrollbar = 0;
    box->scroll_mode = SGL_BOX_SCROLL_VERTICAL_ONLY;
    box->elastic_scroll_up = 0;    // Default: unlimited scrolling up
    box->elastic_scroll_down = 0;  // Default: unlimited scrolling down
    box->elastic_scroll_left = 0;  // Default: unlimited scrolling left
    box->elastic_scroll_right = 0; // Default: unlimited scrolling right

    return obj;
}


/**
 * @brief set background color of the box
 * @param obj box object
 * @param color background color to be set
 * @return none
 */
void sgl_box_set_bg_color(sgl_obj_t *obj, sgl_color_t color)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->bg.color = color;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set radius of the box
 * @param obj box object
 * @param radius radius to be set
 * @return none
 */
void sgl_box_set_radius(sgl_obj_t *obj, uint8_t radius)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    sgl_obj_set_radius(obj, radius);
    box->bg.radius = obj->radius;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set border color of the box
 * @param obj box object
 * @param color border color to be set
 * @return none
 */
void sgl_box_set_border_color(sgl_obj_t *obj, sgl_color_t color)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->bg.border_color = color;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set border width of the box
 * @param obj box object
 * @param width border width to be set
 * @return none
 */
void sgl_box_set_border_width(sgl_obj_t *obj, uint8_t width)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->bg.border = width;
    sgl_obj_set_border_width(obj, width);
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set box pixmap
 * @param obj box object
 * @param pixmap pixmap to be set
 * @return none
 */
void sgl_box_set_pixmap(sgl_obj_t *obj, const sgl_pixmap_t *pixmap)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->bg.pixmap = pixmap;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set scrollbar color of the box
 * @param obj box object
 * @param color scrollbar color to be set
 * @return none
 */
void sgl_box_set_scrollbar_color(sgl_obj_t *obj, sgl_color_t color)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->scroll_color = color;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set whether to show scrollbars
 * @param obj box object
 * @param show_vertical whether to show vertical scrollbar (1 to show, 0 to hide)
 * @param show_horizontal whether to show horizontal scrollbar (1 to show, 0 to hide)
 * @return none
 */
void sgl_box_set_show_scrollbar(sgl_obj_t *obj, uint8_t show_vertical, uint8_t show_horizontal)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->show_v_scrollbar = show_vertical;
    box->show_h_scrollbar = 0;
    (void)show_horizontal;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief set alpha (transparency) of the box
 * @param obj box object
 * @param alpha alpha value to be set (0-255, where 0 is fully transparent and 255 is fully opaque)
 * @return none
 */
void sgl_box_set_alpha(sgl_obj_t *obj, uint8_t alpha)
{
    sgl_box_t *box = sgl_container_of(obj, sgl_box_t, obj);
    box->bg.alpha = alpha;
    sgl_obj_set_dirty(obj);
}

/**
 * @brief Set the elastic scroll limits for up and down directions
 * @param obj box object
 * @param up_limit maximum pixels allowed when scrolling up (0 for unlimited)
 * @param down_limit maximum pixels allowed when scrolling down (0 for unlimited)
 * @param left_limit maximum pixels allowed when scrolling left (0 for unlimited)
 * @param right_limit maximum pixels allowed when scrolling right (0 for unlimited)
 * @return none
 */
void sgl_box_set_elastic_scroll(sgl_obj_t* obj, int16_t up_limit, int16_t down_limit, int16_t left_limit, int16_t right_limit)
{
    SGL_ASSERT(obj != NULL);
    sgl_box_t *box = (sgl_box_t*)obj;
    box->elastic_scroll_up = up_limit;
    box->elastic_scroll_down = down_limit;
    box->elastic_scroll_left = 0;
    box->elastic_scroll_right = 0;
    (void)left_limit;
    (void)right_limit;
}

void sgl_box_set_scroll_offset(sgl_obj_t *obj, int16_t x_offset, int16_t y_offset)
{
    sgl_box_t *box;
    int16_t delta_y;
    int16_t v_scroll_limit;

    SGL_ASSERT(obj != NULL);
    box = sgl_container_of(obj, sgl_box_t, obj);

    x_offset = 0;
    delta_y = y_offset - box->y_offset;
    if(delta_y == 0)
    {
        box->x_offset = 0;
        return;
    }

    v_scroll_limit = sgl_box_get_v_scroll_limit(obj, box);

    box->x_offset = 0;
    box->y_offset = y_offset;
    sgl_box_move_child_pos_silent(obj, 0, delta_y);
    sgl_box_center_children_horizontally(obj, box);
    sgl_obj_set_dirty(obj);
    sgl_box_update_scroll_exposed_area(obj, box, 0, delta_y);
    sgl_box_update_scrollbar_area(obj, box, v_scroll_limit, 0);
    (void)x_offset;
}
