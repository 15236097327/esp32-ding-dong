/*
 * ===================================================
 *  🎯 项目1：温湿度显示器
 *  硬件：ESP32 + DHT11 + 0.96" OLED (SSD1306)
 *  功能：实时读取温湿度，显示在OLED屏幕上
 * ===================================================
 *
 * 【接线说明】（非常重要！照着接，别接错）
 *
 *   DHT11 温湿度传感器（3个脚的那个蓝色/白色小方块）：
 *   ┌──────────────────────────────────────┐
 *   │ DHT11 引脚    →    ESP32 引脚         │
 *   ├──────────────────────────────────────┤
 *   │ VCC (正极)    →    3.3V（红色线）      │
 *   │ DATA (数据)   →    GPIO 4 (D4)        │
 *   │ GND  (负极)   →    GND   （黑色线）    │
 *   └──────────────────────────────────────┘
 *   ⚠️ 注意：有些DHT11模块是4个脚，多的那个NC脚不用接
 *   ⚠️ 一定接3.3V，不要接5V！会烧坏！
 *
 *   OLED 显示屏 (0.96" SSD1306，通常4个脚)：
 *   ┌──────────────────────────────────────┐
 *   │ OLED 引脚     →    ESP32 引脚         │
 *   ├──────────────────────────────────────┤
 *   │ VCC (正极)    →    3.3V（红色线）      │
 *   │ GND  (负极)   →    GND   （黑色线）    │
 *   │ SCL  (时钟)   →    GPIO 22 (D22)      │
 *   │ SDA  (数据)   →    GPIO 21 (D21)      │
 *   └──────────────────────────────────────┘
 *   ⚠️ 有些OLED标注为VDD/GND/SCK/SDA，对应VCC/GND/SCL/SDA
 *
 *   连线技巧：
 *   - 面包板两边红蓝长条：红线接3.3V，蓝线接GND
 *   - 所有模块的VCC都插红线条，GND都插蓝线条
 *   - 信号线用不同颜色的杜邦线，方便区分
 */

// ==================== 引入库 ====================
// Arduino库就是别人写好的代码包，我们直接拿来用
#include <Wire.h>               // I2C通信协议（用来和OLED对话）
#include <Adafruit_GFX.h>       // 图形库（画点、线、文字）
#include <Adafruit_SSD1306.h>   // SSD1306 OLED屏幕驱动
#include <DHT.h>                // DHT11温湿度传感器驱动

// ==================== 定义常量 ====================
// 这些是配置参数，集中写在前面方便修改

// --- OLED 屏幕设置 ---
#define SCREEN_WIDTH  128   // OLED宽度：128像素
#define SCREEN_HEIGHT  64   // OLED高度：64像素
#define OLED_ADDRESS 0x3C   // OLED的I2C地址（大部分模块是这个，如果不行试试0x3D）

// --- DHT11 传感器设置 ---
#define DHTPIN   4          // DHT11数据脚接在 GPIO4
#define DHTTYPE  DHT11      // 传感器型号（DHT11和DHT22不一样，别搞混）

// --- 读取间隔 ---
#define READ_INTERVAL 2000  // 每2000毫秒（2秒）读取一次

// ==================== 创建对象 ====================
// "对象"可以理解为：我们把屏幕和传感器"注册"到程序里
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
DHT dht(DHTPIN, DHTTYPE);

// ==================== 全局变量 ====================
unsigned long lastReadTime = 0;   // 上次读取的时间戳
float temperature = 0;            // 当前温度（摄氏度）
float humidity = 0;               // 当前湿度（百分比）

