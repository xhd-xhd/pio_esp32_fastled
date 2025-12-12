#pragma once
#include "SerialManager.h"
#include <FastLED.h>
#include <Ticker.h>
#include "stdint.h"


//暂时这三个宏定义是写死的
#define NUM_LEDS    80
#define XK_NUM_LEDS    40
#define LX_NUM_LEDS    40
#define TOP_BRIGHTNESS  100
#define LED_PIN     2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


enum SWITCH_STATE {
    ON = 0,
    OFF = 1
};
enum AUDIO_SOURCE {
    MICROPHONE = 0,
    HORN = 1
};
enum CONTROL_SOURCE {
    APP_ONLY = 0,
    APP_and_CAR = 1
};

typedef struct {
    //蓝牙名字，设备型号版本，从硬件读取数据到app --- 待定
    uint32_t turn_off_delay;
    uint8_t music_sens;
    SWITCH_STATE switch_state;
    AUDIO_SOURCE audio_source;
    CONTROL_SOURCE control_source;
} GENERAL_PARA_T;



// 星空灯模式定义
enum XK_LEDMode_E {
    MODE_STATIC = 1,             // 单色
    MODE_BREATH_BW = 2,          // 蓝白呼吸
    MODE_BREATH_GAP = 3,         // 间隔灯珠呼吸
    MODE_MUSIC = 4,              // 跟随音乐节奏
    MODE_WELCOME = 5,          // 迎宾
    MODE_BREATH_DEFAULT = 6,     // 单色呼吸
    MODE_BREATH_RANDOM = 7,      // 随机呼吸
};

// 流星模式定义
enum LX_LEDMode_E {
    MODE_DEFAULT = 1,            // 默认效果
    MODE_RANDOM = 2,             // 随机效果
    MODE_DOUBLE = 3,             // 分两段
};



typedef struct {
    SWITCH_STATE xk_switch_state;      //星空灯开关
    SWITCH_STATE wlcome_switch_state;  //迎宾开关 用于上电判断跑不跑效果 另外两条信号直接触发/结束效果，不用参数
    XK_LEDMode_E xk_mode;
    XK_LEDMode_E xk_last_mode;          //用于记忆功能，保存上次模式
    CRGB xk_custom_colors[NUM_LEDS];    //自定义颜色数组，如果下电前是自定义颜色模式，则保存当前颜色

    uint8_t xk_led_num;
    uint8_t xk_L_top_limit;
    uint8_t xk_L_level;
    uint8_t xk_breath_speed;
} XK_PARA_T;

typedef struct {
    SWITCH_STATE lx_switch_state;      //流星灯开关
    LX_LEDMode_E lx_mode;
    LX_LEDMode_E lx_last_mode;          //用于记忆功能，保存上次模式
    CRGB lx_last_color;
    
    uint8_t lx_led_num;
    uint8_t lx_L_top_limit;
    uint8_t lx_L_level;
    uint8_t lx_total_gap;
    uint8_t lx_1_sepped;
    uint8_t lx_1_dir;
    uint8_t lx_1_2_gap;
    uint8_t lx_2_sepped;
    uint8_t lx_2_dir;
} LX_PARA_T;


extern SerialManager serial;
extern CRGB leds[NUM_LEDS];

extern GENERAL_PARA_T general_para;
extern XK_PARA_T xk_para;
extern LX_PARA_T lx_para;

extern Ticker periodicTimer;      // 周期定时器
extern Ticker onceTimer;          // 单次定时器

void fast_led_init();





