#include "config.h"

Ticker periodicTimer;      // 周期定时器
Ticker onceTimer;          // 单次定时器
//periodicTimer.attach_ms(10, periodicCallback);
//onceTimer.once_ms(30, onceCallback);


SerialManager serial;
CLEDController* xk_strip;
CLEDController* lx_strip;

CRGB xk_leds[XK_LED_NUM];
CRGB lx_leds[LX_LED_NUM];


GENERAL_PARA_T general_para = {
    .turn_off_delay = 0,
    .music_sens = 5,
    .switch_state = OFF,
    .audio_source = MICROPHONE,
    .control_source = APP_and_CAR
};

XK_PARA_T xk_para = {
    .xk_switch_state = OFF,
    .wlcome_switch_state = OFF,
    .xk_mode = MODE_STATIC,
    .xk_last_mode = MODE_STATIC,
    .xk_led_num = XK_LED_NUM,
    .xk_L_top_limit = TOP_BRIGHTNESS,
    .xk_L_level = TOP_BRIGHTNESS,
    .xk_breath_speed = 50,
    .xk_welcome_s = 5
};

LX_PARA_T lx_para = {
    .lx_switch_state = OFF,
    .lx_mode = MODE_DEFAULT,
    .lx_last_mode = MODE_DEFAULT,
    .lx_led_num = LX_LED_NUM,
    .lx_L_top_limit = TOP_BRIGHTNESS,
    .lx_L_level = TOP_BRIGHTNESS,
    .lx_total_gap = 3,
    .lx_1_sepped = 5,
    .lx_1_dir = 0,
    .lx_1_2_gap = 5,
    .lx_2_sepped = 5,
    .lx_2_dir = 0
};


void fast_led_init() {
    // 创建独立控制器
    // xk_strip = &FastLED.addLeds<LED_TYPE, XK_LED_PIN, COLOR_ORDER>(xk_leds, XK_LED_NUM);
    lx_strip = &FastLED.addLeds<LED_TYPE, XK_LED_PIN, COLOR_ORDER>(lx_leds, LX_LED_NUM);
}


