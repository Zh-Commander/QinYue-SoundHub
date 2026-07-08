#include "demo_sliding_menu.h"

#include <string.h>

#define DEMO_SLIDING_MENU_ITEM_STEP          12
#define DEMO_SLIDING_MENU_DRAG_THRESHOLD     12
#define DEMO_SLIDING_MENU_ANIM_DURATION_MS   140
#define DEMO_SLIDING_MENU_LABEL_HEIGHT       14
#define DEMO_SLIDING_MENU_MAX_VISIBLE_DIST   2.5f
#define DEMO_SLIDING_MENU_BG_COLOR           sgl_rgb(255, 255, 255)
#define DEMO_SLIDING_MENU_LABEL_COLOR        sgl_rgb(51, 65, 85)

typedef struct demo_sliding_menu_item_ctx {
    struct demo_sliding_menu *menu;
    uint8_t                  index;
} demo_sliding_menu_item_ctx_t;

struct demo_sliding_menu {
    sgl_obj_t                        *root;
    sgl_obj_t                        *label;
    sgl_obj_t                       **items;
    demo_sliding_menu_item_ctx_t     *item_ctx;
    char                            **texts;
    sgl_color_t                      *colors;
    sgl_anim_t                       *anim;
    demo_sliding_menu_click_cb_t      click_cb;
    const sgl_font_t                 *font;
    int16_t                           width;
    int16_t                           height;
    int16_t                           item_width_small;
    int16_t                           item_height_small;
    int16_t                           item_width_large;
    int16_t                           item_height_large;
    int16_t                           item_offset_x;
    int16_t                           item_offset_y;
    int16_t                           drag_dx;
    int16_t                           drag_dy;
    int16_t                           touch_start_x;
    int16_t                           touch_start_y;
    uint8_t                           touch_active;
    int8_t                            current_index;
    int8_t                            target_index;
    uint8_t                           item_count;
    uint8_t                           is_animating;
    demo_sliding_menu_direction_t     direction;
};

static void DemoSlidingMenu_UpdateLayout(demo_sliding_menu_t *menu, float progress);
static void DemoSlidingMenu_MoveToIndex(demo_sliding_menu_t *menu, int next_index);

static float DemoSlidingMenu_FloatAbs(float value)
{
    return value >= 0.0f ? value : -value;
}

static void DemoSlidingMenu_CompleteAnimation(demo_sliding_menu_t *menu)
{
    if((menu == NULL) || (menu->is_animating == 0U))
    {
        return;
    }

    if(menu->anim != NULL)
    {
        sgl_anim_stop(menu->anim);
    }

    menu->current_index = menu->target_index;
    menu->is_animating = 0U;
    DemoSlidingMenu_UpdateLayout(menu, 1.0f);
}



static void DemoSlidingMenu_AnimPath(sgl_anim_t *anim, int32_t value)
{
    demo_sliding_menu_t *menu;
    float progress;

    if(anim == NULL)
    {
        return;
    }

    menu = (demo_sliding_menu_t *)anim->data;
    if(menu == NULL)
    {
        return;
    }

    progress = (float)value / 1000.0f;
    DemoSlidingMenu_UpdateLayout(menu, progress);
}

static void DemoSlidingMenu_AnimFinished(sgl_anim_t *anim)
{
    demo_sliding_menu_t *menu;

    if(anim == NULL)
    {
        return;
    }

    menu = (demo_sliding_menu_t *)anim->data;
    if(menu == NULL)
    {
        return;
    }

    menu->current_index = menu->target_index;
    menu->is_animating = 0U;
    DemoSlidingMenu_UpdateLayout(menu, 1.0f);
}

