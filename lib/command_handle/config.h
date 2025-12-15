#pragma once
#include "SerialManager.h"
#include <FastLED.h>
#include <Ticker.h>
#include "stdint.h"


//暂时这三个宏定义是写死的
#define XK_LED_NUM    40
#define LX_LED_NUM    40
#define TOP_BRIGHTNESS  100
#define XK_LED_PIN     2
#define LX_LED_PIN     3
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
    MODE_WELCOME_1 = 5,          // 迎宾 60s自动退出
    MODE_WELCOME_2 = 6,          // 迎宾 收到结束信号退出
    MODE_BREATH_DEFAULT = 7,     // 单色呼吸
    MODE_BREATH_RANDOM = 8,      // 随机呼吸
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
    CRGB xk_custom_colors[XK_LED_NUM];  //全局/自定义 颜色修改此数组，静态模式刷这个数组，nvs保存一份
    CRGB xk_end_colors[XK_LED_NUM];     //星空最终数据buffer

    uint8_t xk_led_num;
    uint8_t xk_L_top_limit;
    uint8_t xk_L_level;                 //收到的亮度 0-100
    uint8_t xk_L_actual;                //实际亮度 0-255
    uint8_t xk_breath_speed;
    uint8_t xk_welcome_s;
} XK_PARA_T;

typedef struct {
    SWITCH_STATE lx_switch_state;       //流星灯开关
    LX_LEDMode_E lx_mode;
    LX_LEDMode_E lx_last_mode;          //用于记忆功能，保存上次模式
    CRGB lx_custom_color;               //流星颜色修改此变量，nvs保存一份
    
    uint8_t lx_led_num;
    uint8_t lx_L_top_limit;
    uint8_t lx_L_level;                 //收到的亮度 0-100
    uint8_t lx_L_actual;                //实际亮度 0-255
    uint8_t lx_total_gap;
    uint8_t lx_1_sepped;
    uint8_t lx_1_dir;
    uint8_t lx_1_2_gap;
    uint8_t lx_2_sepped;
    uint8_t lx_2_dir;
} LX_PARA_T;


extern SerialManager serial;

extern CLEDController* xk_strip;
extern CLEDController* lx_strip;

extern CRGB xk_leds[XK_LED_NUM];
extern CRGB lx_leds[LX_LED_NUM];

extern GENERAL_PARA_T general_para;
extern XK_PARA_T xk_para;
extern LX_PARA_T lx_para;

extern Ticker periodicTimer;      // 周期定时器
extern Ticker onceTimer;          // 单次定时器

void fast_led_init();





