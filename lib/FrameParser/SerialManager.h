#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>
#include "BluetoothSerial.h"
#include "FrameParser.h"

// 通信源类型
typedef enum {
    SOURCE_NONE = 0,
    SOURCE_UART = 1,      // 串口
    SOURCE_BLUETOOTH = 2  // 蓝牙
} CommunicationSource;

class SerialManager {
private:
    FrameParser uartParser;
    FrameParser bluetoothParser;
    BluetoothSerial* btSerial;
    
    CommunicationSource lastSource;
    Frame lastReceivedFrame;

public:
    SerialManager();
    
    /**
     * 初始化通信管理器
     * @param btDevice 蓝牙设备名称
     */
    void begin(const char* btDevice = "ESP32_LED");
    
    /**
     * 检查是否有完整的帧接收到
     * @return 如果有完整帧返回true
     */
    boolean hasFrame();
    
    /**
     * 获取接收到的帧
     * @return Frame结构体
     */
    Frame getFrame();
    
    /**
     * 获取帧的来源
     * @return 通信源类型
     */
    CommunicationSource getFrameSource();
    
    /**
     * 发送数据到串口
     */
    void printSerial(const String& msg);
    void printSerialln(const String& msg);
    
    /**
     * 发送数据到蓝牙
     */
    void printBT(const String& msg);
    void printBTln(const String& msg);
    
    /**
     * 同时发送到串口和蓝牙
     */
    void printBoth(const String& msg);
    void printBothln(const String& msg);
    
    /**
     * 打印帧数据（调试用）
     */
    void printFrame(const Frame& frame);
};

#endif
