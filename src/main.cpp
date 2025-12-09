#include <FastLED.h>
#include "BluetoothSerial.h"

#define LED_PIN     2
#define NUM_LEDS    78
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
BluetoothSerial SerialBT;

// 存储当前颜色和模式
CRGB currentColor = CRGB::Black;
uint8_t currentBrightness = BRIGHTNESS;
uint8_t currentMode = 0;
uint8_t hue = 0;
uint8_t speed = 50;
boolean isAnimating = false;

// 模式定义
enum LEDMode {
    MODE_STATIC = 0,      // 静态颜色
    MODE_RAINBOW = 1,     // 彩虹渐变
    MODE_PULSE = 2,       // 脉冲效果
    MODE_WAVE = 3,        // 波浪扫描
    MODE_STROBE = 4,      // 频闪
    MODE_TWINKLE = 5,     // 闪烁
    MODE_FIRE = 6,        // 火焰效果
    MODE_CHASE = 7,       // 追逐效果
    MODE_COLORFUL = 8     // 彩色循环
};

// 时间戳
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 50;

// func declaration
void handleCommand(String cmd);
void displayMenu();
void updateLED();
void rainbowMode();
void pulseMode();
void waveMode();
void strobeMode();
void twinkleMode();
void fireMode();
void chaseMode();
void colorfulMode();

void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(1000);
    
    // 初始化蓝牙
    if(!SerialBT.begin("ESP32_LED")) {
        Serial.println("❌ 蓝牙启动失败！");
    } else {
        Serial.println("✔ 蓝牙已启动！设备名称：ESP32_LED");
    }

    // 初始化 LED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(currentBrightness);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║     ESP32 高级 LED 灯条控制器      ║");
    Serial.println("╚════════════════════════════════════╝");
    displayMenu();
}

void loop() {
    // 处理串口输入
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        handleCommand(cmd);
    }

    // 处理蓝牙输入
    if (SerialBT.available()) {
        String cmd = SerialBT.readStringUntil('\n');
        cmd.trim();
        handleCommand(cmd);
    }

    // 更新LED动画
    if(millis() - lastUpdateTime >= updateInterval) {
        updateLED();
        lastUpdateTime = millis();
    }
}

// 解析命令函数
void handleCommand(String cmd) {
    Serial.print("→ 命令: ");
    Serial.println(cmd);
    cmd.toUpperCase();

    if(cmd == "MENU") {
        displayMenu();
    } 
    else if(cmd == "OFF") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Black;
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        Serial.println("✔ LED 已关闭");
    } 
    else if(cmd == "RED") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Red;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯红色");
    } 
    else if(cmd == "GREEN") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Green;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯绿色");
    } 
    else if(cmd == "BLUE") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Blue;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯蓝色");
    }
    else if(cmd == "WHITE") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::White;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯白色");
    }
    else if(cmd == "YELLOW") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Yellow;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯黄色");
    }
    else if(cmd == "CYAN") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Cyan;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯青色");
    }
    else if(cmd == "MAGENTA") {
        currentMode = MODE_STATIC;
        currentColor = CRGB::Magenta;
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
        Serial.println("✔ 纯品红色");
    }
    else if(cmd == "1") {
        currentMode = MODE_RAINBOW;
        isAnimating = true;
        Serial.println("✔ 切换到彩虹渐变模式");
    }
    else if(cmd == "2") {
        currentMode = MODE_PULSE;
        isAnimating = true;
        Serial.println("✔ 切换到脉冲模式");
    }
    else if(cmd == "3") {
        currentMode = MODE_WAVE;
        isAnimating = true;
        Serial.println("✔ 切换到波浪模式");
    }
    else if(cmd == "4") {
        currentMode = MODE_STROBE;
        isAnimating = true;
        Serial.println("✔ 切换到频闪模式");
    }
    else if(cmd == "5") {
        currentMode = MODE_TWINKLE;
        isAnimating = true;
        Serial.println("✔ 切换到闪烁模式");
    }
    else if(cmd == "6") {
        currentMode = MODE_FIRE;
        isAnimating = true;
        Serial.println("✔ 切换到火焰模式");
    }
    else if(cmd == "7") {
        currentMode = MODE_CHASE;
        isAnimating = true;
        Serial.println("✔ 切换到追逐模式");
    }
    else if(cmd == "8") {
        currentMode = MODE_COLORFUL;
        isAnimating = true;
        Serial.println("✔ 切换到彩色循环模式");
    }
    else if(cmd.startsWith("SPEED")) {
        int spaceIndex = cmd.indexOf(' ');
        if(spaceIndex != -1) {
            String valueStr = cmd.substring(spaceIndex + 1);
            int value = valueStr.toInt();
            if(value >= 1 && value <= 100) {
                speed = value;
                updateInterval = 150 - (speed * 1.4);
                Serial.print("✔ 速度设置为: ");
                Serial.println(speed);
            } else {
                Serial.println("❌ 速度值范围: 1-100");
            }
        }
    }
    else if(cmd.startsWith("BRIGHT")) {
        int spaceIndex = cmd.indexOf(' ');
        if(spaceIndex != -1) {
            String valueStr = cmd.substring(spaceIndex + 1);
            int value = valueStr.toInt();
            if(value >= 1 && value <= 255) {
                currentBrightness = value;
                FastLED.setBrightness(currentBrightness);
                FastLED.show();
                Serial.print("✔ 亮度设置为: ");
                Serial.println(currentBrightness);
            } else {
                Serial.println("❌ 亮度值范围: 1-255");
            }
        }
    }
    else if(cmd.startsWith("HUE")) {
        int spaceIndex = cmd.indexOf(' ');
        if(spaceIndex != -1) {
            String valueStr = cmd.substring(spaceIndex + 1);
            int value = valueStr.toInt();
            if(value >= 0 && value <= 255) {
                hue = value;
                Serial.print("✔ 色调设置为: ");
                Serial.println(hue);
            } else {
                Serial.println("❌ 色调值范围: 0-255");
            }
        }
    }
    else {
        Serial.println("❌ 未知命令，输入 MENU 查看帮助");
    }
}