static void DemoSlidingMenu_OnItemEvent(sgl_event_t *event)
{
    demo_sliding_menu_item_ctx_t *ctx;
    demo_sliding_menu_t *menu;
    int actual_index;

    if((event == NULL) || (event->param == NULL))
    {
        return;
    }

    ctx = (demo_sliding_menu_item_ctx_t *)event->param;
    menu = ctx->menu;
    if(menu == NULL)
    {
        return;
    }

    actual_index = menu->is_animating ? menu->target_index : menu->current_index;

    switch(event->type)
    {
        case SGL_EVENT_PRESSED:
            DemoSlidingMenu_CompleteAnimation(menu);
            menu->drag_dx = 0;
            menu->drag_dy = 0;
            break;

        case SGL_EVENT_MOVE_LEFT:
            if(menu->is_animating == 0U)
            {
                menu->drag_dx -= (int16_t)event->distance;
            }
            break;

        case SGL_EVENT_MOVE_RIGHT:
            if(menu->is_animating == 0U)
            {
                menu->drag_dx += (int16_t)event->distance;
            }
            break;

        case SGL_EVENT_MOVE_UP:
            if(menu->is_animating == 0U)
            {
                menu->drag_dy -= (int16_t)event->distance;
            }
            break;

        case SGL_EVENT_MOVE_DOWN:
            if(menu->is_animating == 0U)
            {
                menu->drag_dy += (int16_t)event->distance;
            }
            break;

        case SGL_EVENT_RELEASED:
            if(menu->is_animating != 0U)
            {
                DemoSlidingMenu_CompleteAnimation(menu);
                return;
            }

            if(menu->direction == DEMO_SLIDING_MENU_VERTICAL)
            {
                if((sgl_abs(menu->drag_dy) >= DEMO_SLIDING_MENU_DRAG_THRESHOLD)
                   && (sgl_abs(menu->drag_dy) > sgl_abs(menu->drag_dx)))
                {
                    if(menu->drag_dy < 0)
                    {
                        DemoSlidingMenu_MoveDown(menu);
                    }
                    else
                    {
                        DemoSlidingMenu_MoveUp(menu);
                    }
                }
                else if((int)ctx->index == actual_index)
                {
                    if(menu->click_cb != NULL)
                    {
                        menu->click_cb(actual_index, menu->texts[actual_index]);
                    }
                }
            }
            else
            {
                if((sgl_abs(menu->drag_dx) >= DEMO_SLIDING_MENU_DRAG_THRESHOLD)
                   && (sgl_abs(menu->drag_dx) > sgl_abs(menu->drag_dy)))
                {
                    if(menu->drag_dx < 0)
                    {
                        DemoSlidingMenu_MoveRight(menu);
                    }
                    else
                    {
                        DemoSlidingMenu_MoveLeft(menu);
                    }
                }
                else if((int)ctx->index == actual_index)
                {
                    if(menu->click_cb != NULL)
                    {
                        menu->click_cb(actual_index, menu->texts[actual_index]);
                    }
                }
            }

            menu->drag_dx = 0;
            menu->drag_dy = 0;
            break;

        default:
            break;
    }
}

