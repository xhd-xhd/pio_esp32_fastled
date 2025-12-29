#include "effect.h"
#include <FastLED.h>

#pragma region 亮度相关设置 最终更新para.L_actual==================

void set_L_Level(uint8_t level, LX_PARA_T &para) {
    if (level > 100) level = 100;
    para.lx_L_level = level;
    uint8_t k = (level * para.lx_L_top_limit) / 100;
    para.lx_L_actual = map(k, 0, 100, 0, 255);
}

void set_L_Level(uint8_t level, XK_PARA_T &para) {
    if (level > 100) level = 100;
    para.xk_L_level = level;
    uint8_t k = (level * para.xk_L_top_limit) / 100;
    para.xk_L_actual = map(k, 0, 100, 0, 255);
}

void set_L_top(uint8_t maxVal, LX_PARA_T &para) {
    if (maxVal > 100) maxVal = 100;
    para.lx_L_top_limit = maxVal;
    set_L_Level(para.lx_L_level, para);
}

void set_L_top(uint8_t maxVal, XK_PARA_T &para) {
    if (maxVal > 100) maxVal = 100;
    para.xk_L_top_limit = maxVal;
    set_L_Level(para.xk_L_level, para);
}

void set_CRGB_L(CRGB& c, uint8_t L)
{
    // 特殊情况：目标亮度L=0时，直接设置为纯黑色，无需计算
    if (L == 0)
    {
        c = CRGB::Black;
        return;
    }

    // 如果已经是目标亮度（或非常接近），避免不必要的计算
    if (L == 255 && c.r == 255 && c.g == 255 && c.b == 255)
    {
        return; // 已经是最高亮度白色，无需处理
    }

    const uint8_t max_brightness = 255; // FastLED最大亮度基准值

    // 1. 强制转换为16位整数（uint16_t），避免8位整数相乘溢出导致的颜色失真
    // 2. 按亮度比例缩放R、G、B三个分量，保证色相不变（等比例缩放）
    c.r = (uint16_t)c.r * L / max_brightness;
    c.g = (uint16_t)c.g * L / max_brightness;
    c.b = (uint16_t)c.b * L / max_brightness;
    
    // 不需要返回任何值，因为c已经被修改
}
#pragma endregion ============================



#pragma region 颜色相关设置 ==================


// 设置XK设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    
    for (int i = 0; i < para.xk_led_num; i++) {
        para.xk_custom_colors[i] = originalColor;
    }
}

// 设置LX设备颜色（不考虑亮度）
void set_color(uint8_t r, uint8_t g, uint8_t b, LX_PARA_T &para) {
    // 保存原始颜色
    CRGB originalColor(r, g, b);
    para.lx_custom_color = originalColor;
}

// 设置XK设备单个LED颜色（不考虑亮度）
void set_pixel_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b, XK_PARA_T &para) {
    if (index < para.xk_led_num) {
        CRGB originalColor(r, g, b);
        para.xk_custom_colors[index] = originalColor;
    }
}


#pragma endregion ============================



#pragma region 星空灯效果 ==========================


#pragma region 迎宾效果
void welcome_effect()
{
    uint8_t l = xk_para.xk_L_actual;
    for(int idx = 0; idx < xk_para.xk_led_num; idx++) 
    {
        xk_para.xk_end_colors[idx] = CRGB::White;
        xk_para.xk_end_colors[idx].nscale8_video(l);
    }
}
#pragma endregion

#pragma region 蓝白呼吸 ==========================

BREATH_PARA_T breath_para =
{
    .random_num = 20,
};


void xk_bw_breath_init()
{
    breath_para.bw_start_ms = (uint32_t)millis();
}
//蓝白呼吸效果 暂时是蓝色/白色分别呼吸一个周期
void blue_white_breath_effect() {
    static uint32_t j = 0, k = 0;
    static uint8_t color[6] = {0,0,255, 255,255,255};//呼吸的颜色
    uint32_t current_ms = (uint32_t)millis();
    uint32_t elapsed_ms = current_ms - breath_para.bw_start_ms;
    uint32_t h_period = xk_para.xk_breath_speed * 500; //半周期
    uint8_t  L_max = xk_para.xk_L_actual;
    uint8_t r, g, b, l;

    j = (elapsed_ms /  h_period) % 2;        // j半个周期翻转一次
    k = (elapsed_ms / (h_period * 2)) % 2;   // k每个完整周期翻转，用于翻转颜色
    float i = (float)(elapsed_ms % h_period) / h_period;
    float ration = powf(i, 2.0);

    l = (j == 0) ? (L_max * ration) : (L_max * (1 - ration));
    r = color[k * 3 + 0];
    g = color[k * 3 + 1];
    b = color[k * 3 + 2];

    for(int i = 0; i < xk_para.xk_led_num; i++) 
    {
        xk_para.xk_end_colors[i].r = r;
        xk_para.xk_end_colors[i].g = g;
        xk_para.xk_end_colors[i].b = b;
        xk_para.xk_end_colors[i].nscale8_video(l);
    }
}
#pragma endregion ============================

