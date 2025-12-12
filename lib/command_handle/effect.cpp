#include "effect.h"
#include <FastLED.h>

#pragma region 亮度相关设置 ==================

uint8_t get_actual_L(const LX_PARA_T &para) {
    return (para.lx_L_level * para.lx_L_top_limit) / 100;
}

uint8_t get_actual_L(const XK_PARA_T &para) {
    return (para.xk_L_level * para.xk_L_top_limit) / 100;
}

//暂定星空和流星共用同一条LED带 且连续 且星空在前 流星在后
void updateActual_L(const LX_PARA_T &para) {
    uint8_t actual = get_actual_L(para);
    for (int i = XK_NUM_LEDS; i < XK_NUM_LEDS + para.lx_led_num; i++) {
        leds[i] = leds[i].nscale8(actual);
    }
    FastLED.show();
}

void updateActual_L(const XK_PARA_T &para) {
    uint8_t actual = get_actual_L(para);
    for (int i = 0; i < para.xk_led_num; i++) {
        leds[i] = leds[i].nscale8(actual);
    }
    FastLED.show();
}

void set_L_Level(uint8_t level, LX_PARA_T &para) {
    if (level > 100) level = 100;
    para.lx_L_level = level;
    updateActual_L(para);
}

void set_L_Level(uint8_t level, XK_PARA_T &para) {
    if (level > 100) level = 100;
    para.xk_L_level = level;
    updateActual_L(para);
}

void set_L_top(uint8_t maxVal, LX_PARA_T &para) {
    para.lx_L_top_limit = maxVal;
    updateActual_L(para);
}

void set_L_top(uint8_t maxVal, XK_PARA_T &para) {
    para.xk_L_top_limit = maxVal;
    updateActual_L(para);
}


#pragma endregion ============================

#pragma region 颜色相关设置 ==================

// 设置XK设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    
    for (int i = 0; i < para.xk_led_num; i++) {
        para.xk_custom_colors[i] = originalColor;
        leds[i] = originalColor;  // 直接使用原始颜色，不应用亮度
    }
    FastLED.show();
}

// 设置LX设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    
    for (int i = XK_NUM_LEDS; i < XK_NUM_LEDS + para.lx_led_num; i++) {
        para.lx_last_color = originalColor;
        leds[i] = originalColor;  // 直接使用原始颜色，不应用亮度
    }
    FastLED.show();
}

// 设置XK设备单个LED颜色（不考虑亮度）
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    if (index < para.xk_led_num) {
        CRGB originalColor(r, g, b);
        para.xk_custom_colors[index] = originalColor;
        leds[index] = originalColor;  // 直接使用原始颜色
        FastLED.show();
    }
}

// 设置LX设备单个LED颜色（不考虑亮度）
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para) {
    uint16_t actualIndex = XK_NUM_LEDS + index;
    if (index < para.lx_led_num) {
        CRGB originalColor(r, g, b);
        para.lx_last_color = originalColor;
        leds[actualIndex] = originalColor;  // 直接使用原始颜色
        FastLED.show();
    }
}

#pragma endregion ============================



void xk_effect()
{
    switch (xk_para.xk_mode)
    {
        case MODE_STATIC:
            // 静态模式效果处理
            break;
        case MODE_BREATH_BW:
            // 蓝白呼吸效果处理
            break;
        case MODE_BREATH_GAP:
            // 间隔灯珠呼吸效果处理
            break;
        case MODE_MUSIC:
            // 跟随音乐节奏效果处理
            break;
        case MODE_WELCOME:
            // 迎宾效果处理
            break;
        case MODE_BREATH_DEFAULT:
            // 单色呼吸效果处理
            break;
        case MODE_BREATH_RANDOM:
            // 随机呼吸效果处理
            break;
        default:
            break;
    }
}
void lx_effect()
{

}   

void _10_ms_task(void)
{
    xk_effect();
    lx_effect();
    FastLED.show();
}

