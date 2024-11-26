#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FreeSansBold7pt7b.h"
#include "Typographica_Blp58pt7b.h"
#include "SparkyStonesRegular_BW6ld9pt7b.h"

// OLED 显示屏设置
#define SCREEN_WIDTH 128  // OLED 屏幕宽度
#define SCREEN_HEIGHT 32 // OLED 屏幕高度
#define OLED_RESET    -1 // 如果没有硬件复位引脚，则设为 -1

// 使用扫描到的正确地址 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);  // 等待串口连接
  }

  Serial.println("Initializing OLED display...");

  Wire.begin(); // 初始化 I2C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 这里使用扫描到的地址 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // 初始化失败时进入死循环
  }

  display.display();
  delay(2000);  // 等待 2 秒

  // 显示欢迎信息
  display.clearDisplay();
  display.setTextSize(1);  // 设置字体大小
  display.setTextColor(SSD1306_WHITE);  // 设置字体颜色
  display.setFont(&SparkyStonesRegular_BW6ld9pt7b);
  display.setCursor(0, 10);  // 设置光标位置
  display.println(F("Welcome to"));  // 显示文本
  display.setCursor(0, 28);  // 设置光标位置
  display.println(F("smart chair!"));  // 显示文本
  display.display();  // 更新显示
  delay(2000);  // 等待 2 秒
}

void loop() {
  // 显示 "Sitting for X seconds!"
  display.clearDisplay();  // 清空屏幕
  display.setFont(&SparkyStonesRegular_BW6ld9pt7b);
  display.setCursor(0, 28);  // 设置光标位置
  display.print(F("Sitting for "));
  display.print(millis() / 1000); // 显示秒数
  display.println(F(" s!"));
  display.display();  // 更新显示
  delay(1000);  // 每 1 秒更新一次

  // 模拟坐姿错误，显示 "Wrong Posture!"
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(F("Wrong Posture!"));
  display.setCursor(0, 28);
  display.println(F("Don't Cross Legs!"));
  display.display();
  delay(2000);

  // 显示 "Break time soon!"
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(F("Break time soon!"));
  display.display();
  delay(2000);

  // 显示 "Time to Stand up!"
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(F("Time to Stand Up!"));
  display.display();
  delay(2000);
}
