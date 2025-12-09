#include <FastLED.h>
#include "SerialManager.h"

#define LED_PIN     2
#define NUM_LEDS    78
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
SerialManager serial;

// 存储当前颜色和模式
CRGB currentColor = CRGB::Black;
uint8_t currentBrightness = BRIGHTNESS;
uint8_t currentMode = 0;
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
void handleCommand(Frame frame);
void handleColorCommand(uint8_t colorType, uint8_t param);
void handleModeCommand(uint8_t modeType, uint8_t param1, uint8_t param2);
void handleParameterCommand(uint8_t paramType, uint16_t value, uint8_t extra);
void handleSystemCommand(uint8_t sysCmd);
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
    delay(100);
    
    // 打印芯片和 Flash 信息
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║        ESP32 芯片信息              ║");
    Serial.println("╠════════════════════════════════════╣");
    Serial.print("║ Flash 大小: ");
    uint32_t flashSize = ESP.getFlashChipSize();
    Serial.print(flashSize / (1024 * 1024));
    Serial.println(" MB");
    Serial.print("║ Flash 速率: ");
    Serial.print(ESP.getFlashChipSpeed() / 1000000);
    Serial.println(" MHz");
    Serial.print("║ 芯片型号: ");
    Serial.print("ESP32 (Rev ");
    Serial.print(ESP.getChipRevision());
    Serial.println(")");
    Serial.print("║ 核心数: ");
    Serial.print(ESP.getChipCores());
    Serial.println(" cores");
    Serial.print("║ 可用内存: ");
    Serial.print(ESP.getFreeHeap() / 1024);
    Serial.println(" KB");
    Serial.println("╚════════════════════════════════════╝\n");
    
    // 初始化通信管理器
    serial.begin("ESP32_LED");

    // 初始化 LED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(currentBrightness);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║     ESP32 高级 LED 灯条控制器      ║");
    Serial.println("║     帧协议版本 (Frame Protocol)   ║");
    Serial.println("╚════════════════════════════════════╝");
    displayMenu();
}

void loop() {
    // 检查是否有完整的帧接收到
    if(serial.hasFrame()) {
        Frame frame = serial.getFrame();
        
        if(frame.isValid) {
            // 打印接收到的帧（调试）
            serial.printFrame(frame);
            
            // 处理帧命令
            handleCommand(frame);
        } else {
            serial.printBothln("❌ 接收到无效帧，已丢弃");
        }
    }

    // 更新LED动画
    if(millis() - lastUpdateTime >= updateInterval) {
        updateLED();
        lastUpdateTime = millis();
    }
}

// 帧解析命令函数
void handleCommand(Frame frame) {
    // 解析第一个字节获取命令类型
    uint8_t cmdType = frame.data[0];
    
    Serial.print("→ 命令类型: 0x");
    Serial.println(cmdType, HEX);
    
    switch(cmdType) {
        case 0x01: // 3字节命令：颜色设置
            if(frame.length >= 3) {
                uint8_t colorType = frame.data[1];
                uint8_t param = frame.data[2];
                handleColorCommand(colorType, param);
            }
            break;
            
        case 0x02: // 4字节命令：模式设置
            if(frame.length >= 4) {
                uint8_t modeType = frame.data[1];
                uint8_t param1 = frame.data[2];
                uint8_t param2 = frame.data[3];
                handleModeCommand(modeType, param1, param2);
            }
            break;
            
        case 0x03: // 5字节命令：高级参数设置
            if(frame.length >= 5) {
                uint8_t paramType = frame.data[1];
                uint16_t value = (frame.data[2] << 8) | frame.data[3];
                uint8_t extra = frame.data[4];
                handleParameterCommand(paramType, value, extra);
            }
            break;
            
        case 0x04: // 6字节命令：自定义RGB值
            if(frame.length >= 6) {
                uint8_t r = frame.data[1];
                uint8_t g = frame.data[2];
                uint8_t b = frame.data[3];
                uint8_t brightness = frame.data[4];
                // frame.data[5] 保留
                
                currentMode = MODE_STATIC;
                currentColor = CRGB(r, g, b);
                currentBrightness = brightness;
                FastLED.setBrightness(currentBrightness);
                fill_solid(leds, NUM_LEDS, currentColor);
                FastLED.show();
                
                Serial.print("✔ 自定义RGB: R=");
                Serial.print(r);
                Serial.print(" G=");
                Serial.print(g);
                Serial.print(" B=");
                Serial.print(b);
                Serial.print(" 亮度=");
                Serial.println(brightness);
            }
            break;
            
        case 0xFF: // 2字节命令：系统命令
            if(frame.length >= 2) {
                uint8_t sysCmd = frame.data[1];
                handleSystemCommand(sysCmd);
            }
            break;
            
        default:
            serial.printBothln("❌ 不支持的命令类型");
            break;
    }
}

