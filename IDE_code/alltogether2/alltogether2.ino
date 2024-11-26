#include <Wire.h>
//#include <Adafruit_Sensor.h> //找不到对应的库
#include <Adafruit_LSM303_Accel.h>
#include <Servo.h>
//#include <LiquidCrystal_I2C.h> //LCD显示库
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // OLED 显示库
#include "FreeSansBold7pt7b.h"//自定义字体
#include <VL53L0X.h> // VL53L0X库

// 引脚定义
#define LDR_PIN A0            // 光敏电阻引脚
#define RED_PIN 5             // 红色 LED 引脚
#define GREEN_PIN 3           // 绿色 LED 引脚
#define BLUE_PIN 6            // 蓝色 LED 引脚
#define SERVO_PIN 9           // 舵机引脚
#define TILT_ANGLE 0         // 舵机倾斜角度
#define RESET_ANGLE 15         // 舵机初始角度
#define LIGHT_THRESHOLD 700   // 光敏电阻阈值
#define SIT_TIME_THRESHOLD 30000  // 坐下倒计时时间 20 秒
#define WARNING_TIME 5000     // 提示时间提前 5 秒
#define XSHUT1_PIN 12  // 传感器 1 的 XSHUT 引脚
#define XSHUT2_PIN 13  // 传感器 2 的 XSHUT 引脚
VL53L0X sensor1;
VL53L0X sensor2;
//#define POSTURE_THRESHOLD 0.3 // 坐姿偏移阈值（g）

// 初始化传感器和其他硬件
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(12345);
Servo myServo;
//LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C 地址 0x27 的 16x2 LCD

// OLED 显示屏的定义，128x32 的屏幕
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

//创建OLED显示对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 变量定义
unsigned long sitStartTime = 0; // 用户坐下的时间
bool userIsSitting = false;     // 是否有人在椅子上
bool warningShown = false;      // 是否已经提示用户
bool postureIncorrect = false;  // 是否坐姿不正确
bool legsCrossed = false;

// LED函数定义
void blinkLED(int times, int r, int g, int b);
void blinkLEDUntilPostureCorrect();  // 声明 blinkLEDUntilPostureCorrect 函数

unsigned long lastBlinkTime = 0;  // 上一次闪烁的时间
unsigned long blinkInterval = 250; // 闪烁的间隔时间

void setup() {
  // 初始化串口
  Serial.begin(9600);
  Serial.println("Serial test message");
  // 初始化舵机
  myServo.attach(SERVO_PIN); // 初始化舵机
  myServo.write(RESET_ANGLE); // 舵机复位

  // 初始化加速传感器
  if (!accel.begin()) {
    Serial.println("LSM303AGR not detected. Check connections!");
    while (1); {
      delay(1000); // 添加延迟，避免完全无响应
    } // 如果无法初始化传感器，停在这里
  }

  // 初始化RGB_LED引脚
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);

  // 初始化 LCD
  //lcd.init();
  //lcd.backlight();
  //lcd.print("Welcome to");
  //lcd.setCursor(0, 1);
  //lcd.print("Smart Seat!");

  // 初始化 OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1); {
      delay(1000); // 添加延迟，避免完全无响应
    }
  }
  display.clearDisplay();  // 清除显示
  display.setTextSize(1);  // 设置字体大小
  display.setTextColor(SSD1306_WHITE);  // 设置文字颜色
  display.setFont(&FreeSansBold7pt7b);
  display.setCursor(0,10);
  display.println(F("Hello ~"));
  display.setCursor(0,28);
  display.println(F("smart chair!"));
  display.display();  // 显示内容

  // 初始化 VL53L0X 传感器
  pinMode(XSHUT1_PIN, OUTPUT);
  pinMode(XSHUT2_PIN, OUTPUT);

  // 确保两个传感器都禁用
  digitalWrite(XSHUT1_PIN, LOW);
  digitalWrite(XSHUT2_PIN, LOW);
  delay(10);
  Serial.println("XSHUT pins initialized.");

  // 启用传感器 1 并设置地址
  digitalWrite(XSHUT1_PIN, HIGH);
  delay(10);
  if (!sensor1.init()) {
    Serial.println("Sensor 1 initialization failed!");
    while (1); {
      delay(1000); // 添加延迟，避免完全无响应
    }
  }
  Serial.println("Sensor 1 initialized.");
  sensor1.setAddress(0x30);  // 设置传感器 1 的新地址

  // 启用传感器 2 并设置地址
  digitalWrite(XSHUT2_PIN, HIGH);
  delay(10);
  if (!sensor2.init()) {
    Serial.println("Sensor 2 initialization failed!");
    while (1); {
      delay(1000); // 添加延迟，避免完全无响应
    }
  }
  Serial.println("Sensor 2 initialized.");
  sensor2.setAddress(0x31);  // 设置传感器 2 的新地址
}


