#include "sgl_demo_app.h"
#include "debug.h"
#include "sgl.h"
#include "components/sgl_timer.h"
#include "ili9341.h"
#include "ft6336_touch.h"
#include "../../loop/loop.h"
#include "../../midi/midi_mode.h"
#include <string.h>
#include <stdio.h>

/* FreeRTOS 头文件 */
#include "FreeRTOS.h"
#include "task.h"

#define PANEL_BUFFER_LINES              16
#define TOUCH_SCAN_PERIOD_MS            2
#define TOUCH_IDLE_POLL_PERIOD_MS       6
#define LOOP_UI_REFRESH_MS              60
#define TOUCH_TAP_MOVE_THRESHOLD        8
#define TOUCH_SWIPE_THRESHOLD           18

#define LOOP_SLOT_VISIBLE_COUNT         4U
#define LOOP_SLOT_CARD_W                190
#define LOOP_SLOT_CARD_H                38
#define LOOP_SLOT_CARD_GAP_Y            6
#define LOOP_SLOT_ROW_SPAN              (LOOP_SLOT_CARD_H + LOOP_SLOT_CARD_GAP_Y)
#define LOOP_SLOT_LIST_X                10
#define LOOP_SLOT_LIST_Y                54
#define LOOP_SLOT_LIST_BOTTOM           (LOOP_SLOT_LIST_Y + (LOOP_SLOT_VISIBLE_COUNT * LOOP_SLOT_CARD_H) + ((LOOP_SLOT_VISIBLE_COUNT - 1U) * LOOP_SLOT_CARD_GAP_Y) - 1)
#define LOOP_SLOT_LIST_H                (LOOP_SLOT_LIST_BOTTOM - LOOP_SLOT_LIST_Y + 1)
#define LOOP_SLOT_CONTENT_H             ((LOOP_SLOT_COUNT * LOOP_SLOT_CARD_H) + ((LOOP_SLOT_COUNT - 1U) * LOOP_SLOT_CARD_GAP_Y))

#define LOOP_ACTION_BTN_W               92
#define LOOP_ACTION_BTN_H               28
#define LOOP_ACTION_COL_X               214
#define LOOP_ACTION_RECORD_Y            54
#define LOOP_ACTION_PLAY_Y              90
#define LOOP_ACTION_STOP_Y              126
#define LOOP_ACTION_CLEAR_Y             162
#define LOOP_HINT_LINE_CHARS            11U
#define MODE_BTN_W                      46
#define MODE_BTN_H                      20
#define MODE_BTN_Y                      8
#define MODE_FREE_BTN_X                 214
#define MODE_MIDI_BTN_X                 266

typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} sgl_demo_hit_rect_t;

typedef enum
{
    SGL_DEMO_TOUCH_EVT_DOWN = 1,
    SGL_DEMO_TOUCH_EVT_MOVE,
    SGL_DEMO_TOUCH_EVT_UP
} sgl_demo_touch_evt_type_t;

typedef enum
{
    SGL_LOOP_ACTION_RECORD = 0,
    SGL_LOOP_ACTION_PLAY,
    SGL_LOOP_ACTION_STOP,
    SGL_LOOP_ACTION_CLEAR,
    SGL_LOOP_ACTION_CONFIRM_CANCEL,
    SGL_LOOP_ACTION_CONFIRM_OK,
    SGL_LOOP_ACTION_MODE_FREE,
    SGL_LOOP_ACTION_MODE_MIDI,
    SGL_LOOP_ACTION_NONE = 0xFF
} sgl_loop_action_t;

static sgl_color_t g_panel_buffer0[LCD_W * PANEL_BUFFER_LINES];
static sgl_color_t g_panel_buffer1[LCD_W * PANEL_BUFFER_LINES];

static sgl_obj_t *g_loop_page = NULL;
static sgl_obj_t *g_header_title = NULL;
static sgl_obj_t *g_status_label = NULL;
static sgl_obj_t *g_selected_label = NULL;
static sgl_obj_t *g_hint_label = NULL;
static sgl_obj_t *g_hint_label2 = NULL;
static sgl_obj_t *g_mode_free_btn = NULL;
static sgl_obj_t *g_mode_midi_btn = NULL;
static sgl_obj_t *g_slot_scroll_box = NULL;
static sgl_obj_t *g_slot_cards[LOOP_SLOT_COUNT] = {0};
static sgl_obj_t *g_slot_title_labels[LOOP_SLOT_COUNT] = {0};
static sgl_obj_t *g_slot_state_labels[LOOP_SLOT_COUNT] = {0};
static sgl_obj_t *g_slot_meta_labels[LOOP_SLOT_COUNT] = {0};
static sgl_obj_t *g_record_btn = NULL;
static sgl_obj_t *g_play_btn = NULL;
static sgl_obj_t *g_stop_btn = NULL;
static sgl_obj_t *g_clear_btn = NULL;
static sgl_obj_t *g_confirm_mask = NULL;
static sgl_obj_t *g_confirm_box = NULL;
static sgl_obj_t *g_confirm_title = NULL;
static sgl_obj_t *g_confirm_text = NULL;
static sgl_obj_t *g_confirm_cancel_btn = NULL;
static sgl_obj_t *g_confirm_ok_btn = NULL;
static volatile uint8_t g_touch_scan_pending = 0;
static uint16_t g_last_touch_x = 0;
static uint16_t g_last_touch_y = 0;
static uint16_t g_touch_down_x = 0;
static uint16_t g_touch_down_y = 0;
static uint8_t g_touch_active = 0;
static uint8_t g_touch_started_on_slots = 0;
static uint8_t g_touch_drag_handled = 0;
static uint8_t g_slot_first_visible = 0;
static int16_t g_slot_scroll_offset = 0;
static int16_t g_slot_press_scroll_offset = 0;
static uint8_t g_confirm_visible = 0;
static loop_ui_state_t g_loop_ui_state = {0};
static app_mode_status_t g_app_mode_status = {0};
static TickType_t g_last_ui_refresh_tick = 0;
static char g_ui_hint[48] = "点击顶部切换模式";
static char g_ui_hint_line1[24] = "点击顶部切换模式";
static char g_ui_hint_line2[24] = "";
static TaskHandle_t SGLTask_Handler;

static void SGL_DemoApp_RefreshLoopUI(uint8_t force);

static const sgl_demo_hit_rect_t g_action_hit_rects[] = {
    {LOOP_ACTION_COL_X, LOOP_ACTION_RECORD_Y, LOOP_ACTION_COL_X + LOOP_ACTION_BTN_W - 1, LOOP_ACTION_RECORD_Y + LOOP_ACTION_BTN_H - 1},
    {LOOP_ACTION_COL_X, LOOP_ACTION_PLAY_Y, LOOP_ACTION_COL_X + LOOP_ACTION_BTN_W - 1, LOOP_ACTION_PLAY_Y + LOOP_ACTION_BTN_H - 1},
    {LOOP_ACTION_COL_X, LOOP_ACTION_STOP_Y, LOOP_ACTION_COL_X + LOOP_ACTION_BTN_W - 1, LOOP_ACTION_STOP_Y + LOOP_ACTION_BTN_H - 1},
    {LOOP_ACTION_COL_X, LOOP_ACTION_CLEAR_Y, LOOP_ACTION_COL_X + LOOP_ACTION_BTN_W - 1, LOOP_ACTION_CLEAR_Y + LOOP_ACTION_BTN_H - 1}
};

