#include "effect.h"
#include <FastLED.h>

#pragma region 亮度相关设置 最终更新para.L_actual==================

void set_L_Level(uint8_t level, LX_PARA_T &para) {
    if (level > 100) level = 100;
    para.lx_L_level = level;
    uint8_t k = (level * para.lx_L_top_limit) / 100;
    para.lx_L_actual = map(k, 0, 100, 0, 255);
}

void set_L_Level(uint8_t level, XK_PARA_T &para) {
    if (level > 100) level = 100;
    para.xk_L_level = level;
    uint8_t k = (level * para.xk_L_top_limit) / 100;
    para.xk_L_actual = map(k, 0, 100, 0, 255);
}

void set_L_top(uint8_t maxVal, LX_PARA_T &para) {
    if (maxVal > 100) maxVal = 100;
    para.lx_L_top_limit = maxVal;
    set_L_Level(para.lx_L_level, para);
}

void set_L_top(uint8_t maxVal, XK_PARA_T &para) {
    if (maxVal > 100) maxVal = 100;
    para.xk_L_top_limit = maxVal;
    set_L_Level(para.xk_L_level, para);
}

#pragma endregion ============================

#pragma region 颜色相关设置 ==================

// 设置XK设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    
    for (int i = 0; i < para.xk_led_num; i++) {
        para.xk_custom_colors[i] = originalColor;
    }
}

// 设置LX设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    para.lx_custom_color = originalColor;
}

// 设置XK设备单个LED颜色（不考虑亮度）
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    if (index < para.xk_led_num) {
        CRGB originalColor(r, g, b);
        para.xk_custom_colors[index] = originalColor;
    }
}


#pragma endregion ============================


#pragma region 效果 ==========================

BW_BREATH_PARA_T bw_para;

void xk_bw_breath_init()
{
    bw_para.s_period = xk_para.xk_breath_speed % 6 * 1000;
    bw_para.start_ms = (uint32_t)millis();
}

void blue_white_breath_effect() {
    static uint8_t period = (xk_para.xk_breath_speed) % 6 * 1000;

}

#pragma endregion ============================





void xk_effect()
{
    switch (xk_para.xk_mode)
    {
        case MODE_STATIC:
            // 静态模式效果处理
            for(int i = 0; i < XK_LED_NUM; i++) 
            {
                xk_para.xk_end_colors[i] = xk_para.xk_custom_colors[i];//这个数组里我只保存了收到的rgb
                xk_para.xk_end_colors[i].nscale8_video(xk_para.xk_L_actual);//L_actual是算好了的0-255
            }
            break;
        case MODE_BREATH_BW:
            // 蓝白呼吸效果处理
            blue_white_breath_effect();
            break;
        case MODE_BREATH_GAP:
            // 间隔灯珠呼吸效果处理
            break;
        case MODE_MUSIC:
            // 跟随音乐节奏效果处理
            break;
        case MODE_WELCOME_1:
        case MODE_WELCOME_2:
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
    //end -> leds
    for(int i = 0; i < XK_LED_NUM; i++) 
    {
        xk_leds[i] = xk_para.xk_end_colors[i];
    }
}
void lx_effect()
{

}   

void _10_ms_task(void)
{
    //开始之前看看开关之类的再刷灯
    xk_effect();
    lx_effect();
    FastLED.show();
    //试了 准的
    // uint32_t now = millis();
    // serial.printBothln("当前时间(ms): " + String(now));
}