#pragma region 间隔呼吸 ==========================

void gap_breath_init()
{
    breath_para.gap_start_ms = (uint32_t)millis();
}
// void gap_led_breath_effect()
// {
//     static uint32_t j = 0, k = 0, gap = 2;
//     uint32_t current_ms = (uint32_t)millis();
//     uint32_t elapsed_ms = current_ms - breath_para.gap_start_ms;
//     uint32_t h_period = xk_para.xk_breath_speed * 500; //半周期
//     uint8_t  L_max = xk_para.xk_L_actual;
//     uint8_t  r, g, b, l;

//     j = (elapsed_ms /  h_period) % 2;        // j半个周期翻转一次
//     k = (elapsed_ms / (h_period * 2)) % 2;   // k每个完整周期翻转
//     float i = (float)(elapsed_ms % h_period) / h_period;  //i半周期 0-1过渡
//     float ration = powf(i, 2.0);

//     l = (j == 0) ? (L_max * ration) : (L_max * (1 - ration));  //呼吸的亮度


//     for(int i = 0; i < XK_LED_NUM; i++) 
//     {
//         xk_para.xk_end_colors[i].r = xk_para.xk_custom_colors[i].r;
//         xk_para.xk_end_colors[i].g = xk_para.xk_custom_colors[i].g;
//         xk_para.xk_end_colors[i].b = xk_para.xk_custom_colors[i].b;
//         uint8_t n = i % (gap * 2);
//         if(n < gap)
//         {
//             xk_para.xk_end_colors[i].nscale8_video(l);
//         }
//         else
//         {
//             xk_para.xk_end_colors[i].nscale8_video(L_max - l);
//         }
        
//     }
// }

//ai 写的，看着还行，比上面我写的好，逻辑差不多，效果好些
void gap_led_breath_effect()
{
    uint32_t current_ms = millis();
    uint32_t elapsed_ms = current_ms - breath_para.gap_start_ms;

    // 使用整数运算避免浮点，提高效率
    uint32_t h_period = xk_para.xk_breath_speed * 500; // 半周期（毫秒）
    uint32_t full_period = h_period * 2;
    
    // 计算当前在半周期内的位置 (0-255，对应0.0-1.0)
    uint32_t phase = elapsed_ms % full_period;
    uint8_t i_255;
    
    if(phase < h_period) {
        // 上升阶段
        i_255 = (phase * 255) / h_period;
    } else {
        // 下降阶段
        i_255 = 255 - ((phase - h_period) * 255) / h_period;
    }
    
    // 转换为0-1的浮点数（如果需要）
    float i = i_255 / 255.0f;
    
    // 改进的呼吸曲线：使用easeInOutCubic，更自然的呼吸效果
    float t = i;
    float ration;
    
    if(t < 0.5f) {
        // 前半段：easeInCubic
        ration = 4.0f * t * t * t;
    } else {
        // 后半段：easeOutCubic
        t = t - 1.0f;
        ration = 1.0f + 4.0f * t * t * t;
    }
    
    // 计算亮度（优化版，避免浮点运算）
    uint8_t l = (uint8_t)(xk_para.xk_L_actual * ration);
    
    // 预计算互补亮度
    uint8_t l_complement = xk_para.xk_L_actual - l;
    
    // 应用效果到LED（优化循环）
    uint8_t gap = 2; // 间隔大小
    
    for(int idx = 0; idx < xk_para.xk_led_num; idx++) 
    {
        // 直接赋值，减少结构体访问
        CRGB &src_color = xk_para.xk_custom_colors[idx];
        CRGB &dst_color = xk_para.xk_end_colors[idx];
        
        dst_color.r = src_color.r;
        dst_color.g = src_color.g;
        dst_color.b = src_color.b;
        
        // 判断LED属于哪一组
        if((idx % (gap * 2)) < gap)
        {
            // 第一组：跟随主呼吸
            dst_color.nscale8_video(l);
        }
        else
        {
            // 第二组：反向呼吸
            dst_color.nscale8_video(l_complement);
        }
    }
}
#pragma endregion ============================