static const sgl_demo_hit_rect_t g_slot_view_rect = {LOOP_SLOT_LIST_X, LOOP_SLOT_LIST_Y, LOOP_SLOT_LIST_X + LOOP_SLOT_CARD_W - 1, LOOP_SLOT_LIST_BOTTOM};
static const sgl_demo_hit_rect_t g_confirm_cancel_rect = {82, 139, 145, 162};
static const sgl_demo_hit_rect_t g_confirm_ok_rect = {174, 139, 237, 162};
static const sgl_demo_hit_rect_t g_mode_free_rect = {MODE_FREE_BTN_X, MODE_BTN_Y, MODE_FREE_BTN_X + MODE_BTN_W - 1, MODE_BTN_Y + MODE_BTN_H - 1};
static const sgl_demo_hit_rect_t g_mode_midi_rect = {MODE_MIDI_BTN_X, MODE_BTN_Y, MODE_MIDI_BTN_X + MODE_BTN_W - 1, MODE_BTN_Y + MODE_BTN_H - 1};

static void SGL_DemoApp_FlushArea(sgl_area_t *area, sgl_color_t *src)
{
    uint16_t width = (uint16_t)(area->x2 - area->x1 + 1);
    uint16_t height = (uint16_t)(area->y2 - area->y1 + 1);
    uint16_t bytes = (uint16_t)(width * height * sizeof(sgl_color_t));

    ILI9341_FlushArea((uint16_t)area->x1,
                      (uint16_t)area->x2,
                      (uint16_t)area->y1,
                      (uint16_t)area->y2,
                      src,
                      bytes);
}

static void SGL_DemoApp_Tim2Init(uint16_t arr, uint16_t psc)
{
    NVIC_InitTypeDef nvic_init = {0};
    TIM_TimeBaseInitTypeDef tim_init = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    tim_init.TIM_Period = arr;
    tim_init.TIM_Prescaler = psc;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &tim_init);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    nvic_init.NVIC_IRQChannel = TIM2_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

static void SGL_DemoApp_Tim3Init(uint16_t arr, uint16_t psc)
{
    NVIC_InitTypeDef nvic_init = {0};
    TIM_TimeBaseInitTypeDef tim_init = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    tim_init.TIM_Period = arr;
    tim_init.TIM_Prescaler = psc;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &tim_init);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

    nvic_init.NVIC_IRQChannel = TIM3_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

static sgl_obj_t *SGL_DemoApp_CreateLabel(sgl_obj_t *parent,
                                          int16_t x,
                                          int16_t y,
                                          int16_t width,
                                          int16_t height,
                                          const char *text,
                                          sgl_color_t color,
                                          const sgl_font_t *font,
                                          sgl_align_type_t align)
{
    sgl_obj_t *label = sgl_label_create(parent);

    if(label == NULL)
    {
        return NULL;
    }

    sgl_obj_set_pos(label, x, y);
    sgl_obj_set_size(label, width, height);
    sgl_label_set_text(label, (char *)text);
    sgl_label_set_text_color(label, color);
    if(font != NULL)
    {
        sgl_label_set_font(label, font);
    }
    sgl_label_set_text_align(label, align);
    return label;
}

static sgl_obj_t *SGL_DemoApp_CreateButton(sgl_obj_t *parent,
                                           int16_t x,
                                           int16_t y,
                                           int16_t width,
                                           int16_t height,
                                           const char *text,
                                           sgl_color_t color,
                                           sgl_color_t text_color,
                                           sgl_color_t border_color)
{
    sgl_obj_t *button = sgl_button_create(parent);

    if(button == NULL)
    {
        return NULL;
    }

    sgl_obj_set_pos(button, x, y);
    sgl_obj_set_size(button, width, height);
    sgl_button_set_color(button, color);
    sgl_button_set_text(button, text);
    sgl_button_set_text_color(button, text_color);
    sgl_button_set_border_color(button, border_color);
    sgl_button_set_border_width(button, 1);
    sgl_button_set_radius(button, 8);
    sgl_button_set_font(button, &consolas14);
    sgl_button_set_text_align(button, SGL_ALIGN_CENTER);
    sgl_obj_set_unflexible(button);
    sgl_obj_set_event_cb(button, NULL, NULL);
    return button;
}

static uint8_t SGL_DemoApp_PointInRect(const sgl_demo_hit_rect_t *rect, uint16_t x, uint16_t y)
{
    if(rect == NULL)
    {
        return 0U;
    }

    return (uint8_t)((x >= (uint16_t)rect->x1) &&
                     (x <= (uint16_t)rect->x2) &&
                     (y >= (uint16_t)rect->y1) &&
                     (y <= (uint16_t)rect->y2));
}

static uint8_t SGL_DemoApp_GetMaxSlotFirstVisible(void)
{
    return (LOOP_SLOT_COUNT > LOOP_SLOT_VISIBLE_COUNT) ? (uint8_t)(LOOP_SLOT_COUNT - LOOP_SLOT_VISIBLE_COUNT) : 0U;
}

static int16_t SGL_DemoApp_GetMinSlotScrollOffset(void)
{
    int16_t min_offset = (int16_t)LOOP_SLOT_LIST_H - (int16_t)LOOP_SLOT_CONTENT_H;

    return (min_offset < 0) ? min_offset : 0;
}

static void SGL_DemoApp_ApplySlotScrollOffset(int16_t target_offset)
{
    int16_t min_offset = SGL_DemoApp_GetMinSlotScrollOffset();

    target_offset = sgl_clamp(target_offset, min_offset, 0);
    if((target_offset == g_slot_scroll_offset) && (g_slot_scroll_box != NULL))
    {
        return;
    }

    g_slot_scroll_offset = target_offset;
    if(g_slot_scroll_box != NULL)
    {
        sgl_box_set_scroll_offset(g_slot_scroll_box, 0, target_offset);
    }
}

static void SGL_DemoApp_SyncSlotScrollToTouch(void)
{
    int16_t touch_delta_y = (int16_t)g_last_touch_y - (int16_t)g_touch_down_y;

    SGL_DemoApp_ApplySlotScrollOffset((int16_t)(g_slot_press_scroll_offset + touch_delta_y));
}

static uint8_t SGL_DemoApp_PointInSlotView(uint16_t x, uint16_t y)
{
    return SGL_DemoApp_PointInRect(&g_slot_view_rect, x, y);
}

