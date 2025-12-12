// /*
//  * Ticker 定时器使用示例
//  * 支持周期触发和单次触发
//  */

// #include <Ticker.h>

// // =============== 定时器实例 ===============
// Ticker periodicTimer;      // 周期定时器
// Ticker onceTimer;          // 单次定时器

// // =============== 回调函数 ===============

// // 周期执行的函数（比如LED更新）
// void periodicCallback() {
//     Serial.println("周期定时器触发 - updateLED()");
//     // updateLED();
// }

// // 单次执行的函数
// void onceCallback() {
//     Serial.println("单次定时器触发 - 30ms后执行一次");
//     // 某些只需要执行一次的操作
// }

// // =============== 初始化定时器 ===============
// void setupTickers() {
//     // 周期定时器：每10ms执行一次
//     // attach_ms(interval_ms, callback)
//     periodicTimer.attach_ms(10, periodicCallback);
    
//     // 单次定时器：30ms后执行一次，然后自动停止
//     // once_ms(delay_ms, callback)
//     onceTimer.once_ms(30, onceCallback);
    
//     Serial.println("✔ 定时器已启动");
//     Serial.println("  - 周期定时器: 每10ms触发一次");
//     Serial.println("  - 单次定时器: 30ms后触发一次");
// }

// // =============== 控制函数 ===============

// // 停止周期定时器
// void stopPeriodicTimer() {
//     periodicTimer.detach();
//     Serial.println("✔ 周期定时器已停止");
// }

// // 重新启动周期定时器
// void startPeriodicTimer() {
//     periodicTimer.attach_ms(10, periodicCallback);
//     Serial.println("✔ 周期定时器已启动");
// }

// // 停止单次定时器（如果还未触发）
// void stopOnceTimer() {
//     onceTimer.detach();
//     Serial.println("✔ 单次定时器已停止");
// }

// // 重新设置单次定时器
// void resetOnceTimer(uint32_t delayMs) {
//     onceTimer.once_ms(delayMs, onceCallback);
//     Serial.print("✔ 单次定时器已重置，");
//     Serial.print(delayMs);
//     Serial.println("ms后触发");
// }

// =============== 在 setup() 中使用 ===============
/*
void setup() {
    Serial.begin(115200);
    
    // 初始化定时器
    setupTickers();
    
    // 使用示例：
    // - 周期定时器会持续每10ms执行一次 periodicCallback()
    // - 单次定时器会在30ms后执行一次 onceCallback()，然后停止
}
*/

// =============== 在 loop() 中使用命令控制 ===============
/*
void loop() {
    if(Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toUpperCase();
        
        if(cmd == "STOP_PERIODIC") {
            stopPeriodicTimer();
        } 
        else if(cmd == "START_PERIODIC") {
            startPeriodicTimer();
        }
        else if(cmd == "STOP_ONCE") {
            stopOnceTimer();
        }
        else if(cmd == "RESET_ONCE") {
            resetOnceTimer(50);  // 重新设置为50ms
        }
    }
}
*/

// =============== 常用方法速查表 ===============
/*

【周期定时器】
periodicTimer.attach_ms(interval_ms, callback);    // 每interval_ms毫秒执行一次
periodicTimer.attach_us(interval_us, callback);    // 每interval_us微秒执行一次
periodicTimer.detach();                            // 停止定时器

【单次定时器】
onceTimer.once_ms(delay_ms, callback);            // delay_ms毫秒后执行一次
onceTimer.once_us(delay_us, callback);            // delay_us微秒后执行一次
onceTimer.detach();                               // 停止定时器（如果还未触发）

【检查定时器状态】
if(periodicTimer.active()) {
    // 定时器正在运行
}

【高级用法 - 带参数的回调】
// 定义回调（带参数）
void callbackWithArg(void* arg) {
    int value = (int)arg;
    Serial.println(value);
}

// 使用
periodicTimer.attach_ms(100, callbackWithArg, (void*)123);

*/

