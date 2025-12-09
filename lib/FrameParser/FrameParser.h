#ifndef FRAME_PARSER_H
#define FRAME_PARSER_H

#include <Arduino.h>

// 命令类型定义 - 根据第一个字节判断长度
typedef enum {
    CMD_TYPE_01 = 0x01,  // 长度: 3 bytes (01 + 2个hex数据)
    CMD_TYPE_02 = 0x02,  // 长度: 4 bytes (02 + 3个hex数据)
    CMD_TYPE_03 = 0x03,  // 长度: 5 bytes (03 + 4个hex数据)
    CMD_TYPE_04 = 0x04,  // 长度: 6 bytes (04 + 5个hex数据)
    CMD_TYPE_FF = 0xFF,  // 长度: 2 bytes (FF + 1个hex数据)
} CommandType;

// 帧结构
typedef struct {
    uint8_t data[64];      // 命令数据
    uint8_t length;        // 数据长度
    boolean isValid;       // 是否有效
} Frame;

class FrameParser {
private:
    // 接收缓冲区
    char receiveBuffer[128];
    uint16_t bufferIndex;
    
    // 帧状态机
    boolean frameStarted;
    
    // 最后接收到的帧
    Frame lastFrame;
    
    // 命令定义结构体
    struct CommandDef {
        uint8_t cmdType;      // 命令类型（第一字节）
        uint8_t length;       // 命令长度（字节数）
        const char* name;     // 命令名称（调试用）
    };
    
    // 命令定义表 - 易于查看和维护
    static const CommandDef commandTable[];
    static const uint8_t commandTableSize;
    
    /**
     * 获取命令长度
     * @param cmdType 命令类型（第一个字节）
     * @return 命令长度（字节数），0表示无效命令类型
     */
    uint8_t getCommandLength(uint8_t cmdType);
    
    /**
     * 将HEX字符串转换为字节数组
     * @param hexStr HEX字符串
     * @param length 字符串长度
     * @param output 输出字节数组
     * @return 转换后的字节数，-1表示转换失败
     */
    int16_t hexStringToBytes(const char* hexStr, uint8_t length, uint8_t* output);
    
    /**
     * 检查HEX字符串有效性
     * @param hexStr 字符串
     * @param length 长度
     * @return 是否有效
     */
    boolean isValidHexString(const char* hexStr, uint8_t length);
    
    /**
     * 清空接收缓冲区
     */
    void clearBuffer();

public:
    FrameParser();
    
    /**
     * 初始化解析器
     */
    void begin();
    
    /**
     * 处理来自数据流的一个字节
     * @param byte 接收到的字节
     * @return 如果收到完整帧返回true，否则返回false
     */
    boolean feedByte(uint8_t byte);
    
    /**
     * 获取最后解析的帧
     * @return Frame结构体
     */
    Frame getFrame();
    
    /**
     * 获取最后一个错误信息
     * @return 错误描述字符串
     */
    String getLastError();
};

#endif