static int8_t SGL_DemoApp_HitTestSlot(uint16_t x, uint16_t y)
{
    uint8_t slot_index;
    int16_t local_y;

    if(SGL_DemoApp_PointInSlotView(x, y) == 0U)
    {
        return -1;
    }

    local_y = (int16_t)y - (int16_t)LOOP_SLOT_LIST_Y - g_slot_scroll_offset;
    if(local_y < 0)
    {
        return -1;
    }

    if((local_y % (int16_t)LOOP_SLOT_ROW_SPAN) >= (int16_t)LOOP_SLOT_CARD_H)
    {
        return -1;
    }

    slot_index = (uint8_t)(local_y / (int16_t)LOOP_SLOT_ROW_SPAN);
    if(slot_index >= LOOP_SLOT_COUNT)
    {
        return -1;
    }

    return (int8_t)slot_index;
}

static sgl_loop_action_t SGL_DemoApp_HitTestAction(uint16_t x, uint16_t y)
{
    uint8_t i;

    for(i = 0U; i < SGL_ARRAY_SIZE(g_action_hit_rects); i++)
    {
        if(SGL_DemoApp_PointInRect(&g_action_hit_rects[i], x, y) != 0U)
        {
            return (sgl_loop_action_t)i;
        }
    }

    if(SGL_DemoApp_PointInRect(&g_confirm_cancel_rect, x, y) != 0U)
    {
        return SGL_LOOP_ACTION_CONFIRM_CANCEL;
    }

    if(SGL_DemoApp_PointInRect(&g_confirm_ok_rect, x, y) != 0U)
    {
        return SGL_LOOP_ACTION_CONFIRM_OK;
    }

    if(SGL_DemoApp_PointInRect(&g_mode_free_rect, x, y) != 0U)
    {
        return SGL_LOOP_ACTION_MODE_FREE;
    }

    if(SGL_DemoApp_PointInRect(&g_mode_midi_rect, x, y) != 0U)
    {
        return SGL_LOOP_ACTION_MODE_MIDI;
    }

    return SGL_LOOP_ACTION_NONE;
}

static void SGL_DemoApp_SetHint(const char *text)
{
    size_t len;
    size_t split;
    size_t line1_len;
    const char *line2_src;

    if(text == NULL)
    {
        return;
    }

    (void)snprintf(g_ui_hint, sizeof(g_ui_hint), "%s", text);
    len = strlen(g_ui_hint);
    if(len <= LOOP_HINT_LINE_CHARS)
    {
        (void)snprintf(g_ui_hint_line1, sizeof(g_ui_hint_line1), "%s", g_ui_hint);
        g_ui_hint_line2[0] = '\0';
        return;
    }

    split = LOOP_HINT_LINE_CHARS;
    while((split > 0U) && (g_ui_hint[split] != ' '))
    {
        split--;
    }

    if(split == 0U)
    {
        split = LOOP_HINT_LINE_CHARS;
        line1_len = split;
        line2_src = &g_ui_hint[split];
    }
    else
    {
        line1_len = split;
        line2_src = &g_ui_hint[split + 1U];
    }

    if(line1_len >= sizeof(g_ui_hint_line1))
    {
        line1_len = sizeof(g_ui_hint_line1) - 1U;
    }

    memcpy(g_ui_hint_line1, g_ui_hint, line1_len);
    g_ui_hint_line1[line1_len] = '\0';
    (void)snprintf(g_ui_hint_line2, sizeof(g_ui_hint_line2), "%s", line2_src);
}

static void SGL_DemoApp_SetConfirmVisible(uint8_t visible)
{
    g_confirm_visible = visible;

    if(g_confirm_mask != NULL)
    {
        if(visible != 0U)
        {
            sgl_obj_set_visible(g_confirm_mask);
        }
        else
        {
            sgl_obj_set_hidden(g_confirm_mask);
        }
    }

    if(g_confirm_box != NULL)
    {
        if(visible != 0U)
        {
            sgl_obj_set_visible(g_confirm_box);
        }
        else
        {
            sgl_obj_set_hidden(g_confirm_box);
        }
    }
}

static void SGL_DemoApp_UpdateSlotLayout(void)
{
    uint8_t i;
    uint8_t max_first = SGL_DemoApp_GetMaxSlotFirstVisible();

    if(g_slot_first_visible > max_first)
    {
        g_slot_first_visible = max_first;
    }

    for(i = 0U; i < LOOP_SLOT_COUNT; i++)
    {
        if(g_slot_cards[i] == NULL)
        {
            continue;
        }

        if(g_slot_scroll_box != NULL)
        {
            int16_t y = (int16_t)((uint16_t)i * LOOP_SLOT_ROW_SPAN);
            sgl_obj_set_visible(g_slot_cards[i]);
            if(g_slot_scroll_offset == 0)
            {
                sgl_obj_set_pos(g_slot_cards[i], 0, y);
            }
        }
        else
        {
            if((i >= g_slot_first_visible) && (i < (uint8_t)(g_slot_first_visible + LOOP_SLOT_VISIBLE_COUNT)))
            {
                uint8_t row = (uint8_t)(i - g_slot_first_visible);
                int16_t y = (int16_t)(LOOP_SLOT_LIST_Y + ((uint16_t)row * LOOP_SLOT_ROW_SPAN));

                sgl_obj_set_pos(g_slot_cards[i], LOOP_SLOT_LIST_X, y);
                sgl_obj_set_visible(g_slot_cards[i]);
            }
            else
            {
                sgl_obj_set_hidden(g_slot_cards[i]);
            }
        }
    }
}

