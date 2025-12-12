#include "FrameParser.h"

// 命令定义表 - 清晰易维护的格式
const FrameParser::CommandDef FrameParser::commandTable[] = {
    // 格式: {cmdType, length, "name"}
    {0x01,  3, "set_L_level"},           // 颜色设置
    {0x02,  3, "set_all_L_level"},       // 动画模式设置
    {0x07,  2, "switch"},                // 参数设置
    {0x14,  2, "welcome"},               // 自定义RGB值
    {0x06,  4, "set_all_color"},         // 设置所有颜色
    {0x10,  4, "set_xk_color"},          // 设置星空灯颜色
    {0x11,  4, "set_lx_color"},          // 设置流星颜色
    {0x13,  5, "set_xk_1_point_color"},  // 设置星空灯单点颜色
    {0x03,  3, "set_voice_sensitivity"}, // 设置声音灵敏度
    {0x85,  2, "set_xk_breath_speed"},   // 设置星空灯呼吸速度
    {0x86,  3, "set_lx_para"},           // 设置流星参数
    {0x35, 20, "read_config"},           // 读取配置 长度暂定
    {0x40, 20, "set_led_num"},           // 设置led数量 长度暂定
    {0x41,  2, "set_voice_source"},      // 设置声音来源
    {0x50,  2, "set_control_source"},    // 设置控制来源
    {0x23, 20, "set_bt_name"},           // 设置蓝牙名称 长度暂定
    {0x24, 20, "device_version"},        // 设备型号版本 长度暂定 //牛魔的到底是设置还是读取？
    {0x44,  4, "set_shutdown_delay"},    // 设置关机延时
    {0x21,  3, "set_L_top"},             // 设置亮度上限
    {0x80,  2, "set_xk_mode"},           // 设置星空灯模式
    {0x81,  2, "set_lx_mode"},           // 设置流星效果
    {0xFF,  2, "SysCmd"}                 // 系统命令
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
        //添加一个不定长数据判断，如果命令类型是0xff，则允许长度 > 0 就过
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
