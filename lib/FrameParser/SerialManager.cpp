#include "SerialManager.h"

SerialManager::SerialManager() : btSerial(nullptr), lastSource(SOURCE_NONE) {
    lastReceivedFrame.isValid = false;
    lastReceivedFrame.length = 0;
}

void SerialManager::begin(const char* btDevice) {
    // 初始化UART解析器
    uartParser.begin();
    
    // 初始化蓝牙
    btSerial = new BluetoothSerial();
    if(!btSerial->begin(btDevice)) {
        Serial.println("❌ 蓝牙启动失败！");
    } else {
        Serial.println("✔ 蓝牙已启动！设备名称：" + String(btDevice));
    }
    
    bluetoothParser.begin();
}

boolean SerialManager::hasFrame() {
    // 检查UART
    if(Serial.available()) {
        uint8_t byte = Serial.read();
        if(uartParser.feedByte(byte)) {
            lastReceivedFrame = uartParser.getFrame();
            lastSource = SOURCE_UART;
            return true;
        }
    }
    
    // 检查蓝牙
    if(btSerial && btSerial->available()) {
        uint8_t byte = btSerial->read();
        if(bluetoothParser.feedByte(byte)) {
            lastReceivedFrame = bluetoothParser.getFrame();
            lastSource = SOURCE_BLUETOOTH;
            return true;
        }
    }
    
    return false;
}

Frame SerialManager::getFrame() {
    return lastReceivedFrame;
}

CommunicationSource SerialManager::getFrameSource() {
    return lastSource;
}

void SerialManager::printSerial(const String& msg) {
    Serial.print(msg);
}

void SerialManager::printSerialln(const String& msg) {
    Serial.println(msg);
}

void SerialManager::printBT(const String& msg) {
    if(btSerial) {
        btSerial->print(msg);
    }
}

void SerialManager::printBTln(const String& msg) {
    if(btSerial) {
        btSerial->println(msg);
    }
}

void SerialManager::printBoth(const String& msg) {
    printSerial(msg);
    printBT(msg);
}

void SerialManager::printBothln(const String& msg) {
    printSerialln(msg);
    printBTln(msg);
}

void SerialManager::printFrame(const Frame& frame) {
    if(!frame.isValid) {
        Serial.println("❌ 无效帧");
        return;
    }
    
    Serial.print("✔ 帧数据 (长度:");
    Serial.print(frame.length);
    Serial.print("字节): ");
    
    for(uint8_t i = 0; i < frame.length; i++) {
        if(frame.data[i] < 16) Serial.print("0");
        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // 打印源
    Serial.print("来源: ");
    if(lastSource == SOURCE_UART) {
        Serial.println("UART");
    } else if(lastSource == SOURCE_BLUETOOTH) {
        Serial.println("Bluetooth");
    } else {
        Serial.println("Unknown");
    }
}