static void SGL_DemoApp_UpdateSlotCard(uint8_t slot_index, const loop_ui_state_t *state)
{
    sgl_color_t card_color = sgl_rgb(248, 250, 252);
    sgl_color_t border_color = sgl_rgb(203, 213, 225);
    sgl_color_t title_color = sgl_rgb(15, 23, 42);
    sgl_color_t state_color = sgl_rgb(71, 85, 105);
    sgl_color_t meta_color = sgl_rgb(100, 116, 139);
    char title_text[16];
    char state_text[16];
    char meta_text[32];

    if(slot_index >= LOOP_SLOT_COUNT)
    {
        return;
    }

    if(state->selected_slot == slot_index)
    {
        card_color = sgl_rgb(239, 246, 255);
        border_color = sgl_rgb(59, 130, 246);
    }

    if((state->recording != 0U) && (state->record_slot == slot_index))
    {
        card_color = sgl_rgb(254, 242, 242);
        border_color = sgl_rgb(239, 68, 68);
        state_color = sgl_rgb(185, 28, 28);
    }
    else if((state->playing != 0U) && (state->play_slot == slot_index))
    {
        card_color = sgl_rgb(240, 253, 244);
        border_color = sgl_rgb(34, 197, 94);
        state_color = sgl_rgb(21, 128, 61);
    }
    else if(state->slot_has_file[slot_index] != 0U)
    {
        state_color = sgl_rgb(37, 99, 235);
    }

    (void)snprintf(title_text, sizeof(title_text), "SLOT %u", (unsigned)(slot_index + 1U));

    if((state->recording != 0U) && (state->record_slot == slot_index))
    {
        (void)snprintf(state_text, sizeof(state_text), "REC");
    }
    else if((state->playing != 0U) && (state->play_slot == slot_index))
    {
        (void)snprintf(state_text, sizeof(state_text), "PLAY");
    }
    else if(state->slot_has_file[slot_index] != 0U)
    {
        (void)snprintf(state_text, sizeof(state_text), "USED");
    }
    else
    {
        (void)snprintf(state_text, sizeof(state_text), "EMPTY");
    }

    if(state->slot_has_file[slot_index] != 0U)
    {
        (void)snprintf(meta_text,
                       sizeof(meta_text),
                       "%luev %lut",
                       state->slot_event_count[slot_index],
                       state->slot_length_ticks[slot_index]);
    }
    else
    {
        (void)snprintf(meta_text, sizeof(meta_text), "No data");
    }

    if(g_slot_cards[slot_index] != NULL)
    {
        sgl_button_set_color(g_slot_cards[slot_index], card_color);
        sgl_button_set_border_color(g_slot_cards[slot_index], border_color);
    }

    if(g_slot_title_labels[slot_index] != NULL)
    {
        sgl_label_set_text_fmt(g_slot_title_labels[slot_index], "%s", title_text);
        sgl_label_set_text_color(g_slot_title_labels[slot_index], title_color);
    }

    if(g_slot_state_labels[slot_index] != NULL)
    {
        sgl_label_set_text_fmt(g_slot_state_labels[slot_index], "%s", state_text);
        sgl_label_set_text_color(g_slot_state_labels[slot_index], state_color);
    }

    if(g_slot_meta_labels[slot_index] != NULL)
    {
        sgl_label_set_text_fmt(g_slot_meta_labels[slot_index], "%s", meta_text);
        sgl_label_set_text_color(g_slot_meta_labels[slot_index], meta_color);
    }
}

static void SGL_DemoApp_UpdateModeButtons(void)
{
    sgl_color_t free_bg = sgl_rgb(241, 245, 249);
    sgl_color_t free_text = sgl_rgb(51, 65, 85);
    sgl_color_t free_border = sgl_rgb(148, 163, 184);
    sgl_color_t midi_bg = sgl_rgb(241, 245, 249);
    sgl_color_t midi_text = sgl_rgb(51, 65, 85);
    sgl_color_t midi_border = sgl_rgb(148, 163, 184);

    if(g_app_mode_status.current_mode == APP_MODE_FREE_PLAY)
    {
        free_bg = sgl_rgb(37, 99, 235);
        free_text = sgl_rgb(255, 255, 255);
        free_border = sgl_rgb(29, 78, 216);
    }
    else if(g_app_mode_status.current_mode == APP_MODE_MIDI_KEYBOARD)
    {
        midi_bg = sgl_rgb(22, 163, 74);
        midi_text = sgl_rgb(255, 255, 255);
        midi_border = sgl_rgb(21, 128, 61);
    }

    if(g_mode_free_btn != NULL)
    {
        sgl_button_set_color(g_mode_free_btn, free_bg);
        sgl_button_set_text_color(g_mode_free_btn, free_text);
        sgl_button_set_border_color(g_mode_free_btn, free_border);
    }

    if(g_mode_midi_btn != NULL)
    {
        sgl_button_set_color(g_mode_midi_btn, midi_bg);
        sgl_button_set_text_color(g_mode_midi_btn, midi_text);
        sgl_button_set_border_color(g_mode_midi_btn, midi_border);
    }
}

static void SGL_DemoApp_UpdateActionButtons(const loop_ui_state_t *state)
{
    uint8_t loop_enabled = app_mode_is_loop_enabled();

    if(g_record_btn != NULL)
    {
        sgl_button_set_color(g_record_btn,
                             (loop_enabled == 0U) ? sgl_rgb(226, 232, 240) : ((state->recording != 0U) ? sgl_rgb(239, 68, 68) : sgl_rgb(255, 245, 245)));
        sgl_button_set_border_color(g_record_btn,
                                    (loop_enabled == 0U) ? sgl_rgb(203, 213, 225) : ((state->recording != 0U) ? sgl_rgb(185, 28, 28) : sgl_rgb(248, 113, 113)));
        sgl_button_set_text_color(g_record_btn,
                                  (loop_enabled == 0U) ? sgl_rgb(100, 116, 139) : ((state->recording != 0U) ? sgl_rgb(255, 255, 255) : sgl_rgb(127, 29, 29)));
    }

    if(g_play_btn != NULL)
    {
        sgl_button_set_color(g_play_btn,
                             (loop_enabled == 0U) ? sgl_rgb(226, 232, 240) : ((state->playing != 0U) ? sgl_rgb(34, 197, 94) : sgl_rgb(240, 253, 244)));
        sgl_button_set_border_color(g_play_btn,
                                    (loop_enabled == 0U) ? sgl_rgb(203, 213, 225) : ((state->playing != 0U) ? sgl_rgb(21, 128, 61) : sgl_rgb(74, 222, 128)));
        sgl_button_set_text_color(g_play_btn,
                                  (loop_enabled == 0U) ? sgl_rgb(100, 116, 139) : ((state->playing != 0U) ? sgl_rgb(255, 255, 255) : sgl_rgb(20, 83, 45)));
    }

    if(g_stop_btn != NULL)
    {
        sgl_button_set_color(g_stop_btn, (loop_enabled != 0U) ? sgl_rgb(241, 245, 249) : sgl_rgb(226, 232, 240));
        sgl_button_set_border_color(g_stop_btn, (loop_enabled != 0U) ? sgl_rgb(148, 163, 184) : sgl_rgb(203, 213, 225));
        sgl_button_set_text_color(g_stop_btn, (loop_enabled != 0U) ? sgl_rgb(51, 65, 85) : sgl_rgb(100, 116, 139));
    }

    if(g_clear_btn != NULL)
    {
        sgl_button_set_color(g_clear_btn, (loop_enabled != 0U) ? sgl_rgb(255, 247, 237) : sgl_rgb(226, 232, 240));
        sgl_button_set_border_color(g_clear_btn, (loop_enabled != 0U) ? sgl_rgb(251, 146, 60) : sgl_rgb(203, 213, 225));
        sgl_button_set_text_color(g_clear_btn, (loop_enabled != 0U) ? sgl_rgb(154, 52, 18) : sgl_rgb(100, 116, 139));
    }
}