static void DemoSlidingMenu_UpdateLayout(demo_sliding_menu_t *menu, float progress)
{
    float center_start;
    float center_end;
    float center_pos;
    float best_distance = 9999.0f;
    int center_x;
    int center_y;
    uint8_t i;

    if((menu == NULL) || (menu->item_count == 0U))
    {
        return;
    }

    sgl_obj_set_dirty(menu->root);

    if(progress < 0.0f)
    {
        progress = 0.0f;
    }
    else if(progress > 1.0f)
    {
        progress = 1.0f;
    }

    center_start = (float)menu->current_index;
    center_end = (float)menu->target_index;

    if(menu->item_count > 1U)
    {
        float diff = center_end - center_start;
        if(diff > ((float)menu->item_count / 2.0f))
        {
            center_end -= (float)menu->item_count;
        }
        else if(diff < -((float)menu->item_count / 2.0f))
        {
            center_end += (float)menu->item_count;
        }
    }

    center_pos = center_start + (center_end - center_start) * progress;
    center_x = menu->width / 2 + menu->item_offset_x;
    if(menu->direction == DEMO_SLIDING_MENU_VERTICAL)
    {
        center_y = (menu->height - DEMO_SLIDING_MENU_LABEL_HEIGHT) / 2 + menu->item_offset_y;
    }
    else
    {
        center_y = menu->height / 2 + menu->item_offset_y;
    }

    for(i = 0U; i < menu->item_count; i++)
    {
        float rel = (float)i - center_pos;
        float abs_rel;
        int16_t width;
        int16_t height;
        int16_t x;
        int16_t y;

        if(menu->item_count > 1U)
        {
            while(rel > ((float)menu->item_count / 2.0f))
            {
                rel -= (float)menu->item_count;
            }
            while(rel < -((float)menu->item_count / 2.0f))
            {
                rel += (float)menu->item_count;
            }
        }

        abs_rel = DemoSlidingMenu_FloatAbs(rel);

        if(abs_rel > DEMO_SLIDING_MENU_MAX_VISIBLE_DIST)
        {
            sgl_obj_set_hidden(menu->items[i]);
            sgl_obj_set_size(menu->items[i], 0, 0);
            continue;
        }

        if(abs_rel < best_distance)
        {
            best_distance = abs_rel;

        }

        if(abs_rel <= 0.5f)
        {
            float t = 1.0f - abs_rel / 0.5f;
            width = (int16_t)(menu->item_width_small + (menu->item_width_large - menu->item_width_small) * (0.5f + 0.5f * t));
            height = (int16_t)(menu->item_height_small + (menu->item_height_large - menu->item_height_small) * (0.5f + 0.5f * t));
        }
        else if(abs_rel <= 1.5f)
        {
            float t = 1.0f - (abs_rel - 0.5f) / 1.0f;
            width = (int16_t)(menu->item_width_small + (menu->item_width_large - menu->item_width_small) * 0.5f * t);
            height = (int16_t)(menu->item_height_small + (menu->item_height_large - menu->item_height_small) * 0.5f * t);
        }
        else
        {
            width = menu->item_width_small;
            height = menu->item_height_small;
        }

        sgl_obj_set_visible(menu->items[i]);
        if(menu->direction == DEMO_SLIDING_MENU_VERTICAL)
        {
            x = (int16_t)(center_x - width / 2);
            y = (int16_t)(center_y + rel * (menu->item_height_small + DEMO_SLIDING_MENU_ITEM_STEP) - height / 2);
        }
        else
        {
            x = (int16_t)(center_x + rel * (menu->item_width_small + DEMO_SLIDING_MENU_ITEM_STEP) - width / 2);
            y = (int16_t)(center_y - height / 2);
        }

        sgl_obj_set_pos(menu->items[i], x, y);
        sgl_obj_set_size(menu->items[i], width, height);
    }

    sgl_obj_set_dirty(menu->root);
}

static void DemoSlidingMenu_MoveToIndex(demo_sliding_menu_t *menu, int next_index)
{
    if((menu == NULL) || (menu->item_count <= 1U) || (menu->is_animating != 0U))
    {
        return;
    }

    if(next_index < 0)
    {
        next_index = menu->item_count - 1;
    }
    else if(next_index >= menu->item_count)
    {
        next_index = 0;
    }

    menu->target_index = (int8_t)next_index;
    menu->is_animating = 1U;

    if(menu->anim != NULL)
    {
        sgl_anim_set_start_value(menu->anim, 0);
        sgl_anim_set_end_value(menu->anim, 1000);
        sgl_anim_start(menu->anim, SGL_ANIM_REPEAT_ONCE);
    }
    else
    {
        menu->current_index = menu->target_index;
        menu->is_animating = 0U;
        DemoSlidingMenu_UpdateLayout(menu, 1.0f);
    }
}

