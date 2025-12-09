#include "FrameParser.h"

// 命令定义表 - 清晰易维护的格式
const FrameParser::CommandDef FrameParser::commandTable[] = {
    // 格式: {cmdType, length, "name"}
    {0x01, 3, "ColorSet"},      // 颜色设置
    {0x02, 4, "ModeSet"},       // 动画模式设置
    {0x03, 5, "ParamSet"},      // 参数设置
    {0x04, 6, "RGBCustom"},     // 自定义RGB值
    {0xFF, 2, "SysCmd"}         // 系统命令
};

const uint8_t FrameParser::commandTableSize = sizeof(FrameParser::commandTable) / sizeof(FrameParser::CommandDef);

FrameParser::FrameParser() : bufferIndex(0), frameStarted(false) {
    lastFrame.isValid = false;
    lastFrame.length = 0;
}

void FrameParser::begin() {
    clearBuffer();
    frameStarted = false;
}

uint8_t FrameParser::getCommandLength(uint8_t cmdType) {
    // 在命令表中查找对应的命令
    for(uint8_t i = 0; i < commandTableSize; i++) {
        if(commandTable[i].cmdType == cmdType) {
            return commandTable[i].length;
        }
    }
    // 如果找不到，返回 0（表示无效命令）
    return 0;
}

int16_t FrameParser::hexStringToBytes(const char* hexStr, uint8_t length, uint8_t* output) {
    // HEX字符串长度必须是偶数
    if(length % 2 != 0) {
        return -1;
    }
    
    uint8_t byteCount = length / 2;
    
    for(uint8_t i = 0; i < byteCount; i++) {
        char highNibble = hexStr[i * 2];
        char lowNibble = hexStr[i * 2 + 1];
        
        // 转换为大写
        highNibble = toupper(highNibble);
        lowNibble = toupper(lowNibble);
        
        // 验证是否为有效的HEX字符
        if(!isxdigit(highNibble) || !isxdigit(lowNibble)) {
            return -1;
        }
        
        // 转换为字节
        uint8_t highVal = (highNibble >= 'A') ? (highNibble - 'A' + 10) : (highNibble - '0');
        uint8_t lowVal = (lowNibble >= 'A') ? (lowNibble - 'A' + 10) : (lowNibble - '0');
        
        output[i] = (highVal << 4) | lowVal;
    }
    
    return byteCount;
}

boolean FrameParser::isValidHexString(const char* hexStr, uint8_t length) {
    if(length % 2 != 0) {
        return false;
    }
    
    for(uint8_t i = 0; i < length; i++) {
        if(!isxdigit(hexStr[i])) {
            return false;
        }
    }
    
    return true;
}

void FrameParser::clearBuffer() {
    bufferIndex = 0;
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
}

boolean FrameParser::feedByte(uint8_t byte) {
    char ch = (char)byte;
    
    if(ch == '<') {
        // 帧开始
        frameStarted = true;
        clearBuffer();
        return false;
    }
    
    if(!frameStarted) {
        return false;
    }
    
    if(ch == '>') {
        // 帧结束，开始解析
        
        Frame result;
        result.isValid = false;
        result.length = 0;
        
        if(bufferIndex == 0) {
            // 空帧
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        // 验证HEX字符串
        if(!isValidHexString(receiveBuffer, bufferIndex)) {
            Serial.println("❌ 帧错误: 包含非HEX字符");
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        // 转换第一个字节以获取命令类型
        uint8_t tempByte;
        int16_t convResult = hexStringToBytes(receiveBuffer, 2, &tempByte);
        if(convResult <= 0) {
            Serial.println("❌ 帧错误: 第一个字节转换失败");
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        uint8_t cmdType = tempByte;
        uint8_t expectedLength = getCommandLength(cmdType);
        
        // 验证命令长度
        if(expectedLength == 0) {
            Serial.print("❌ 帧错误: 不支持的命令类型 0x");
            Serial.println(cmdType, HEX);
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        // HEX字符串长度应该是字节数*2
        uint8_t expectedHexLength = expectedLength * 2;
        if(bufferIndex != expectedHexLength) {
            Serial.print("❌ 帧错误: 命令长度不匹配 (期望");
            Serial.print(expectedLength);
            Serial.print("字节, 收到");
            Serial.print(bufferIndex / 2);
            Serial.println("字节)");
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        // 转换完整的帧数据
        int16_t bytesConverted = hexStringToBytes(receiveBuffer, bufferIndex, result.data);
        if(bytesConverted <= 0) {
            Serial.println("❌ 帧错误: 数据转换失败");
            frameStarted = false;
            clearBuffer();
            return false;
        }
        
        result.length = (uint8_t)bytesConverted;
        result.isValid = true;
        
        // 保存帧到成员变量
        lastFrame = result;
        
        frameStarted = false;
        clearBuffer();
        
        return true;
    }
    
    // 积累字节到缓冲区
    if(bufferIndex < sizeof(receiveBuffer) - 1) {
        receiveBuffer[bufferIndex++] = ch;
        return false;
    } else {
        // 缓冲区溢出
        Serial.println("❌ 帧错误: 缓冲区溢出");
        frameStarted = false;
        clearBuffer();
        return false;
    }
}

Frame FrameParser::getFrame() {
    return lastFrame;
}

String FrameParser::getLastError() {
    return "Error details not implemented";
}