#pragma region 单色呼吸
void static_breath_init()
{
    breath_para.static_start_ms = (uint32_t)millis();
}
//目前自定义颜色和静态模式下设置的全局颜色是同一个buffer，记忆用户自定义颜色功能由上层完成，我只管接受颜色数据。
//进入单色呼吸之前，先固定custom里的颜色一致，这里我直接刷custom里的颜色，暂时就这样吧
void static_breath_effect()
{
    static uint32_t j = 0, k = 0;
    uint32_t current_ms = (uint32_t)millis();
    uint32_t elapsed_ms = current_ms - breath_para.static_start_ms;
    uint32_t h_period = xk_para.xk_breath_speed * 500; //半周期
    uint8_t  L_max = xk_para.xk_L_actual;
    uint8_t r, g, b, l;

    j = (elapsed_ms /  h_period) % 2;        // j半个周期翻转一次
    k = (elapsed_ms / (h_period * 2)) % 2;   // k每个完整周期翻转，用于翻转颜色
    float i = (float)(elapsed_ms % h_period) / h_period;
    float ration = powf(i, 2.0);

    l = (j == 0) ? (L_max * ration) : (L_max * (1 - ration));
    for(int idx = 0; idx < xk_para.xk_led_num; idx++) 
    {
        xk_para.xk_end_colors[idx] = xk_para.xk_custom_colors[idx];
        xk_para.xk_end_colors[idx].nscale8_video(l);
    }
}
#pragma endregion