// 颜色命令处理：0x01
void handleColorCommand(uint8_t colorType, uint8_t param) {
    currentMode = MODE_STATIC;
    
    switch(colorType) {
        case 0x00: currentColor = CRGB::Black; break;
        case 0x01: currentColor = CRGB::Red; break;
        case 0x02: currentColor = CRGB::Green; break;
        case 0x03: currentColor = CRGB::Blue; break;
        case 0x04: currentColor = CRGB::White; break;
        case 0x05: currentColor = CRGB::Yellow; break;
        case 0x06: currentColor = CRGB::Cyan; break;
        case 0x07: currentColor = CRGB::Magenta; break;
        default:
            serial.printBothln("❌ 不支持的颜色类型");
            return;
    }
    
    fill_solid(leds, NUM_LEDS, currentColor);
    FastLED.show();
    
    Serial.print("✔ 颜色已设置: 0x");
    Serial.println(colorType, HEX);
}

// 模式命令处理：0x02 <02 ModeType Speed Reserved>
void handleModeCommand(uint8_t modeType, uint8_t param1, uint8_t param2) {
    uint8_t newMode = 0xFF;
    
    switch(modeType) {
        case 0x01: newMode = MODE_RAINBOW; break;
        case 0x02: newMode = MODE_PULSE; break;
        case 0x03: newMode = MODE_WAVE; break;
        case 0x04: newMode = MODE_STROBE; break;
        case 0x05: newMode = MODE_TWINKLE; break;
        case 0x06: newMode = MODE_FIRE; break;
        case 0x07: newMode = MODE_CHASE; break;
        case 0x08: newMode = MODE_COLORFUL; break;
        default:
            serial.printBothln("❌ 不支持的模式类型");
            return;
    }
    
    currentMode = newMode;
    isAnimating = true;
    speed = param1; // 动画速度 (0-100)
    
    Serial.print("✔ 模式已设置: 0x");
    Serial.print(modeType, HEX);
    Serial.print(" (Speed: ");
    Serial.print(speed);
    Serial.println("%)");
}

// 参数命令处理：0x03 <03 ParamType ValueHi ValueLo Extra>
void handleParameterCommand(uint8_t paramType, uint16_t value, uint8_t extra) {
    switch(paramType) {
        case 0x01: // 亮度设置
            if(value <= 255) {
                currentBrightness = (uint8_t)value;
                FastLED.setBrightness(currentBrightness);
                FastLED.show();
                Serial.print("✔ 亮度设置为: ");
                Serial.println(currentBrightness);
            }
            break;
            
        case 0x02: // 速度设置
            if(value <= 100) {
                speed = (uint8_t)value;
                Serial.print("✔ 速度设置为: ");
                Serial.print(speed);
                Serial.println("%");
            } else {
                Serial.println("❌ 速度值范围: 0-100");
            }
            break;
            
        default:
            serial.printBothln("❌ 不支持的参数类型");
            break;
    }
}

