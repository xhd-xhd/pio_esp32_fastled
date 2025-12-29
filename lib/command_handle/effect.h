#pragma once

#include "config.h"

typedef struct
{
    uint8_t speed;
    uint32_t s_period;
    uint32_t bw_start_ms;
    uint32_t gap_start_ms;
    uint32_t static_start_ms;
    uint32_t random_start_ms;
    uint32_t random_num;
} BREATH_PARA_T;//星空 呼吸效果参数 结构体

typedef struct
{
    uint8_t dummy_num;          //一个完整流星帧的前后添加的空led
    uint8_t random_light_num;   //随机流星 亮几个？
    uint8_t tail_len;           //拖尾长度
} LX_EFFECT_PARA_T;//流星 参数 结构体



void _10_ms_task(void);

//effect api
void set_L_Level(uint8_t level, LX_PARA_T &para);
void set_L_Level(uint8_t level, XK_PARA_T &para);
void set_L_top(uint8_t maxVal, LX_PARA_T &para);
void set_L_top(uint8_t maxVal, XK_PARA_T &para);

void set_color(uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para);
void set_color(uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para);
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para);

void xk_bw_breath_init();
void gap_breath_init();
void static_breath_init();
void random_breath_init();