#pragma region 随机呼吸
void reset_arr(uint8_t* arr, uint8_t len, uint8_t _1num)
{
    if (!arr || len == 0) return;

    if (_1num > len)
        _1num = len;

    // 1. 清零
    for (uint8_t i = 0; i < len; i++)
        arr[i] = 0;

    // 2. 前 _1num 个置 1
    for (uint8_t i = 0; i < _1num; i++)
        arr[i] = 1;

    // 3. Fisher-Yates 洗牌
    for (uint8_t i = len - 1; i > 0; i--)
    {
        uint8_t j = random(i + 1);  // 0 ~ i
        uint8_t tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}
void reset_arr(uint8_t* arr, uint8_t len, uint8_t _0num, uint8_t _1num)
{
    if (!arr || len == 0) return;

    if (_0num + _1num > len)
        return;  // 参数错误，直接返回（也可以自行修正）

    uint8_t idx = 0;

    // 1. 填 0
    for (uint8_t i = 0; i < _0num; i++)
        arr[idx++] = 0;

    // 2. 填 1
    for (uint8_t i = 0; i < _1num; i++)
        arr[idx++] = 1;

    // 3. 剩余填 2
    while (idx < len)
        arr[idx++] = 2;

    // 4. 洗牌（Fisher-Yates）
    for (uint8_t i = len - 1; i > 0; i--)
    {
        uint8_t j = random(i + 1); // 0 ~ i
        uint8_t tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void random_breath_init()
{
    breath_para.random_start_ms = (uint32_t)millis();
}
void random_breath_effect()
{
    static uint32_t j = 0, k = 0, last_k;
    uint32_t current_ms = (uint32_t)millis();
    uint32_t elapsed_ms = current_ms - breath_para.random_start_ms;
    uint32_t h_period = xk_para.xk_breath_speed * 500; //半周期
    uint8_t  L_max = xk_para.xk_L_actual;
    uint8_t r, g, b, l;
    static uint8_t random_arr[40] = {0};

    j = (elapsed_ms /  h_period) % 2;        // j半个周期翻转一次
    k = (elapsed_ms / (h_period * 2)) % 2;   // k每个完整周期翻转，用于翻转颜色
    float i = (float)(elapsed_ms % h_period) / h_period;
    float ration = powf(i, 2.0);

    l = (j == 0) ? (L_max * ration) : (L_max * (1 - ration));


    if(last_k != k)
    {
        last_k = k;
        reset_arr(random_arr, xk_para.xk_led_num, 10, 10);
    }

    for(int idx = 0; idx < xk_para.xk_led_num; idx++) 
    {
        xk_para.xk_end_colors[idx] = xk_para.xk_custom_colors[idx];
        if(random_arr[idx] == 0)
        {
            xk_para.xk_end_colors[idx].nscale8_video(l);
        }
        else if(random_arr[idx] == 1)
        {
            xk_para.xk_end_colors[idx].nscale8_video(L_max - l);
        }
    }
}
//这个效果是随机k个led呼吸，其余的灭，感觉一般
// void random_breath_effect()
// {
//     static uint32_t j = 0, k = 0, last_k;
//     uint32_t current_ms = (uint32_t)millis();
//     uint32_t elapsed_ms = current_ms - breath_para.random_start_ms;
//     uint32_t h_period = xk_para.xk_breath_speed * 500; //半周期
//     uint8_t  L_max = xk_para.xk_L_actual;
//     uint8_t r, g, b, l;
//     static uint8_t random_arr[40] = {0};

//     j = (elapsed_ms /  h_period) % 2;        // j半个周期翻转一次
//     k = (elapsed_ms / (h_period * 2)) % 2;   // k每个完整周期翻转，用于翻转颜色
//     float i = (float)(elapsed_ms % h_period) / h_period;
//     float ration = powf(i, 2.0);

//     l = (j == 0) ? (L_max * ration) : (L_max * (1 - ration));


//     if(last_k != k)
//     {
//         last_k = k;
//         //一个周期过了，重置01数组
//         reset_arr(random_arr, xk_para.xk_led_num, 10);
//     }

//     for(int idx = 0; idx < xk_para.xk_led_num; idx++) 
//     {
//         xk_para.xk_end_colors[idx] = xk_para.xk_custom_colors[idx];
//         if(random_arr[idx] == 1)
//         {
//             xk_para.xk_end_colors[idx].nscale8_video(l);
//         }
//         else
//         {
//             xk_para.xk_end_colors[idx] = CRGB::Black;
//         }
//     }
// }
#pragma endregion


#pragma endregion 星空灯效果========================



#pragma region 流星灯效果 ==========================

LX_EFFECT_PARA_T lx_effect_para =
{
    .dummy_num = 5,
    .random_light_num = 5,
    .tail_len = 8,
};

void set_tail_L(uint8_t p[], uint8_t tail_len, uint8_t max_L, uint8_t min_L)
{
    // 参数验证
    if (tail_len == 0 || max_L <= min_L || p == NULL) {
        return; // 或者返回错误代码
    }
    
    if (tail_len == 1) {
        p[0] = max_L;
        return;
    }
    
    for (int i = 0; i < tail_len; i++) {
        // 使用浮点数计算增益，然后四舍五入
        float gain = (float)i / (float)(tail_len - 1);
        float value = min_L + (max_L - min_L) * powf(gain, 2.0);
        
        // 四舍五入到最近的整数
        p[i] = (uint8_t)(value + 0.5f);
    }
}

#define LED_NUM     40
#define METEOR_LEN  8     // 尾巴长度
#define METEOR_DECAY 150  // 拖尾衰减（0~255，越小尾巴越长）
//ai写的效果，看着也还好
void lx_default_effect_ai()
{
    static int16_t pos = -METEOR_LEN;   // 流星当前位置
    static uint8_t tick = 0;

    // ---------- 1. 控制速度 ----------
    tick++;
    if (tick < 3)   // 3 * 10ms = 30ms 移动一次
        return;
    tick = 0;

    // ---------- 2. 尾巴整体衰减 ----------
    for (int i = 0; i < LED_NUM; i++)
    {
        lx_leds[i].fadeToBlackBy(METEOR_DECAY);
    }

    // ---------- 3. 绘制流星头 ----------
    if (pos >= 0 && pos < LED_NUM)
    {
        lx_leds[pos] = CRGB::White; // 流星颜色
    }

    // ---------- 4. 前进 ----------
    pos++;

    // ---------- 5. 到尾部重置 ----------
    if (pos > LED_NUM + METEOR_LEN)
    {
        pos = -METEOR_LEN;
    }
}
//自己写的流星效果，基本能控制所有参数
void lx_default_effect()
{
    static uint32_t t1 = 0, step_cnt = 1000 * 20;//目的：第一次进入效果的时候先要设置大随机数
    static uint8_t eff_data[LX_LED_NUM * 2 + 10 * 2 + 10 * 2] = {};//被滑 窗口数据，默认给个很大的内存，dummy <= 10,tail <= 10
    static uint8_t tail_L[10] = {};//存储拖尾亮度

    //方向暂时由流星1段指定方向, 亮度最大值由设置的亮度上限转换后的实际上限决定
    uint8_t dir = lx_para.lx_1_dir, MAX_L = lx_para.lx_L_actual;
    uint32_t t2 = millis(), period = lx_para.lx_total_gap * 1000;//一趟流星周期
    uint8_t led_num = lx_para.lx_led_num, dummy_num = lx_effect_para.dummy_num, tail_len = lx_effect_para.tail_len;
    uint8_t total_step = led_num + tail_len + dummy_num * 2;//窗口滑动次数
    uint32_t step_gap = period / (total_step);//根据周期计算窗口滑动间隔 gap
    set_tail_L(tail_L, tail_len, MAX_L, 1);//根据拖尾长度，亮度上下限，提前算出拖尾亮度
 
    memset(eff_data, 0, sizeof(eff_data));
    memset(&eff_data[led_num + dummy_num], 1, tail_len);
    if(step_cnt > total_step)//窗口滑动到头了，重置参数
    {
        step_cnt = 0;
        serial.printBothln("total_step_passed");
    }
    if(t2 - t1 > step_gap)
    {
        t1 = t2;
        step_cnt++;
        //效果步进 lx_leds
        uint8_t p = step_cnt + led_num;
        uint8_t k = 0;
        for (int i = 0; i < led_num; i++)
        {
            uint8_t pos = (dir == 1) ? (i) : (led_num - i - 1);
            if(eff_data[p--] == 1)
            {
                CRGB c = {255, 255, 255};
                set_CRGB_L(c, tail_L[k++]);
                lx_leds[pos] = c;
            }
            else
            {
                lx_leds[pos] = CRGB::Black;
            }
        }
    }
}

//效果思路：随机帧，固定窗口大小滑动取值
void lx_random_effect() {
    static uint32_t t1 = 0, step_cnt = 1000 * 20;//目的：第一次进入效果的时候先要设置随机数
    static uint8_t eff_data[LX_LED_NUM * 3 + 5 * 2] = {};//被滑 窗口数据，默认给个很大的内存，dummy <= 5

    uint32_t t2 = millis(), period = lx_para.lx_total_gap*1000;
    uint8_t led_num = lx_para.lx_led_num, dummy_num = lx_effect_para.dummy_num, light_num = lx_effect_para.random_light_num;
    uint8_t len = led_num * 3 + dummy_num * 2;//帧长度
    uint8_t total_step = led_num * 2 + dummy_num * 2;
    uint32_t step_gap = period / (total_step);
    if(step_cnt > total_step)
    {
        memset(eff_data, 0, sizeof(eff_data));
        reset_arr(&eff_data[40 + 5], 40, light_num);
        step_cnt = 0;
        serial.printBothln("total_step_passed");
    }
    if(t2 - t1 > step_gap)
    {
        t1 = t2;
        step_cnt++;
        //效果步进 lx_leds
        uint8_t p = step_cnt + 40;
        for (int i = 0; i < 40; i++)
        {
            if(eff_data[p--] == 1)
            {
                lx_leds[i] = CRGB::White; // 流星颜色
            }
            else
            {
                lx_leds[i] = CRGB::Black;
            }
        }
    }
}

//双流星
void lx_double_effect()
{
    //功能变了，不用了
}

#pragma endregion 流星灯效果 =======================




/**********************************************  10ms task  *******************************************************/
void xk_effect()
{
    switch (xk_para.xk_mode)
    {
        case MODE_STATIC:
            // 静态模式效果处理
            for(int i = 0; i < XK_LED_NUM; i++) 
            {
                xk_para.xk_end_colors[i] = xk_para.xk_custom_colors[i];//这个数组里我只保存了收到的rgb
                xk_para.xk_end_colors[i].nscale8_video(xk_para.xk_L_actual);//L_actual是算好了的0-255
            }
            break;
        case MODE_BREATH_BW:
            // 蓝白呼吸效果处理
            blue_white_breath_effect();
            break;
        case MODE_BREATH_GAP:
            // 间隔灯珠呼吸效果处理
            gap_led_breath_effect();
            break;
        case MODE_MUSIC:
            // 跟随音乐节奏效果处理
            break;
        case MODE_WELCOME_1:
        case MODE_WELCOME_2:
            // 迎宾效果处理
            welcome_effect();
            break;
        case MODE_BREATH_STATIC:
            // 单色呼吸效果处理
            static_breath_effect();
            break;
        case MODE_BREATH_RANDOM:
            // 随机呼吸效果处理
            random_breath_effect();
            break;
        default:
            break;
    }
    //end -> leds
    for(int i = 0; i < xk_para.xk_led_num; i++) 
    {
        xk_leds[i] = xk_para.xk_end_colors[i];
    }
}

void lx_effect()
{
    lx_para.lx_mode = MODE_DOUBLE;//test

    switch (lx_para.lx_mode)
    {
        case MODE_DEFAULT:
            lx_default_effect();
            break;
        case MODE_RANDOM:
            lx_random_effect();
            break;
        case MODE_DOUBLE:
            lx_double_effect();
            break;
        default:
            break;
    }
}   

void _10_ms_task(void)
{
    xk_effect();
    lx_effect();
    FastLED.show();
}