static void SGL_DemoApp_RefreshLoopUI(uint8_t force)
{
    TickType_t now_tick;
    char status_text[48];
    char selected_text[64];
    uint8_t selected_slot;

    now_tick = xTaskGetTickCount();
    if((force == 0U) && ((TickType_t)(now_tick - g_last_ui_refresh_tick) < pdMS_TO_TICKS(LOOP_UI_REFRESH_MS)))
    {
        return;
    }

    g_last_ui_refresh_tick = now_tick;
    app_mode_get_status(&g_app_mode_status);
    loop_get_ui_state(&g_loop_ui_state);
    selected_slot = g_loop_ui_state.selected_slot;
    if(selected_slot >= LOOP_SLOT_COUNT)
    {
        selected_slot = 0U;
    }

    if(g_app_mode_status.switching != 0U)
    {
        (void)snprintf(status_text, sizeof(status_text), "Mode: switching");
        (void)snprintf(selected_text, sizeof(selected_text), "Please wait...");
    }
    else if(g_app_mode_status.current_mode == APP_MODE_MIDI_KEYBOARD)
    {
        (void)snprintf(status_text,
                       sizeof(status_text),
                       "Mode: MIDI  USB:%s",
                       (g_app_mode_status.midi_ready != 0U) ? "OK" : "WAIT");
        (void)snprintf(selected_text,
                       sizeof(selected_text),
                       "Note:%u Vel:%u Err:%lu",
                       (unsigned)g_app_mode_status.last_note,
                       (unsigned)g_app_mode_status.last_velocity,
                       g_app_mode_status.send_error_count);
    }
    else if((g_loop_ui_state.recording != 0U) && (g_loop_ui_state.record_slot < LOOP_SLOT_COUNT))
    {
        (void)snprintf(status_text,
                       sizeof(status_text),
                       "State: REC slot %u",
                       (unsigned)(g_loop_ui_state.record_slot + 1U));
        (void)snprintf(selected_text,
                       sizeof(selected_text),
                       "Mode: %s",
                       app_mode_get_name(g_app_mode_status.current_mode));
    }
    else if((g_loop_ui_state.playing != 0U) && (g_loop_ui_state.play_slot < LOOP_SLOT_COUNT))
    {
        (void)snprintf(status_text,
                       sizeof(status_text),
                       "State: PLAY slot %u",
                       (unsigned)(g_loop_ui_state.play_slot + 1U));
        (void)snprintf(selected_text,
                       sizeof(selected_text),
                       "Mode: %s",
                       app_mode_get_name(g_app_mode_status.current_mode));
    }
    else if(g_loop_ui_state.slot_has_file[selected_slot] != 0U)
    {
        (void)snprintf(status_text, sizeof(status_text), "State: IDLE");
        (void)snprintf(selected_text,
                       sizeof(selected_text),
                       "Selected:%u %luev %lut",
                       (unsigned)(selected_slot + 1U),
                       g_loop_ui_state.slot_event_count[selected_slot],
                       g_loop_ui_state.slot_length_ticks[selected_slot]);
    }
    else
    {
        (void)snprintf(status_text, sizeof(status_text), "State: IDLE");
        (void)snprintf(selected_text,
                       sizeof(selected_text),
                       "Selected: %u empty",
                       (unsigned)(selected_slot + 1U));
    }

    if(g_status_label != NULL)
    {
        sgl_label_set_text_fmt(g_status_label, "%s", status_text);
    }

    if(g_selected_label != NULL)
    {
        sgl_label_set_text_fmt(g_selected_label, "%s", selected_text);
    }

    if(g_hint_label != NULL)
    {
        sgl_label_set_text_fmt(g_hint_label, "%s", g_ui_hint_line1);
    }

    if(g_hint_label2 != NULL)
    {
        sgl_label_set_text_fmt(g_hint_label2, "%s", g_ui_hint_line2);
    }

    for(selected_slot = 0U; selected_slot < LOOP_SLOT_COUNT; selected_slot++)
    {
        SGL_DemoApp_UpdateSlotCard(selected_slot, &g_loop_ui_state);
    }

    SGL_DemoApp_UpdateModeButtons();
    SGL_DemoApp_UpdateActionButtons(&g_loop_ui_state);
    SGL_DemoApp_UpdateSlotLayout();
}

static void SGL_DemoApp_CreateSlotCard(sgl_obj_t *parent, uint8_t slot_index, int16_t x, int16_t y)
{
    sgl_obj_t *card;

    if((parent == NULL) || (slot_index >= LOOP_SLOT_COUNT))
    {
        return;
    }

    card = SGL_DemoApp_CreateButton(parent,
                                    x,
                                    y,
                                    LOOP_SLOT_CARD_W,
                                    LOOP_SLOT_CARD_H,
                                    "",
                                    sgl_rgb(248, 250, 252),
                                    sgl_rgb(15, 23, 42),
                                    sgl_rgb(203, 213, 225));
    if(card == NULL)
    {
        return;
    }

    g_slot_cards[slot_index] = card;
    g_slot_title_labels[slot_index] = SGL_DemoApp_CreateLabel(card,
                                                              8,
                                                              4,
                                                              72,
                                                              12,
                                                              "SLOT",
                                                              sgl_rgb(15, 23, 42),
                                                              &consolas14,
                                                              SGL_ALIGN_LEFT_MID);
    g_slot_state_labels[slot_index] = SGL_DemoApp_CreateLabel(card,
                                                              126,
                                                              4,
                                                              56,
                                                              12,
                                                              "EMPTY",
                                                              sgl_rgb(71, 85, 105),
                                                              &consolas14,
                                                              SGL_ALIGN_RIGHT_MID);
    g_slot_meta_labels[slot_index] = SGL_DemoApp_CreateLabel(card,
                                                             8,
                                                             22,
                                                             174,
                                                             12,
                                                             "No data",
                                                             sgl_rgb(100, 116, 139),
                                                             &consolas14,
                                                             SGL_ALIGN_LEFT_MID);
}

