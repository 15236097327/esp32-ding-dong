# 🌡️ 温湿度显示器 — 第一个ESP32项目

> **难度**：⭐ 入门级  
> **耗时**：30~60 分钟  
> **硬件**：ESP32 + DHT11 + 0.96" OLED

---

## 📦 所需硬件（从你的套件里拿）

| 硬件 | 数量 | 备注 |
|------|------|------|
| ESP32 开发板 | 1 块 | 核心大脑 |
| DHT11 模块 | 1 个 | 蓝色/白色小方块，3脚或4脚 |
| 0.96" OLED | 1 块 | SSD1306芯片，4个脚 |
| 面包板 | 1 块 | 830孔的那个 |
| M-M 杜邦线 | 4 根 | 公对公（两头都是针） |
| Micro USB 线 | 1 根 | 连接电脑和ESP32 |

---

## 🔌 接线图（文字版）

### 第一步：给面包板供电

把面包板侧面的红蓝长条接上电：
- **红线**：ESP32 `3.3V` → 面包板 `红色长条(+)`
- **蓝线**：ESP32 `GND` → 面包板 `蓝色长条(-)`

> 这样面包板上的红条全是3.3V正极，蓝条全是GND负极，后面的模块都从这里取电，方便！

### 第二步：接 DHT11 传感器

```
DHT11模块              ESP32
┌─────────┐          ┌──────────┐
│  VCC (+) │ ───红──→│  3.3V     │
│  DATA    │ ───黄──→│  GPIO 4   │
│  GND (-) │ ───黑──→│  GND      │
└─────────┘          └──────────┘
```

### 第三步：接 OLED 显示屏

```
OLED模块               ESP32
┌─────────┐          ┌──────────┐
│  VCC    │ ───红──→│  3.3V     │
│  GND    │ ───黑──→│  GND      │
│  SCL    │ ───绿──→│  GPIO 22  │
│  SDA    │ ───蓝──→│  GPIO 21  │
└─────────┘          └──────────┘
```

### 完整接线速查表

| ESP32 引脚 | 连接到的硬件 |
|-----------|-------------|
| 3.3V | DHT11 VCC + OLED VCC |
| GND | DHT11 GND + OLED GND |
| GPIO 4 | DHT11 DATA |
| GPIO 21 | OLED SDA |
| GPIO 22 | OLED SCL |

---

## 📚 安装必要的库（Arduino IDE 操作）

打开 Arduino IDE，按以下步骤安装4个库：

### 方法：工具 → 管理库 → 搜索 → 安装

| 序号 | 搜索关键词 | 库名称 | 作者 |
|------|-----------|--------|------|
| 1 | `DHT sensor library` | DHT sensor library | Adafruit |
| 2 | `Adafruit SSD1306` | Adafruit SSD1306 | Adafruit |
| 3 | `Adafruit GFX` | Adafruit GFX Library | Adafruit |
| 4 | `Adafruit Unified Sensor` | Adafruit Unified Sensor | Adafruit |

> ⚠️ 安装 DHT sensor library 时会提示安装依赖，全部点「Install All」即可。

---

## 🚀 上传代码到 ESP32

1. 用 Micro USB 线连接 ESP32 到电脑
2. Arduino IDE 里选择：
   - **工具 → 开发板 → ESP32 Arduino → ESP32 Dev Module**
   - **工具 → 端口 → COM?**（插上ESP32后多出来的那个）
3. 点击 **→（上传）** 按钮
4. 等待编译+上传（首次较慢，约1~2分钟）
5. 上传成功后，OLED 就会显示温湿度了！

---

## 🧪 测试方法

1. **哈一口气** 到 DHT11 上 → 湿度和温度应该都会上升
2. **用手捏住** DHT11 传感器 → 温度会慢慢上升
3. 打开 Arduino IDE 的 **工具 → 串口监视器**（波特率选115200）→ 能看到每秒打印的温度湿度数值

---

## ❗ 常见问题排查

| 现象 | 可能原因 | 解决方法 |
|------|---------|---------|
| OLED 不亮 | VCC/GND 接反 | 检查红色线是否接3.3V |
| OLED 亮但无文字 | SDA/SCL 接错 | SDA→D21, SCL→D22 |
| 显示 Sensor Error | DHT11 接线问题 | 检查 DATA 脚是否接GPIO4 |
| 上传失败 "Connecting..." | ESP32 没进入下载模式 | 按住 BOOT 键再点上传 |
| 温度显示 -999 或 0 | DHT11 库问题 | 确认库安装正确 |
| COM 端口不出现 | 驱动没装 | 下载 CH340/CP2102 驱动 |

### ESP32 常见 USB 芯片驱动

你的 ESP32 开发板大概率用的是以下两种 USB 转串口芯片之一：
- **CP2102**：https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- **CH340**：http://www.wch.cn/downloads/CH341SER_EXE.html

---

## 📖 代码里面学了什么

| 知识点 | 说明 |
|--------|------|
| `#include <...>` | 引入别人写好的库 |
| `#define` | 定义常量（不会变的量） |
| `setup()` | 初始化函数，开机只跑一次 |
| `loop()` | 主循环，反复跑 |
| `delay(ms)` | 等待多少毫秒 |
| `millis()` | 开机到现在的毫秒数（用于定时） |
| `Serial.println()` | 打印到电脑串口（调试用） |
| I2C 通信 | SDA(数据)+SCL(时钟)两线协议，OLED用 |
| GPIO | ESP32的可编程引脚 |

---

## 🎯 下一步可以玩

搞定这个后，可以接着做：
- 加 LED 指示灯：温度过高亮红灯
- 加蜂鸣器：温度超过35°C 报警
- 加 WiFi：数据传到手机看
- 就是清单里的第2~9个项目 😄