demo_sliding_menu_t *DemoSlidingMenu_Create(sgl_obj_t *parent,
                                            int16_t x,
                                            int16_t y,
                                            int16_t width,
                                            int16_t height,
                                            const char * const *texts,
                                            uint8_t item_count,
                                            demo_sliding_menu_direction_t direction,
                                            const sgl_font_t *font,
                                            demo_sliding_menu_click_cb_t click_cb)
{
    demo_sliding_menu_t *menu;
    uint8_t i;

    if((parent == NULL) || (texts == NULL) || (item_count == 0U) || (font == NULL))
    {
        return NULL;
    }

    menu = (demo_sliding_menu_t *)sgl_malloc(sizeof(demo_sliding_menu_t));
    if(menu == NULL)
    {
        return NULL;
    }
    memset(menu, 0, sizeof(*menu));

    menu->width = width;
    menu->height = height;
    menu->item_count = item_count;
    menu->direction = direction;
    menu->font = font;
    menu->click_cb = click_cb;

    if(direction == DEMO_SLIDING_MENU_VERTICAL)
    {
        menu->item_width_small = width - 72;
        menu->item_height_small = 32;
        menu->item_width_large = width - 40;
        menu->item_height_large = 46;
        menu->item_offset_x = 0;
        menu->item_offset_y = -10;
    }
    else
    {
        menu->item_width_small = 28;
        menu->item_height_small = 28;
        menu->item_width_large = 42;
        menu->item_height_large = 42;
        menu->item_offset_x = 0;
        menu->item_offset_y = 0;
    }

    menu->items = (sgl_obj_t **)sgl_malloc(sizeof(sgl_obj_t *) * item_count);
    menu->item_ctx = (demo_sliding_menu_item_ctx_t *)sgl_malloc(sizeof(demo_sliding_menu_item_ctx_t) * item_count);
    menu->texts = (char **)sgl_malloc(sizeof(char *) * item_count);
    menu->colors = (sgl_color_t *)sgl_malloc(sizeof(sgl_color_t) * item_count);
    if((menu->items == NULL) || (menu->item_ctx == NULL) || (menu->texts == NULL) || (menu->colors == NULL))
    {
        DemoSlidingMenu_Delete(menu);
        return NULL;
    }

    for(i = 0U; i < item_count; i++)
    {
        menu->texts[i] = NULL;
        menu->colors[i] = sgl_rgb((uint8_t)(70U + i * 24U),
                                  (uint8_t)(110U + i * 17U),
                                  (uint8_t)(180U - i * 18U));
    }

    menu->root = sgl_rect_create(parent);
    if(menu->root == NULL)
    {
        DemoSlidingMenu_Delete(menu);
        return NULL;
    }
    sgl_obj_set_pos(menu->root, x, y);
    sgl_obj_set_size(menu->root, width, height);
    sgl_rect_set_color(menu->root, DEMO_SLIDING_MENU_BG_COLOR);
    sgl_rect_set_alpha(menu->root, 255);
    sgl_rect_set_radius(menu->root, 10);
    sgl_rect_set_border_width(menu->root, 1);
    sgl_rect_set_border_color(menu->root, sgl_rgb(226, 232, 240));

    menu->label = sgl_label_create(menu->root);
    if(menu->label == NULL)
    {
        DemoSlidingMenu_Delete(menu);
        return NULL;
    }
    sgl_obj_set_pos(menu->label, 0, height - DEMO_SLIDING_MENU_LABEL_HEIGHT - 6);
    sgl_obj_set_size(menu->label, width, DEMO_SLIDING_MENU_LABEL_HEIGHT);
    sgl_label_set_font(menu->label, font);
    sgl_label_set_text_align(menu->label, SGL_ALIGN_CENTER);
    sgl_label_set_text_color(menu->label, DEMO_SLIDING_MENU_LABEL_COLOR);

    for(i = 0U; i < item_count; i++)
    {
        size_t len;

        len = strlen(texts[i]);
        menu->texts[i] = (char *)sgl_malloc(len + 1U);
        if(menu->texts[i] == NULL)
        {
            DemoSlidingMenu_Delete(menu);
            return NULL;
        }
        memcpy(menu->texts[i], texts[i], len + 1U);

        menu->items[i] = sgl_button_create(menu->root);
        if(menu->items[i] == NULL)
        {
            DemoSlidingMenu_Delete(menu);
            return NULL;
        }

        sgl_obj_set_unflexible(menu->items[i]);
        sgl_obj_set_pos(menu->items[i], 0, 0);
        sgl_obj_set_size(menu->items[i], 0, 0);
        sgl_button_set_color(menu->items[i], menu->colors[i]);
        sgl_button_set_border_color(menu->items[i], sgl_rgb(245, 247, 250));
        sgl_button_set_border_width(menu->items[i], 2);
        sgl_button_set_radius(menu->items[i], 10);
        sgl_button_set_text(menu->items[i], direction == DEMO_SLIDING_MENU_VERTICAL ? menu->texts[i] : " ");
        sgl_button_set_text_color(menu->items[i], sgl_rgb(255, 255, 255));
        sgl_button_set_text_align(menu->items[i], SGL_ALIGN_CENTER);
        sgl_button_set_font(menu->items[i], font);

        menu->item_ctx[i].menu = menu;
        menu->item_ctx[i].index = i;
        sgl_obj_set_event_cb(menu->items[i], DemoSlidingMenu_OnItemEvent, &menu->item_ctx[i]);
    }

    menu->anim = sgl_anim_create();
    if(menu->anim != NULL)
    {
        sgl_anim_set_data(menu->anim, menu);
        sgl_anim_set_path(menu->anim, DemoSlidingMenu_AnimPath, SGL_ANIM_PATH_EASE_IN_OUT);
        sgl_anim_set_act_duration(menu->anim, DEMO_SLIDING_MENU_ANIM_DURATION_MS);
        sgl_anim_set_finish_cb(menu->anim, DemoSlidingMenu_AnimFinished);
    }

    DemoSlidingMenu_UpdateLayout(menu, 0.0f);
    return menu;
}

