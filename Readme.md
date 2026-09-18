# 8x8 WS2812B LED 点阵动画演示

![](C:\Users\QROBOT\Desktop\新建文件夹\esp32_ws2812b_8x8\ws2812.JPG)

基于 Arduino 的 8×8 WS2812B（NeoPixel）LED 点阵动画程序，自动循环播放 6 种炫彩动画效果，无需任何按键或传感器控制。

## 硬件需求

| 硬件     | 说明                                                |
| ------ | ------------------------------------------------- |
| 主控板    | ESP32 / ESP32-S3（数据引脚为 GPIO 18，其他 Arduino 板需修改引脚） |
| LED 点阵 | 8×8 = 64 颗 WS2812B，GRB 色序，800KHz，**蛇形（S 形）排列**    |
| 电源     | 建议独立 5V 供电（64 颗全亮峰值电流可达 4A），与主控共地                 |

## 接线说明

```
WS2812B 点阵        主控板
─────────────       ─────────
DIN  ────────────►  GPIO 18
5V   ────────────►  5V（建议独立电源）
GND  ────────────►  GND（必须共地）
```

> 💡 建议在 DIN 线上串联一个 330Ω 电阻，并在点阵电源两端并联 1000μF 电容，以保护第一颗 LED。

## 动画效果

程序按以下顺序循环播放，**每种模式持续 10 秒**：

| 序号  | 模式              | 效果描述                      |
|:---:| --------------- | ------------------------- |
| 1   | 🌈 彩虹（RAINBOW）  | 整屏彩虹渐变，色相持续流动             |
| 2   | ☄️ 彗星（COMET）    | 一颗橙色彗星沿蛇形路径环绕，带 12 像素渐隐拖尾 |
| 3   | ✨ 星闪（SPARKLE）   | 蓝白色星星随机闪烁，错相位呼吸效果         |
| 4   | ⬛ 方框（SQUARE）    | 彩色方框从外向内逐层收缩，再展开循环        |
| 5   | ▭ 矩形（RECTANGLE） | 全宽矩形边框上下呼吸移动，青色系渐变        |
| 6   | ⭕ 圆形（CIRCLE）    | 圆形从中心 2×2 逐级扩大至全屏，紫粉色系    |

## 关键参数配置

参数定义在 `main.ino` 顶部的 `constexpr` 常量区，可直接修改：

```cpp
constexpr uint8_t  MATRIX_PIN        = 18;    // 数据引脚
constexpr uint8_t  WIDTH             = 8;     // 点阵宽度
constexpr uint8_t  HEIGHT            = 8;     // 点阵高度
constexpr uint8_t  BRIGHTNESS        = 85;    // 亮度 0~255（当前约 1/3）
constexpr uint32_t FRAME_INTERVAL_MS = 50;    // 帧间隔，50ms ≈ 20 FPS
constexpr uint32_t MODE_DURATION_MS  = 10000; // 每种动画持续时间 10s
```

> ⚠️ 亮度越高功耗越大：64 颗 LED 全白全亮约需 4A，请确保电源余量充足。

## 软件依赖

- **Adafruit NeoPixel 库**（通过 Arduino IDE 库管理器安装）
  - Arduino IDE → 工具 → 管理库 → 搜索 `Adafruit NeoPixel` → 安装

## 编译与烧录

1. 用 Arduino IDE 打开 `软件/main.ino`（确保位于同名文件夹内）
2. 选择对应的开发板型号和端口（如 `ESP32S3 Dev Module`）
3. 点击「上传」即可

## 代码结构

```
main.ino
├── enum AnimationMode          动画模式枚举（状态机）
├── xy()                        蛇形排列坐标 → 灯珠序号换算
├── setXY()                     带边界保护的坐标画点
├── drawRectangle()             空心矩形绘制（供方框/矩形动画复用）
├── showRainbow()               动画 1：彩虹
├── showComet()                 动画 2：彗星
├── showSparkle()               动画 3：星闪
├── showSquare()                动画 4：方框收缩
├── showRectangle()             动画 5：矩形呼吸
├── showCircle()                动画 6：圆形扩散
├── nextMode()                  切换到下一种动画并复位状态
├── setup()                     初始化点阵、亮度并清屏
└── loop()                      非阻塞主循环：模式计时 + 帧率控制
```

### 设计说明

- **蛇形坐标映射**：`xy()` 函数处理偶数行正向、奇数行反向的物理走线，上层动画代码统一使用逻辑坐标 `(x, y)`，无需关心物理排列
- **非阻塞调度**：全程使用 `millis()` 时间差判断，无 `delay()`，帧率与模式切换互不干扰
- **伽马校正**：颜色输出经 `gamma32()` 处理，视觉过渡更自然
- **边界保护**：`setXY()` 自动忽略越界坐标，动画代码可放心使用 `int8_t` 负数坐标

## 自定义扩展

- **新增动画**：在 `AnimationMode` 枚举中添加名称，编写 `showXxx()` 函数，并在 `loop()` 的 `switch` 中注册
- **修改切换时间**：调整 `MODE_DURATION_MS`
- **固定单种动画**：删除或注释 `loop()` 中的 `nextMode()` 触发条件即可

### 视频效果

【使用Schematik开发一个ESP32控制WSB2812动画】 https://www.bilibili.com/video/BV1huem63E9g/?share_source=copy_web&vd_source=7bf10437d889d07e12643b64c1742abf