void displayMenu() {
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║        命令菜单 (Command Menu)     ║");
    Serial.println("╠════════════════════════════════════╣");
    Serial.println("║ 基础颜色:                          ║");
    Serial.println("║  RED, GREEN, BLUE, WHITE           ║");
    Serial.println("║  YELLOW, CYAN, MAGENTA, OFF        ║");
    Serial.println("╠════════════════════════════════════╣");
    Serial.println("║ 高级动画模式:                      ║");
    Serial.println("║  1 - 彩虹渐变 (RAINBOW)            ║");
    Serial.println("║  2 - 脉冲 (PULSE)                  ║");
    Serial.println("║  3 - 波浪 (WAVE)                   ║");
    Serial.println("║  4 - 频闪 (STROBE)                 ║");
    Serial.println("║  5 - 闪烁 (TWINKLE)                ║");
    Serial.println("║  6 - 火焰 (FIRE)                   ║");
    Serial.println("║  7 - 追逐 (CHASE)                  ║");
    Serial.println("║  8 - 彩色循环 (COLORFUL)           ║");
    Serial.println("╠════════════════════════════════════╣");
    Serial.println("║ 控制参数:                          ║");
    Serial.println("║  SPEED <1-100>  - 动画速度         ║");
    Serial.println("║  BRIGHT <1-255> - LED亮度          ║");
    Serial.println("║  HUE <0-255>    - 色调值           ║");
    Serial.println("║  MENU           - 显示此菜单       ║");
    Serial.println("╚════════════════════════════════════╝\n");
}

void updateLED() {
    switch(currentMode) {
        case MODE_RAINBOW:
            rainbowMode();
            break;
        case MODE_PULSE:
            pulseMode();
            break;
        case MODE_WAVE:
            waveMode();
            break;
        case MODE_STROBE:
            strobeMode();
            break;
        case MODE_TWINKLE:
            twinkleMode();
            break;
        case MODE_FIRE:
            fireMode();
            break;
        case MODE_CHASE:
            chaseMode();
            break;
        case MODE_COLORFUL:
            colorfulMode();
            break;
        default:
            break;
    }
    FastLED.show();
}

void rainbowMode() {
    fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
    hue += (speed / 50.0);
}

void pulseMode() {
    static uint8_t brightness = 0;
    static int8_t direction = 1;
    
    CRGB pulsedColor = currentColor;
    uint8_t pulseVal = (sin8(hue) * brightness) / 255;
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue, 255, pulseVal);
    }
    
    hue += (speed / 50.0);
}

void waveMode() {
    for(int i = 0; i < NUM_LEDS; i++) {
        uint8_t waveBright = sin8(hue + (i * 255 / NUM_LEDS));
        leds[i] = CHSV(hue, 200, waveBright);
    }
    hue += (speed / 50.0);
}

void strobeMode() {
    static unsigned long strobeTimer = 0;
    static boolean isOn = true;
    
    if(millis() - strobeTimer > (150 - speed)) {
        isOn = !isOn;
        strobeTimer = millis();
    }
    
    if(isOn) {
        fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    } else {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
}

void twinkleMode() {
    for(int i = 0; i < NUM_LEDS; i++) {
        if(random8() < (speed * 2)) {
            leds[i] = CHSV(hue + random8(), 150, 255);
        } else {
            leds[i].nscale8(200);
        }
    }
}

void fireMode() {
    // 简化的火焰效果
    for(int i = 0; i < NUM_LEDS; i++) {
        uint8_t heat = random8(150, 255);
        
        if(i < NUM_LEDS / 3) {
            leds[i] = HeatColor(heat);
        } else if(i < (NUM_LEDS * 2) / 3) {
            leds[i] = HeatColor(heat - 30);
        } else {
            leds[i] = HeatColor(heat - 60);
        }
    }
}

void chaseMode() {
    static uint8_t pos = 0;
    
    fadeToBlackBy(leds, NUM_LEDS, 20);
    leds[pos] = CHSV(hue, 255, 255);
    if(pos > 0) leds[pos - 1] = CHSV(hue, 200, 200);
    
    pos = (pos + 1) % NUM_LEDS;
    hue += (speed / 100.0);
}

void colorfulMode() {
    static uint8_t colorPos = 0;
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((colorPos + i * 256 / NUM_LEDS) % 256, 255, 255);
    }
    
    colorPos += (speed / 50.0);
}
