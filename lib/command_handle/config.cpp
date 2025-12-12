#include "config.h"

Ticker periodicTimer;      // 周期定时器
Ticker onceTimer;          // 单次定时器
//periodicTimer.attach_ms(10, periodicCallback);
//onceTimer.once_ms(30, onceCallback);


SerialManager serial;
CRGB leds[NUM_LEDS];



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
    .xk_led_num = XK_NUM_LEDS,
    .xk_L_top_limit = TOP_BRIGHTNESS,
    .xk_L_level = TOP_BRIGHTNESS,
    .xk_breath_speed = 50
};

LX_PARA_T lx_para = {
    .lx_switch_state = OFF,
    .lx_mode = MODE_DEFAULT,
    .lx_last_mode = MODE_DEFAULT,
    .lx_led_num = LX_NUM_LEDS,
    .lx_L_top_limit = TOP_BRIGHTNESS,
    .lx_L_level = TOP_BRIGHTNESS,
    .lx_total_gap = 5,
    .lx_1_sepped = 5,
    .lx_1_dir = 0,
    .lx_1_2_gap = 5,
    .lx_2_sepped = 5,
    .lx_2_dir = 0
};


void fast_led_init() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    //初始化亮度应该给默认值，后面可根据记忆功能调整到用户上次设置的亮度
    FastLED.setBrightness(TOP_BRIGHTNESS);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}


