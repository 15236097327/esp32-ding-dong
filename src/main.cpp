/*
 * 光控小夜灯
 * 环境暗 → LED 自动亮起
 * 环境亮 → LED 自动熄灭
 * OLED 显示实时光照强度
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LIGHT_PIN  34    // 光敏电阻 AO → D34 (ADC)
#define LED_PIN    5     // LED → D5

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

int threshold = 2000;   // 光照阈值（低于此值=暗，开灯）
                        // 可调：越大约容易亮

void updateScreen(int light, bool isDark);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED 失败！");
    while (1) { delay(1000); }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("NIGHT");
  display.setCursor(15, 35);
  display.println("LIGHT");
  display.display();
  delay(1500);

  Serial.println("=== 光控小夜灯就绪 ===");
  Serial.print("阈值: ");
  Serial.println(threshold);
  Serial.println("用手遮住光敏电阻试试！\n");
}

void loop() {
  int light = analogRead(LIGHT_PIN);   // 读光照值 (0-4095)
                                        // 越暗 → 值越小
                                        // 越亮 → 值越大

  bool isDark = (light > threshold);

  // 控制 LED
  if (isDark) {
    digitalWrite(LED_PIN, HIGH);   // 暗 → 开灯
  } else {
    digitalWrite(LED_PIN, LOW);    // 亮 → 关灯
  }

  // 串口输出
  Serial.print("光照值: ");
  Serial.print(light);
  Serial.print("  |  ");
  Serial.println(isDark ? "暗 - LED ON" : "亮 - LED OFF");

  // OLED 显示
  updateScreen(light, isDark);

  delay(200);
}

void updateScreen(int light, bool isDark) {
  display.clearDisplay();

  // 标题
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (isDark) {
    display.println("=== NIGHT MODE ===");
  } else {
    display.println("=== DAY MODE ===");
  }
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // 光照数值
  display.setTextSize(2);
  display.setCursor(5, 16);
  display.print("Light:");
  display.setTextSize(2);
  display.setCursor(5, 35);
  display.print(light);

  // 用方块画亮度条
  int barLen = map(light, 0, 4095, 5, 120);
  display.fillRect(5, 55, barLen, 6, SSD1306_WHITE);

  // LED 状态
  display.setTextSize(1);
  display.setCursor(70, 20);
  if (isDark) {
    display.print("LED: ON ");
  } else {
    display.print("LED: OFF");
  }

  display.display();
}
