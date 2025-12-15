#include "cmd_handler.h"
#include "stdint.h"
#include "config.h"
#include <FastLED.h>
#include "effect.h"

// 帧解析命令函数
void handleCommand(Frame frame) {
    // 解析第一个字节获取命令类型
    uint8_t cmdType = frame.data[0];
    
    Serial.print("→ 命令类型: 0x");
    Serial.println(cmdType, HEX);
    
    switch(cmdType) {
        case 0x01: // 分区亮度设置
        {
            uint8_t type = frame.data[1];
            uint8_t level = frame.data[2];
            switch(type)
            {
                case 0x02:
                {
                    set_L_Level(level, xk_para);
                }
                case 0x03:
                {
                    set_L_Level(level, lx_para);
                }
                default:
                    break;
            }
        }
        break;
            
        case 0x02: // 统一亮度设置
        {
            uint8_t type = frame.data[1];
            uint8_t level = frame.data[2];
            set_L_Level(level, xk_para);
            set_L_Level(level, lx_para);
        }
        break;
            
        case 0x07: // 开关设置
        {
            uint8_t type = frame.data[1];
            switch(type)
            {
                case 0x00://全局关闭
                {
                    general_para.switch_state = OFF;
                }
                break;        
                case 0x01:
                {
                    general_para.switch_state = ON;
                }
                break;     
                case 0x20:
                {
                    xk_para.xk_switch_state = OFF;
                }
                break;        
                case 0x21:
                {
                    xk_para.xk_switch_state = ON;
                }
                break;
                case 0x30:
                {
                    lx_para.lx_switch_state = OFF;
                }
                break;        
                case 0x31:
                {
                    lx_para.lx_switch_state = ON;
                }
                break;
                default:
                    break;
            }
        }
        break;
            
        case 0x14: // 迎宾开关/触发/结束
        {
            uint8_t type = frame.data[1];
            switch(type)
            {
                case 0x02://关闭迎宾
                {
                    xk_para.wlcome_switch_state = OFF;
                }
                break;        
                case 0x03://开启迎宾
                {
                    xk_para.wlcome_switch_state = ON;
                }
                break;
                case 0x05://触发迎宾效果
                {
                    //如果当前就是迎宾模式，忽略此次触发
                    if(xk_para.xk_mode == MODE_WELCOME_1 || xk_para.xk_mode == MODE_WELCOME_2) break;

                    //触发迎宾效果
                    xk_para.xk_mode = MODE_WELCOME_1;
                    //60s倒计时，到时间后结束迎宾效果，恢复上次模式
                    onceTimer.once_ms(xk_para.xk_welcome_s * 1000, [](){
                        // xk_para.xk_mode = xk_para.xk_last_mode;
                        xk_para.xk_mode = MODE_STATIC;
                    });
                }
                break;        
                case 0x06://结束迎宾效果
                {   //恢复上次模式，暂时返回静态，可以搞一个切换模式，专门用来衔接各种切换
                    xk_para.xk_mode = MODE_STATIC;
                }
                break;
                default:
                    break;
            }
        }
        break;
            
        case 0x06: // 统一颜色设置
        {
            //设置死颜色，星空模式暂时跳回静态吧，流星模式不管
            xk_para.xk_mode = MODE_STATIC;
            uint8_t r = frame.data[1];
            uint8_t g = frame.data[2];
            uint8_t b = frame.data[3];
            set_color(r, g, b, xk_para);
            set_color(r, g, b, lx_para);
        }
        break;

        case 0x10: // 星空颜色设置
        {
            //设置死颜色，星空模式暂时跳回静态吧，流星模式不管
            xk_para.xk_mode = MODE_STATIC;
            uint8_t r = frame.data[1];
            uint8_t g = frame.data[2];
            uint8_t b = frame.data[3];
            set_color(r, g, b, xk_para);
        }
        break;

        case 0x11: // 流星颜色设置
        {
            uint8_t r = frame.data[1];
            uint8_t g = frame.data[2];
            uint8_t b = frame.data[3];
            set_color(r, g, b, lx_para);
        }
        break;

        case 0x13: // 星空灯单组（3个）LED颜色设置
        {
            xk_para.xk_mode = MODE_STATIC;
            uint8_t group = frame.data[1];
            uint8_t r = frame.data[2];
            uint8_t g = frame.data[3];
            uint8_t b = frame.data[4];
            // 每组3个LED 细节要注意，01代表 123号灯珠，02代表456号灯珠，，，暂时先乱写个3的倍数
            for (int i = 0; i < 3; i++)
            {
                set_pixel_color(group * 3 + i, r, g, b, xk_para);
            }
        }
        break;

        case 0x03: // 音乐灵敏度设置
        {
            uint8_t level = frame.data[1];
            general_para.music_sens = level;
        }
        break;

        case 0x85: // 星空灯呼吸速度设置
        {
            uint8_t level = frame.data[1];
            xk_para.xk_breath_speed = level;
        }
        break;

        case 0x86: // 流星灯参数设置
        {
            uint8_t type = frame.data[1];
            uint8_t para = frame.data[2];

            switch (type)
            {
                case 0x01:
                {
                    lx_para.lx_total_gap = para;
                }
                break;        
                case 0x02:
                {
                    lx_para.lx_1_sepped = para;
                }
                case 0x03:
                {
                    lx_para.lx_1_dir = para;
                }
                case 0x04:
                {
                    lx_para.lx_1_2_gap = para;
                }
                case 0x05:
                {
                    lx_para.lx_2_sepped = para;
                }
                case 0x06:
                {
                    lx_para.lx_2_dir = para;
                }
                break;     
                default:
                    break;
            }
        }
        break;

        case 0x35: //硬件读数据到app
        {
        }
        break;
           
        case 0x40: //调整灯珠数量， 暂时格式有问题，跟表格对不上，表格目前我看不懂
        {
            uint8_t type = frame.data[1];
            uint8_t num = frame.data[2];
            if(type == 0x02) //星空灯
            {
                xk_para.xk_led_num = num;
            }
            else if(type == 0x03) //流星灯
            {
                lx_para.lx_led_num = num;
            }
        }
        break;
           
        case 0x41: // 音源选择
        {
            uint8_t source = frame.data[1];
            if(source == 0x01)
            {
                general_para.audio_source = HORN;
            }
            else if(source == 0x02)
            {
                general_para.audio_source = MICROPHONE;
            }
            //切换了音源，更新工作谁来做？如果当前正好是music, 搞个音乐效果结构体参数
            if(xk_para.xk_mode == MODE_MUSIC)
            {
            }
        }
        break;
           
        case 0x50: // 控制源选择
        {
            uint8_t source = frame.data[1];
            if(source == 0x00)
            {
                general_para.control_source = APP_ONLY;
            }
            else if(source == 0x01)
            {
                general_para.control_source = APP_and_CAR;
            }
        }
        break;   
        
        case 0x23: // 更改蓝牙名称
        {
        }
        break;
           
        case 0x24: // 设备型号版本
        {
        }
        break;
           
        case 0x44: // 关机延时设置
        {
        }
        break;
           
        case 0x21: // 氛围灯亮度上限设置
        {
            uint8_t type = frame.data[1];
            uint8_t maxVal = frame.data[2];
            if(type == 0x02)
            {
                set_L_top(maxVal, xk_para);
            }
            else if(type == 0x03)
            {
                set_L_top(maxVal, lx_para);
            }
        }
        break;

        case 0x80: // 星空灯模式
        {
            uint8_t mode = frame.data[1];
            if(xk_para.xk_mode == mode) break; //如果模式没变就不管了

            if(mode >= MODE_STATIC && mode <= MODE_BREATH_RANDOM)
            {
                //保存上次模式
                xk_para.xk_last_mode = xk_para.xk_mode;
                xk_para.xk_mode = static_cast<XK_LEDMode_E>(mode);
            }
            if(mode == MODE_WELCOME_1)
            {
                //60s倒计时，到时间后结束迎宾效果，恢复上次模式
                onceTimer.once_ms(xk_para.xk_welcome_s * 1000, [](){
                // xk_para.xk_mode = xk_para.xk_last_mode;
                xk_para.xk_mode = MODE_STATIC;
                });
            }
            if(xk_para.xk_last_mode != MODE_BREATH_BW && xk_para.xk_mode == MODE_BREATH_BW)
            {
                //进蓝白呼吸初始化
                xk_bw_breath_init();
            }
        }
        break;
           
        case 0x81: // 流星效果
        {
            uint8_t mode = frame.data[1];
            if(mode >= MODE_DEFAULT && mode <= MODE_DOUBLE)
            {
                lx_para.lx_mode = static_cast<LX_LEDMode_E>(mode);
            }
        }
        break;

        default:
            serial.printBothln("❌ 不支持的命令类型");
            break;
    }
}