void DemoSlidingMenu_Delete(demo_sliding_menu_t *menu)
{
    uint8_t i;

    if(menu == NULL)
    {
        return;
    }

    if(menu->anim != NULL)
    {
        sgl_anim_stop(menu->anim);
        sgl_anim_delete(menu->anim);
        menu->anim = NULL;
    }

    if(menu->items != NULL)
    {
        for(i = 0U; i < menu->item_count; i++)
        {
            if(menu->items[i] != NULL)
            {
                sgl_obj_set_event_cb(menu->items[i], NULL, NULL);
            }
        }
    }

    if(menu->root != NULL)
    {
        sgl_obj_delete(menu->root);
        menu->root = NULL;
    }

    if(menu->texts != NULL)
    {
        for(i = 0U; i < menu->item_count; i++)
        {
            if(menu->texts[i] != NULL)
            {
                sgl_free(menu->texts[i]);
            }
        }
        sgl_free(menu->texts);
    }

    if(menu->colors != NULL)
    {
        sgl_free(menu->colors);
    }

    if(menu->item_ctx != NULL)
    {
        sgl_free(menu->item_ctx);
    }

    if(menu->items != NULL)
    {
        sgl_free(menu->items);
    }

    sgl_free(menu);
}

void DemoSlidingMenu_SetItemColor(demo_sliding_menu_t *menu, uint8_t index, sgl_color_t color)
{
    if((menu == NULL) || (index >= menu->item_count))
    {
        return;
    }

    menu->colors[index] = color;
    sgl_button_set_color(menu->items[index], color);
}

void DemoSlidingMenu_SetVisible(demo_sliding_menu_t *menu, bool visible)
{
    if((menu == NULL) || (menu->root == NULL))
    {
        return;
    }

    if(visible)
    {
        sgl_obj_set_visible(menu->root);
    }
    else
    {
        sgl_obj_set_hidden(menu->root);
    }
}

void DemoSlidingMenu_SetCurrentIndex(demo_sliding_menu_t *menu, uint8_t index)
{
    if((menu == NULL) || (menu->item_count == 0U))
    {
        return;
    }

    if(index >= menu->item_count)
    {
        index = 0U;
    }

    DemoSlidingMenu_CompleteAnimation(menu);
    menu->current_index = (int8_t)index;
    menu->target_index = (int8_t)index;
    menu->drag_dx = 0;
    menu->drag_dy = 0;
    DemoSlidingMenu_UpdateLayout(menu, 1.0f);
}

void DemoSlidingMenu_MoveLeft(demo_sliding_menu_t *menu)
{
    if((menu == NULL) || (menu->direction != DEMO_SLIDING_MENU_HORIZONTAL))
    {
        return;
    }

    DemoSlidingMenu_MoveToIndex(menu, menu->current_index - 1);
}

void DemoSlidingMenu_MoveRight(demo_sliding_menu_t *menu)
{
    if((menu == NULL) || (menu->direction != DEMO_SLIDING_MENU_HORIZONTAL))
    {
        return;
    }

    DemoSlidingMenu_MoveToIndex(menu, menu->current_index + 1);
}