void loop() {
  int lightLevel = analogRead(LDR_PIN); // 读取光敏电阻值
  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  sensors_event_t event;
  accel.getEvent(&event); // 读取加速度数据
  Serial.print("X: ");
  
  // 计算倾斜角度
  float tiltAngleSide = atan2(event.acceleration.x, event.acceleration.z) * 180 / PI;  // 左右倾斜角
  float tiltAngleFrontBack = atan2(event.acceleration.y, event.acceleration.z) * 180 / PI; // 前后倾斜角

  // 判断是否超过合理范围
  bool isSideTiltIncorrect = abs(tiltAngleSide) > 10; // 左右倾斜超出范围
  bool isFrontBackTiltIncorrect = abs(tiltAngleFrontBack) > 15; // 前后倾斜超出范围

  // 姿态检测
  // 获取 Z 轴加速度（假设坐标轴方向正确）
  ///float zAcceleration = event.acceleration.z;
  // 设定判断阈值，假设正确坐姿时 Z 轴加速度接近重力加速度（9.81 m/s²）
  ///float threshold = 7.0;  // 可以根据实际情况调整该阈值

  //检测双腿
  Serial.println("Reading sensor 1...");
  uint16_t distance1 = sensor1.readRangeSingleMillimeters();
  if (sensor1.timeoutOccurred()) {
    Serial.println("Sensor 1 timeout occurred!");
  } else {
    Serial.print("Sensor 1 Distance: ");
    Serial.println(distance1);
  }

  Serial.println("Reading sensor 2...");
  uint16_t distance2 = sensor2.readRangeSingleMillimeters();
  if (sensor2.timeoutOccurred()) {
    Serial.println("Sensor 2 timeout occurred!");
  } else {
    Serial.print("Sensor 2 Distance: ");
    Serial.println(distance2);
  }

  uint16_t legThreshold = 50; // 两腿间的距离变化阈值
  legsCrossed = abs(distance1 - distance2) > legThreshold;
    
  if (lightLevel <= LIGHT_THRESHOLD) { // 用户坐下
    if (!userIsSitting) {
      userIsSitting = true;
      sitStartTime = millis(); // 记录时间
      warningShown = false;
      postureIncorrect = false;

      Serial.println("User is sitting. Timer started.");
    } 

    // 记录已坐下时间
    unsigned long sittingDuration = millis() - sitStartTime;
    //lcd.setCursor(0, 0);
    //lcd.print("Sitting for ");
    //lcd.print(sittingDuration / 1000); // 显示秒数
    //lcd.print(" s");
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print(F("Sitting for "));
    display.print(sittingDuration / 1000); // 显示秒数
    display.println(F(" s!"));
    display.display();

    //是否翘二郎腿
    if (legsCrossed) {
      display.setCursor(0, 28);
      display.println(F("Don't Cross Legs!"));
      blinkLED(1, 60, 0, 0); // 闪烁白灯
    } else {
      display.setCursor(0, 28);
      display.println(F("Correct Posture!"));
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 255);
      analogWrite(BLUE_PIN, 255);
    }
    display.display();

    // 判断坐姿是否正确
    // if (zAcceleration < threshold) {
    //   if (!postureIncorrect) {
    //     postureIncorrect = true;
    //     // 启动闪烁，直到坐姿恢复
    //     lastBlinkTime = millis();  // 记录闪烁开始时间
    //     //lcd.setCursor(0, 10);
    //     //lcd.print("Wrong posture!");
    //     display.setCursor(0, 28);
    //     display.println(F("Wrong Posture!"));
    //     display.display();
    //     Serial.println("坐姿不正确，LED 提示");
    //   }
    // } else {
    //   // 如果坐姿正确，关闭 LED 闪烁
    //   if (postureIncorrect) {
    //     postureIncorrect = false;  // 结束闪烁
    //     analogWrite(RED_PIN, 255);
    //     analogWrite(GREEN_PIN, 255);
    //     analogWrite(BLUE_PIN, 255); // 关闭 LED
    //     //lcd.setCursor(0, 10);
    //     //lcd.print("Correct posture!");
    //     display.setCursor(0, 28);
    //     display.println(F("Correct Posture!"));
    //     display.display();
    //   }
    // }
    if (isSideTiltIncorrect || isFrontBackTiltIncorrect) {
        postureIncorrect = true;
        lastBlinkTime = millis();  // 记录闪烁开始时间
        display.setCursor(0, 28);
        display.println(F("Wrong Posture!"));
        display.display();
        Serial.println("坐姿不正确，LED 提示");
    } else {
        postureIncorrect = false;
        analogWrite(RED_PIN, 255);
        analogWrite(GREEN_PIN, 255);
        analogWrite(BLUE_PIN, 255); // 关闭提示灯
        display.setCursor(0, 28);
        display.println(F("Correct Posture!"));
        display.display();
    }

    // 提前 5 秒提醒
    if(sittingDuration >= SIT_TIME_THRESHOLD - WARNING_TIME && !warningShown && !postureIncorrect) {
      warningShown = true;
      blinkLED(2, 255, 255, 0); // 蓝色闪烁
      //lcd.setCursor(0, 10);  // 更新第二行显示
      //lcd.print("Break time soon!");
      display.setCursor(0, 28);  // 更新第二行显示
      display.println(F("Break time soon!"));
      display.display();
    }

    if (sittingDuration >= SIT_TIME_THRESHOLD) {
      // 超时提醒
      myServo.write(TILT_ANGLE);
      //lcd.clear();
      //lcd.print("Time to stand ~");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println(F("Time to Stand Up!"));
      display.display();
      Serial.println("超时警告");
    }

  } else { // 用户离开
    if (userIsSitting) {
      userIsSitting = false;
      myServo.write(RESET_ANGLE); // 重置舵机
      warningShown = false;
      postureIncorrect = false;

      //lcd.clear();
      //lcd.print("Welcome to");
      //lcd.setCursor(0, 10);
      //lcd.print("Smart Chair!");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println(F("Welcome to"));
      display.setCursor(0, 28);
      display.println(F("smart chair!"));
      display.display();
      Serial.println("User left. Timer reset. Reset position.");
    }
  }

  // 如果坐姿不正确，保持 LED 闪烁
  if (postureIncorrect) {
    blinkLEDUntilPostureCorrect();
  }

  delay(1000); // 每秒检测一次
}

// LED 闪烁函数
void blinkLED(int times, int r, int g, int b) {
  for (int i = 0; i < times; i++) {
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
    delay(250);
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 255);
    delay(250);
  }
} 

void blinkLEDUntilPostureCorrect() {
  unsigned long currentMillis = millis();
  
  // 每隔一段时间闪烁一次
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    analogWrite(RED_PIN, 0);  // 红色
    analogWrite(GREEN_PIN, 0);  // 绿色
    analogWrite(BLUE_PIN, 0);   // 蓝色
    delay(250);  // 闪烁一段时间
    analogWrite(RED_PIN, 255);    // 关闭红色
    analogWrite(GREEN_PIN, 255);  // 关闭绿色
    analogWrite(BLUE_PIN, 255);   // 关闭蓝色
    lastBlinkTime = currentMillis;  // 记录当前时间
  }

  sensors_event_t event;
  accel.getEvent(&event); // 重新读取加速度数据

  float zAcceleration = event.acceleration.z;
  if (zAcceleration >= 7.0) {  // 如果恢复正确坐姿
    postureIncorrect = false;
  }
}