static void SGL_DemoApp_CreateConfirmDialog(sgl_obj_t *parent)
{
    if(parent == NULL)
    {
        return;
    }

    g_confirm_mask = sgl_box_create(parent);
    if(g_confirm_mask != NULL)
    {
        sgl_obj_set_pos(g_confirm_mask, 0, 0);
        sgl_obj_set_size(g_confirm_mask, LCD_W, LCD_H);
        sgl_box_set_bg_color(g_confirm_mask, sgl_rgb(60, 60, 72));
        sgl_box_set_show_scrollbar(g_confirm_mask, 0, 0);
        ((sgl_box_t *)g_confirm_mask)->scroll_enable = 0;
        sgl_obj_set_hidden(g_confirm_mask);
    }

    g_confirm_box = sgl_box_create(parent);
    if(g_confirm_box == NULL)
    {
        return;
    }

    sgl_obj_set_unmovable(g_confirm_box);
    sgl_obj_set_pos(g_confirm_box, 64, 69);
    sgl_obj_set_size(g_confirm_box, 192, 102);
    sgl_box_set_bg_color(g_confirm_box, sgl_rgb(255, 255, 255));
    sgl_box_set_border_color(g_confirm_box, sgl_rgb(226, 232, 240));
    sgl_box_set_border_width(g_confirm_box, 1);
    sgl_box_set_radius(g_confirm_box, 10);
    sgl_box_set_show_scrollbar(g_confirm_box, 0, 0);
    ((sgl_box_t *)g_confirm_box)->scroll_enable = 0;

    g_confirm_title = SGL_DemoApp_CreateLabel(g_confirm_box,
                                              12,
                                              10,
                                              168,
                                              14,
                                              "Clear selected slot?",
                                              sgl_rgb(15, 23, 42),
                                              &consolas14,
                                              SGL_ALIGN_LEFT_MID);
    g_confirm_text = SGL_DemoApp_CreateLabel(g_confirm_box,
                                             12,
                                             38,
                                             168,
                                             24,
                                             "This will delete loop file.",
                                             sgl_rgb(71, 85, 105),
                                             &consolas14,
                                             SGL_ALIGN_LEFT_MID);
    g_confirm_cancel_btn = SGL_DemoApp_CreateButton(g_confirm_box,
                                                    18,
                                                    70,
                                                    64,
                                                    24,
                                                    "Cancel",
                                                    sgl_rgb(241, 245, 249),
                                                    sgl_rgb(51, 65, 85),
                                                    sgl_rgb(203, 213, 225));
    g_confirm_ok_btn = SGL_DemoApp_CreateButton(g_confirm_box,
                                                110,
                                                70,
                                                64,
                                                24,
                                                "Clear",
                                                sgl_rgb(239, 68, 68),
                                                sgl_rgb(255, 255, 255),
                                                sgl_rgb(185, 28, 28));
    sgl_obj_set_hidden(g_confirm_box);
}