// 系统命令处理：0xFF <FF SysCmd>
void handleSystemCommand(uint8_t sysCmd) {
    switch(sysCmd) {
        case 0x00: // 关闭所有
            currentMode = MODE_STATIC;
            currentColor = CRGB::Black;
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
            serial.printBothln("✔ LED 已关闭");
            break;
            
        case 0x01: // 显示菜单
            displayMenu();
            break;
            
        case 0x02: // 状态查询
            {
                String status = "状态: 模式=";
                status += currentMode;
                status += " 亮度=";
                status += currentBrightness;
                status += " 速度=";
                status += speed;
                serial.printBothln(status);
            }
            break;
            
        default:
            serial.printBothln("❌ 不支持的系统命令");
            break;
    }
}

void displayMenu() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║        帧协议命令格式说明             ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.println("║ 格式: <HEX_DATA>                       ║");
    Serial.println("║                                        ║");
    Serial.println("║ 0x01 - 颜色设置 (3字节)                ║");
    Serial.println("║   <010001> 红色                         ║");
    Serial.println("║   <010002> 绿色                         ║");
    Serial.println("║   <010003> 蓝色                         ║");
    Serial.println("║   <010004> 白色                         ║");
    Serial.println("║                                        ║");
    Serial.println("║ 0x02 - 模式设置 (4字节)                ║");
    Serial.println("║   <0201HH SS> 01=彩虹 02=脉冲 ...      ║");
    Serial.println("║                                        ║");
    Serial.println("║ 0x03 - 参数设置 (5字节)                ║");
    Serial.println("║   <0301VVVV 00> 亮度(0-255)            ║");
    Serial.println("║   <0302SSSS 00> 速度(0-100)            ║");
    Serial.println("║                                        ║");
    Serial.println("║ 0x04 - RGB自定义 (6字节)               ║");
    Serial.println("║   <04RRGGBBBB FF> R G B Brightness    ║");
    Serial.println("║                                        ║");
    Serial.println("║ 0xFF - 系统命令 (2字节)                ║");
    Serial.println("║   <FF00> 关闭                           ║");
    Serial.println("║   <FF01> 显示菜单                       ║");
    Serial.println("║   <FF02> 查询状态                       ║");
    Serial.println("╚════════════════════════════════════════╝\n");
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
    static uint8_t hue = 0;
    fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
    hue += (speed / 50.0);
}

void pulseMode() {
    static uint8_t brightness = 0;
    brightness = (sin8(millis() / (151 - speed)) + 1) / 2;
    
    uint8_t pulseVal = brightness;
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 255, pulseVal);
    }
}

void waveMode() {
    static uint8_t hue = 0;
    for(int i = 0; i < NUM_LEDS; i++) {
        uint8_t waveBright = sin8(hue + (i * 255 / NUM_LEDS));
        leds[i] = CHSV(0, 200, waveBright);
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
        fill_solid(leds, NUM_LEDS, CRGB::White);
    } else {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
}

void twinkleMode() {
    for(int i = 0; i < NUM_LEDS; i++) {
        if(random8() < (speed * 2)) {
            leds[i] = CRGB::White;
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
    static unsigned long lastUpdate = 0;
    
    if(millis() - lastUpdate > (151 - speed)) {
        fadeToBlackBy(leds, NUM_LEDS, 20);
        leds[pos] = CRGB::White;
        if(pos > 0) leds[pos - 1] = CRGB(100, 100, 100);
        
        pos = (pos + 1) % NUM_LEDS;
        lastUpdate = millis();
    }
}

void colorfulMode() {
    static uint8_t colorPos = 0;
    
    for(int i = 0; i < NUM_LEDS; i++) {
        uint8_t hue = (colorPos + i * 256 / NUM_LEDS) % 256;
        leds[i] = CHSV(hue, 255, 255);
    }
    
    colorPos += (speed / 50.0);
}