void DemoSlidingMenu_MoveUp(demo_sliding_menu_t *menu)
{
    if((menu == NULL) || (menu->direction != DEMO_SLIDING_MENU_VERTICAL))
    {
        return;
    }

    DemoSlidingMenu_MoveToIndex(menu, menu->current_index - 1);
}

void DemoSlidingMenu_MoveDown(demo_sliding_menu_t *menu)
{
    if((menu == NULL) || (menu->direction != DEMO_SLIDING_MENU_VERTICAL))
    {
        return;
    }

    DemoSlidingMenu_MoveToIndex(menu, menu->current_index + 1);
}

bool DemoSlidingMenu_HandleTouch(demo_sliding_menu_t *menu, uint8_t type, uint16_t x, uint16_t y)
{
    sgl_obj_t *root;
    int16_t local_x;
    int16_t local_y;
    int actual_index;
    uint8_t inside;
    uint8_t handled = 0U;

    if((menu == NULL) || (menu->root == NULL) || (menu->root->hide != 0U))
    {
        return false;
    }

    root = menu->root;
    inside = ((x >= (uint16_t)root->coords.x1) && (x <= (uint16_t)root->coords.x2)
              && (y >= (uint16_t)root->coords.y1) && (y <= (uint16_t)root->coords.y2)) ? 1U : 0U;

    if((type == 1U) && (inside == 0U))
    {
        return false;
    }

    if(((type == 2U) || (type == 3U)) && (menu->touch_active == 0U))
    {
        return false;
    }

    local_x = (int16_t)x - root->coords.x1;
    local_y = (int16_t)y - root->coords.y1;
    actual_index = menu->is_animating ? menu->target_index : menu->current_index;

    switch(type)
    {
        case 1U:
            DemoSlidingMenu_CompleteAnimation(menu);
            menu->drag_dx = 0;
            menu->drag_dy = 0;
            menu->touch_start_x = local_x;
            menu->touch_start_y = local_y;
            menu->touch_active = 1U;
            handled = 1U;
            break;

        case 2U:
            if(menu->is_animating == 0U)
            {
                menu->drag_dx = local_x - menu->touch_start_x;
                menu->drag_dy = local_y - menu->touch_start_y;
                handled = 1U;
            }
            break;

        case 3U:
            menu->touch_active = 0U;
            if(menu->is_animating != 0U)
            {
                DemoSlidingMenu_CompleteAnimation(menu);
                handled = 1U;
                break;
            }

            if(menu->direction == DEMO_SLIDING_MENU_VERTICAL)
            {
                if((sgl_abs(menu->drag_dy) >= DEMO_SLIDING_MENU_DRAG_THRESHOLD)
                   && (sgl_abs(menu->drag_dy) > sgl_abs(menu->drag_dx)))
                {
                    if(menu->drag_dy < 0)
                    {
                        DemoSlidingMenu_MoveDown(menu);
                    }
                    else
                    {
                        DemoSlidingMenu_MoveUp(menu);
                    }
                    handled = 1U;
                }
                else if(inside != 0U)
                {
                    handled = 1U;
                }
            }
            else
            {
                if((sgl_abs(menu->drag_dx) >= DEMO_SLIDING_MENU_DRAG_THRESHOLD)
                   && (sgl_abs(menu->drag_dx) > sgl_abs(menu->drag_dy)))
                {
                    if(menu->drag_dx < 0)
                    {
                        DemoSlidingMenu_MoveRight(menu);
                    }
                    else
                    {
                        DemoSlidingMenu_MoveLeft(menu);
                    }
                    handled = 1U;
                }
                else if(inside != 0U)
                {
                    if((menu->click_cb != NULL) && (actual_index >= 0) && (actual_index < menu->item_count))
                    {
                        menu->click_cb(actual_index, menu->texts[actual_index]);
                    }
                    handled = 1U;
                }
            }

            menu->drag_dx = 0;
            menu->drag_dy = 0;
            break;

        default:
            break;
    }

    return handled != 0U;
}

sgl_obj_t *DemoSlidingMenu_GetRoot(demo_sliding_menu_t *menu)
{
    return menu != NULL ? menu->root : NULL;
}
