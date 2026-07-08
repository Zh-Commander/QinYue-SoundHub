#ifndef __DEMO_SLIDING_MENU_H__
#define __DEMO_SLIDING_MENU_H__

#include "sgl.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct demo_sliding_menu demo_sliding_menu_t;
typedef void (*demo_sliding_menu_click_cb_t)(int index, const char *text);

typedef enum {
    DEMO_SLIDING_MENU_HORIZONTAL = 0,
    DEMO_SLIDING_MENU_VERTICAL,
} demo_sliding_menu_direction_t;

demo_sliding_menu_t *DemoSlidingMenu_Create(sgl_obj_t *parent,
                                            int16_t x,
                                            int16_t y,
                                            int16_t width,
                                            int16_t height,
                                            const char * const *texts,
                                            uint8_t item_count,
                                            demo_sliding_menu_direction_t direction,
                                            const sgl_font_t *font,
                                            demo_sliding_menu_click_cb_t click_cb);

void DemoSlidingMenu_Delete(demo_sliding_menu_t *menu);
void DemoSlidingMenu_SetItemColor(demo_sliding_menu_t *menu, uint8_t index, sgl_color_t color);
void DemoSlidingMenu_SetVisible(demo_sliding_menu_t *menu, bool visible);
void DemoSlidingMenu_SetCurrentIndex(demo_sliding_menu_t *menu, uint8_t index);
void DemoSlidingMenu_MoveLeft(demo_sliding_menu_t *menu);
void DemoSlidingMenu_MoveRight(demo_sliding_menu_t *menu);
void DemoSlidingMenu_MoveUp(demo_sliding_menu_t *menu);
void DemoSlidingMenu_MoveDown(demo_sliding_menu_t *menu);
bool DemoSlidingMenu_HandleTouch(demo_sliding_menu_t *menu, uint8_t type, uint16_t x, uint16_t y);
sgl_obj_t *DemoSlidingMenu_GetRoot(demo_sliding_menu_t *menu);

#ifdef __cplusplus
}
#endif

#endif