static void SGL_DemoApp_SetupUI(void)
{
    uint8_t slot_index;
    sgl_obj_t *bg;

    g_loop_page = sgl_obj_create(NULL);
    if(g_loop_page == NULL)
    {
        return;
    }

    bg = sgl_box_create(g_loop_page);
    if(bg != NULL)
    {
        sgl_obj_set_pos(bg, 0, 0);
        sgl_obj_set_size(bg, LCD_W, LCD_H);
        sgl_box_set_bg_color(bg, sgl_rgb(236, 240, 245));
        sgl_box_set_show_scrollbar(bg, 0, 0);
        ((sgl_box_t *)bg)->scroll_enable = 0;
    }

    g_header_title = SGL_DemoApp_CreateLabel(g_loop_page,
                                             12,
                                             8,
                                             130,
                                             16,
                                             "Synth Control",
                                             sgl_rgb(15, 23, 42),
                                             &consolas14,
                                             SGL_ALIGN_LEFT_MID);
    g_mode_free_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                               MODE_FREE_BTN_X,
                                               MODE_BTN_Y,
                                               MODE_BTN_W,
                                               MODE_BTN_H,
                                               "Free",
                                               sgl_rgb(241, 245, 249),
                                               sgl_rgb(51, 65, 85),
                                               sgl_rgb(148, 163, 184));
    g_mode_midi_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                               MODE_MIDI_BTN_X,
                                               MODE_BTN_Y,
                                               MODE_BTN_W,
                                               MODE_BTN_H,
                                               "MIDI",
                                               sgl_rgb(241, 245, 249),
                                               sgl_rgb(51, 65, 85),
                                               sgl_rgb(148, 163, 184));
    g_status_label = SGL_DemoApp_CreateLabel(g_loop_page,
                                             12,
                                             28,
                                             188,
                                             12,
                                             "State: IDLE",
                                             sgl_rgb(30, 41, 59),
                                             &consolas14,
                                             SGL_ALIGN_LEFT_MID);
    g_selected_label = SGL_DemoApp_CreateLabel(g_loop_page,
                                               12,
                                               42,
                                               188,
                                               12,
                                               "Selected: 1 empty",
                                               sgl_rgb(37, 99, 235),
                                               &consolas14,
                                               SGL_ALIGN_LEFT_MID);

    g_slot_scroll_box = sgl_box_create(g_loop_page);
    if(g_slot_scroll_box != NULL)
    {
        sgl_obj_set_pos(g_slot_scroll_box, LOOP_SLOT_LIST_X, LOOP_SLOT_LIST_Y);
        sgl_obj_set_size(g_slot_scroll_box, LOOP_SLOT_CARD_W, LOOP_SLOT_LIST_H);
        sgl_box_set_bg_color(g_slot_scroll_box, sgl_rgb(236, 240, 245));
        sgl_box_set_border_width(g_slot_scroll_box, 0);
        sgl_box_set_radius(g_slot_scroll_box, 0);
        sgl_box_set_show_scrollbar(g_slot_scroll_box, 0, 0);
        ((sgl_box_t *)g_slot_scroll_box)->scroll_enable = 0;
    }

    for(slot_index = 0U; slot_index < LOOP_SLOT_COUNT; slot_index++)
    {
        SGL_DemoApp_CreateSlotCard(g_slot_scroll_box != NULL ? g_slot_scroll_box : g_loop_page,
                                   slot_index,
                                   g_slot_scroll_box != NULL ? 0 : LOOP_SLOT_LIST_X,
                                   (int16_t)((g_slot_scroll_box != NULL ? 0 : LOOP_SLOT_LIST_Y) + ((uint16_t)slot_index * LOOP_SLOT_ROW_SPAN)));
    }
    SGL_DemoApp_UpdateSlotLayout();

    g_hint_label = SGL_DemoApp_CreateLabel(g_loop_page,
                                           LOOP_ACTION_COL_X,
                                           198,
                                           LOOP_ACTION_BTN_W,
                                           12,
                                           g_ui_hint_line1,
                                           sgl_rgb(100, 116, 139),
                                           &consolas14,
                                           SGL_ALIGN_LEFT_MID);
    g_hint_label2 = SGL_DemoApp_CreateLabel(g_loop_page,
                                            LOOP_ACTION_COL_X,
                                            212,
                                            LOOP_ACTION_BTN_W,
                                            12,
                                            g_ui_hint_line2,
                                            sgl_rgb(100, 116, 139),
                                            &consolas14,
                                            SGL_ALIGN_LEFT_MID);

    g_record_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                            LOOP_ACTION_COL_X,
                                            LOOP_ACTION_RECORD_Y,
                                            LOOP_ACTION_BTN_W,
                                            LOOP_ACTION_BTN_H,
                                            "Record",
                                            sgl_rgb(255, 245, 245),
                                            sgl_rgb(127, 29, 29),
                                            sgl_rgb(248, 113, 113));
    g_play_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                          LOOP_ACTION_COL_X,
                                          LOOP_ACTION_PLAY_Y,
                                          LOOP_ACTION_BTN_W,
                                          LOOP_ACTION_BTN_H,
                                          "Play",
                                          sgl_rgb(240, 253, 244),
                                          sgl_rgb(20, 83, 45),
                                          sgl_rgb(74, 222, 128));
    g_stop_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                          LOOP_ACTION_COL_X,
                                          LOOP_ACTION_STOP_Y,
                                          LOOP_ACTION_BTN_W,
                                          LOOP_ACTION_BTN_H,
                                          "Stop",
                                          sgl_rgb(241, 245, 249),
                                          sgl_rgb(51, 65, 85),
                                          sgl_rgb(148, 163, 184));
    g_clear_btn = SGL_DemoApp_CreateButton(g_loop_page,
                                           LOOP_ACTION_COL_X,
                                           LOOP_ACTION_CLEAR_Y,
                                           LOOP_ACTION_BTN_W,
                                           LOOP_ACTION_BTN_H,
                                           "Clear",
                                           sgl_rgb(255, 247, 237),
                                           sgl_rgb(154, 52, 18),
                                           sgl_rgb(251, 146, 60));

    SGL_DemoApp_CreateConfirmDialog(g_loop_page);
    SGL_DemoApp_SetConfirmVisible(0U);
    sgl_screen_load(g_loop_page);
    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleModeSwitch(app_mode_t target_mode)
{
    if(g_app_mode_status.switching != 0U)
    {
        SGL_DemoApp_SetHint("Switching mode...");
        return;
    }

    if(app_mode_get() == target_mode)
    {
        SGL_DemoApp_SetHint((target_mode == APP_MODE_FREE_PLAY) ? "Already in free mode" : "Already in MIDI mode");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    SGL_DemoApp_SetHint("Switching mode...");
    SGL_DemoApp_RefreshLoopUI(1U);

    if(app_mode_switch(target_mode) == 0)
    {
        SGL_DemoApp_SetHint((target_mode == APP_MODE_FREE_PLAY) ? "Free mode ready" : "MIDI mode ready");
    }
    else
    {
        const char *error_text = app_mode_get_last_error();
        SGL_DemoApp_SetHint((error_text != NULL && error_text[0] != '\0') ? error_text : "Mode switch failed");
    }

    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleSlotTap(uint8_t slot_index)
{
    char hint_text[48];

    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下不支持Loop");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if(loop_select_slot(slot_index) == 0)
    {
        (void)snprintf(hint_text, sizeof(hint_text), "Selected slot %u", (unsigned)(slot_index + 1U));
        SGL_DemoApp_SetHint(hint_text);
    }
    else
    {
        SGL_DemoApp_SetHint("Select slot failed");
    }

    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleRecordAction(void)
{
    char hint_text[48];
    uint8_t slot_index = g_loop_ui_state.selected_slot;

    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下不支持录制");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if(slot_index >= LOOP_SLOT_COUNT)
    {
        slot_index = 0U;
    }

    if(loop_ui_start_record() == 0)
    {
        (void)snprintf(hint_text, sizeof(hint_text), "Recording slot %u", (unsigned)(slot_index + 1U));
        SGL_DemoApp_SetHint(hint_text);
    }
    else
    {
        SGL_DemoApp_SetHint("Record start rejected");
    }

    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandlePlayAction(void)
{
    char hint_text[48];
    uint8_t slot_index = g_loop_ui_state.selected_slot;

    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下不支持回放");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if(slot_index >= LOOP_SLOT_COUNT)
    {
        slot_index = 0U;
    }

    if(loop_ui_start_playback() == 0)
    {
        (void)snprintf(hint_text, sizeof(hint_text), "Playing slot %u", (unsigned)(slot_index + 1U));
        SGL_DemoApp_SetHint(hint_text);
    }
    else
    {
        SGL_DemoApp_SetHint("Play start rejected");
    }

    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleStopAction(void)
{
    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下没有可停止Loop");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if(g_loop_ui_state.recording != 0U)
    {
        if(loop_ui_stop_record() == 0)
        {
            SGL_DemoApp_SetHint("Record stopped");
        }
        else
        {
            SGL_DemoApp_SetHint("Record stop failed");
        }
    }
    else if(g_loop_ui_state.playing != 0U)
    {
        if(loop_ui_stop_playback() == 0)
        {
            SGL_DemoApp_SetHint("Playback stopped");
        }
        else
        {
            SGL_DemoApp_SetHint("Playback stop failed");
        }
    }
    else
    {
        SGL_DemoApp_SetHint("Nothing to stop");
    }

    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleClearAction(void)
{
    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下不支持清空Loop");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if((g_loop_ui_state.recording != 0U) || (g_loop_ui_state.playing != 0U))
    {
        SGL_DemoApp_SetHint("Stop active loop first");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    SGL_DemoApp_SetConfirmVisible(1U);
}

static void SGL_DemoApp_HandleConfirmOk(void)
{
    char hint_text[48];
    uint8_t slot_index = g_loop_ui_state.selected_slot;

    if(app_mode_is_loop_enabled() == 0U)
    {
        SGL_DemoApp_SetHint("MIDI模式下不支持清空Loop");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if(slot_index >= LOOP_SLOT_COUNT)
    {
        slot_index = 0U;
    }

    if(loop_ui_clear_selected_slot() == 0)
    {
        (void)snprintf(hint_text, sizeof(hint_text), "Cleared slot %u", (unsigned)(slot_index + 1U));
        SGL_DemoApp_SetHint(hint_text);
    }
    else
    {
        SGL_DemoApp_SetHint("Clear slot failed");
    }

    SGL_DemoApp_SetConfirmVisible(0U);
    SGL_DemoApp_RefreshLoopUI(1U);
}

static void SGL_DemoApp_HandleActionTap(sgl_loop_action_t action)
{
    switch(action)
    {
    case SGL_LOOP_ACTION_RECORD:
        SGL_DemoApp_HandleRecordAction();
        break;
    case SGL_LOOP_ACTION_PLAY:
        SGL_DemoApp_HandlePlayAction();
        break;
    case SGL_LOOP_ACTION_STOP:
        SGL_DemoApp_HandleStopAction();
        break;
    case SGL_LOOP_ACTION_CLEAR:
        SGL_DemoApp_HandleClearAction();
        break;
    case SGL_LOOP_ACTION_CONFIRM_CANCEL:
        SGL_DemoApp_SetConfirmVisible(0U);
        SGL_DemoApp_SetHint("Clear canceled");
        SGL_DemoApp_RefreshLoopUI(1U);
        break;
    case SGL_LOOP_ACTION_CONFIRM_OK:
        SGL_DemoApp_HandleConfirmOk();
        break;
    default:
        break;
    }
}

static void SGL_DemoApp_HandleTouchAppEvent(uint8_t type, uint16_t x, uint16_t y)
{
    int16_t dx;
    int16_t dy;
    int8_t slot_index;
    sgl_loop_action_t action;

    if(type == SGL_DEMO_TOUCH_EVT_DOWN)
    {
        g_touch_active = 1U;
        g_touch_started_on_slots = (g_confirm_visible == 0U) ? SGL_DemoApp_PointInSlotView(x, y) : 0U;
        g_touch_drag_handled = 0U;
        g_slot_press_scroll_offset = g_slot_scroll_offset;
        g_touch_down_x = x;
        g_touch_down_y = y;
        g_last_touch_x = x;
        g_last_touch_y = y;
        return;
    }

    if(type == SGL_DEMO_TOUCH_EVT_MOVE)
    {
        g_last_touch_x = x;
        g_last_touch_y = y;
        dx = (int16_t)x - (int16_t)g_touch_down_x;
        dy = (int16_t)y - (int16_t)g_touch_down_y;

        if((g_touch_started_on_slots != 0U) &&
           (app_mode_is_loop_enabled() != 0U) &&
           ((g_touch_drag_handled != 0U) ||
            ((sgl_abs(dy) >= TOUCH_SWIPE_THRESHOLD) &&
             (sgl_abs(dy) > sgl_abs(dx)))))
        {
            SGL_DemoApp_SyncSlotScrollToTouch();
            g_touch_drag_handled = 1U;
        }
        return;
    }

    if(type != SGL_DEMO_TOUCH_EVT_UP)
    {
        return;
    }

    if(g_touch_active == 0U)
    {
        return;
    }

    g_touch_active = 0U;
    g_touch_started_on_slots = 0U;
    dx = (int16_t)g_last_touch_x - (int16_t)g_touch_down_x;
    dy = (int16_t)g_last_touch_y - (int16_t)g_touch_down_y;
    if(g_touch_drag_handled != 0U)
    {
        g_touch_drag_handled = 0U;
        SGL_DemoApp_SetHint("Slot list scrolled");
        SGL_DemoApp_RefreshLoopUI(1U);
        return;
    }

    if((sgl_abs(dx) > TOUCH_TAP_MOVE_THRESHOLD) || (sgl_abs(dy) > TOUCH_TAP_MOVE_THRESHOLD))
    {
        return;
    }

    if(g_confirm_visible != 0U)
    {
        action = SGL_DemoApp_HitTestAction(g_last_touch_x, g_last_touch_y);
        if((action == SGL_LOOP_ACTION_CONFIRM_CANCEL) || (action == SGL_LOOP_ACTION_CONFIRM_OK))
        {
            SGL_DemoApp_HandleActionTap(action);
        }
        return;
    }

    action = SGL_DemoApp_HitTestAction(g_last_touch_x, g_last_touch_y);
    if(action == SGL_LOOP_ACTION_MODE_FREE)
    {
        SGL_DemoApp_HandleModeSwitch(APP_MODE_FREE_PLAY);
        return;
    }

    if(action == SGL_LOOP_ACTION_MODE_MIDI)
    {
        SGL_DemoApp_HandleModeSwitch(APP_MODE_MIDI_KEYBOARD);
        return;
    }

    slot_index = SGL_DemoApp_HitTestSlot(g_last_touch_x, g_last_touch_y);
    if(slot_index >= 0)
    {
        SGL_DemoApp_HandleSlotTap((uint8_t)slot_index);
        return;
    }

    action = SGL_DemoApp_HitTestAction(g_last_touch_x, g_last_touch_y);
    if((action != SGL_LOOP_ACTION_NONE) &&
       (action != SGL_LOOP_ACTION_MODE_FREE) &&
       (action != SGL_LOOP_ACTION_MODE_MIDI))
    {
        SGL_DemoApp_HandleActionTap(action);
    }
}

void SGL_DemoApp_Init(void)
{
    sgl_fbinfo_t fbinfo = {
        .xres = LCD_W,
        .yres = LCD_H,
        .flush_area = SGL_DemoApp_FlushArea,
        .buffer = {g_panel_buffer0, g_panel_buffer1},
        .buffer_size = SGL_ARRAY_SIZE(g_panel_buffer0)
    };

    ILI9341_Init();
    FT6336_Touch_Init();
    SGL_DemoApp_Tim2Init(100 - 1, 1440 - 1);
    SGL_DemoApp_Tim3Init(200 - 1, 1440 - 1);

    sgl_fbdev_register(&fbinfo);
    sgl_init();
    SGL_DemoApp_SetupUI();
}

void SGL_DemoApp_Loop(void)
{
    static TickType_t last_scan_tick = 0;
    TickType_t now_tick;
    TickType_t scan_period_ticks;
    uint16_t x = 0;
    uint16_t y = 0;
    bool pressed = false;

    now_tick = xTaskGetTickCount();

    if(g_touch_scan_pending != 0U)
    {
        g_touch_scan_pending = 0U;
        scan_period_ticks = 0U;
    }
    else
    {
        scan_period_ticks = pdMS_TO_TICKS(g_touch_active != 0U ? TOUCH_SCAN_PERIOD_MS : TOUCH_IDLE_POLL_PERIOD_MS);
    }

    if((scan_period_ticks == 0U) || ((TickType_t)(now_tick - last_scan_tick) >= scan_period_ticks))
    {
        last_scan_tick = now_tick;

        if(FT6336_Touch_ReadState(&x, &y, &pressed))
        {
            if(pressed)
            {
                if(g_touch_active == 0U)
                {
                    SGL_DemoApp_HandleTouchAppEvent(SGL_DEMO_TOUCH_EVT_DOWN, x, y);
                }
                else
                {
                    SGL_DemoApp_HandleTouchAppEvent(SGL_DEMO_TOUCH_EVT_MOVE, x, y);
                }
            }
            else if(g_touch_active != 0U)
            {
                SGL_DemoApp_HandleTouchAppEvent(SGL_DEMO_TOUCH_EVT_UP, g_last_touch_x, g_last_touch_y);
            }
        }
    }

    SGL_DemoApp_RefreshLoopUI(0U);
    sgl_task_handler();
    sgl_timer_handler();
}

void SGL_DemoApp_TickISR(void)
{
    sgl_tick_inc(1);
}

void SGL_DemoApp_TouchScanISR(void)
{
    g_touch_scan_pending = 1U;
}

void SGL_DemoApp_LcdDmaISR(void)
{
    ILI9341_FlushCompleteISR();
}

/* SGL 屏幕任务优先级和栈大小（与音频混音任务同级，先保证界面能启动显示） */
#define SGL_TASK_PRIO       8
#define SGL_TASK_STK_SIZE   1024

/*********************************************************************
 * @fn      sgl_task
 *
 * @brief   SGL 屏幕任务：初始化 UI 后循环处理触摸和渲染
 *
 * @param   pvParameters - FreeRTOS 任务参数（未使用）
 *
 * @return  none
 */
static void sgl_task(void *pvParameters)
{
    (void)pvParameters;
    SGL_DemoApp_Init();

    while(1)
    {
        SGL_DemoApp_Loop();
        vTaskDelay(1);
    }
}

/*********************************************************************
 * @fn      SGL_SystemInit
 *
 * @brief   创建 SGL 屏幕显示 FreeRTOS 任务
 *
 * @return  none
 */
void SGL_SystemInit(void)
{
    xTaskCreate((TaskFunction_t )sgl_task,
                (const char*    )"sgl",
                (uint16_t       )SGL_TASK_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SGL_TASK_PRIO,
                (TaskHandle_t*  )&SGLTask_Handler);
}