// ==================== 初始化函数（只运行一次）====================
void setup() {
  // 1. 启动串口通信（用来在电脑上看调试信息，波特率115200）
  Serial.begin(115200);
  Serial.println("\n\n=================================");
  Serial.println("  温湿度显示器 启动中...");
  Serial.println("=================================");

  // 2. 初始化 DHT11 传感器
  dht.begin();
  Serial.println("[OK] DHT11 传感器已就绪");

  // 3. 初始化 OLED 显示屏
  //    SSD1306_SWITCHCAPVCC = 屏幕内部升压供电模式
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    // 如果初始化失败，卡在这里，串口会一直报错
    Serial.println("[错误] OLED 初始化失败！请检查接线！");
    Serial.println("  常见原因：");
    Serial.println("  1. VCC/GND 接反了");
    Serial.println("  2. SDA/SCL 接错脚了（SDA→D21, SCL→D22）");
    Serial.println("  3. 地址不对（试试把 OLED_ADDRESS 改成 0x3D）");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("[OK] OLED 屏幕已就绪");

  // 4. 清屏并显示启动画面
  display.clearDisplay();           // 清空显示缓冲区
  display.setTextColor(SSD1306_WHITE);  // OLED只有白色（其实是蓝色）

  // 显示标题（大号字）
  display.setTextSize(2);           // 字号：2倍大
  display.setCursor(10, 10);        // 光标位置 (x=10, y=10)
  display.println("DHT11");

  display.setTextSize(1);           // 字号：标准
  display.setCursor(25, 40);
  display.println("Starting...");

  display.display();                // 把缓冲区内容真正显示到屏幕上
  delay(2000);                      // 显示2秒启动画面

  Serial.println("[OK] 系统初始化完成，开始读取数据...\n");
}

// ==================== 主循环（不断重复运行）====================
void loop() {
  // 每隔 READ_INTERVAL 毫秒读取一次
  // millis() 返回从开机到现在经过的毫秒数
  if (millis() - lastReadTime < READ_INTERVAL) {
    return;  // 还没到时间，直接返回，不做事
  }
  lastReadTime = millis();  // 更新时间戳

  // --- 读取温湿度 ---
  // readHumidity() 和 readTemperature() 返回 float 类型的小数
  float h = dht.readHumidity();         // 读湿度（%）
  float t = dht.readTemperature();      // 读温度（℃），默认就是摄氏度

  // --- 检查读数是否有效 ---
  // isnan() = "is Not a Number"，如果传感器返回的不是数字，说明读失败了
  if (isnan(h) || isnan(t)) {
    Serial.println("[警告] DHT11 读取失败！");
    showError("Sensor Error", "Check DHT11 wiring");
    return;
  }

  // --- 保存有效数据 ---
  temperature = t;
  humidity = h;

  // --- 在电脑串口打印（方便调试）---
  Serial.print("温度: ");
  Serial.print(temperature);
  Serial.print(" °C  |  湿度: ");
  Serial.print(humidity);
  Serial.println(" %");

  // --- 更新 OLED 显示 ---
  updateDisplay();
}

// ==================== OLED 显示函数 ====================
void updateDisplay() {
  display.clearDisplay();         // 先清空屏幕

  // ---- 顶部标题栏 ----
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== Weather Monitor ===");

  // 画一条分割线（从(x=0,y=10)到(x=127,y=10)，颜色=白色）
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // ---- 温度显示区域（大号字体）----
  display.setTextSize(3);         // 3倍大字体
  display.setCursor(5, 16);      // 从 (5,16) 开始
  display.print(temperature, 1); // 显示温度，保留1位小数
  display.setTextSize(2);
  display.print(" C");           // 摄氏度符号

  // ---- 湿度显示区域 ----
  // 用一个简单的图标表示湿度 (💧的替代)
  display.setTextSize(2);
  display.setCursor(5, 44);
  display.print("RH:");
  display.setTextSize(2);
  display.print(humidity, 1);    // 显示湿度，保留1位小数
  display.print("%");

  // ---- 右下角状态指示 ----
  display.setTextSize(1);
  display.setCursor(80, 56);

  // 根据温湿度给出中文提示（OLED不直接支持中文，用英文简写）
  String status = "";
  if (temperature > 30) {
    status = "HOT!";
  } else if (temperature < 15) {
    status = "COLD";
  } else {
    status = "OK";
  }

  // 湿度判断
  if (humidity > 70) {
    status = status + " Wet";
  } else if (humidity < 30) {
    status = status + " Dry";
  }

  display.print(status);

  // 刷新屏幕
  display.display();
}

// ==================== 错误提示画面 ====================
void showError(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);

  // 画一个矩形框
  display.drawRect(0, 0, 127, 63, SSD1306_WHITE);

  // 错误图标：X
  display.setTextSize(2);
  display.setCursor(55, 5);
  display.println(":(");

  // 错误信息
  display.setTextSize(1);
  display.setCursor(10, 30);
  display.println(line1);
  display.setCursor(10, 45);
  display.println(line2);

  display.display();
  delay(2000);
}
