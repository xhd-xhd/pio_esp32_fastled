#pragma once

#include "config.h"

void _10_ms_task(void);

//effect api
void set_L_Level(uint8_t level, LX_PARA_T &para);
void set_L_Level(uint8_t level, XK_PARA_T &para);
void set_L_top(uint8_t maxVal, LX_PARA_T &para);
void set_L_top(uint8_t maxVal, XK_PARA_T &para);

void set_color(uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para);
void set_color(uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para);
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para);
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para);




