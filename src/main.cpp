#include <FastLED.h>
#include "config.h"
#include "cmd_handler.h"
#include "effect.h"


void display_chip_info();
void displayMenu();


void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(100);

    // 初始化通信管理器
    serial.begin("ESP32_LED");
    // 初始化 LED
    fast_led_init();
    //memory_func();
    
    display_chip_info();
    displayMenu();

    //periodicTimer.attach_ms(5000, _10_ms_task);  // 每10ms更新一次LED动画
    // onceTimer.once_ms(5000, []() {
    //         serial.printBothln("一次性定时器触发！");
    // });
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

    // 每10ms任务
    static uint32_t lastLed = 0;
    uint32_t now = millis();
    if (now - lastLed >= 10) {
        lastLed = now;
        _10_ms_task();
        //serial.printBothln(String(millis()));  // 获取从启动开始的毫秒数
    }
}


void displayMenu() {
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║     ESP32 高级 LED 灯条控制器      ║");
    Serial.println("║     帧协议版本 (Frame Protocol)   ║");
    Serial.println("╚════════════════════════════════════╝");

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

void display_chip_info() {
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
}
