#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_Accel.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// 引脚定义
#define LDR_PIN A0            // 光敏电阻引脚
#define RED_PIN 6             // 红色 LED 引脚
#define GREEN_PIN 5           // 绿色 LED 引脚
#define BLUE_PIN 3            // 蓝色 LED 引脚
#define SERVO_PIN 9           // 舵机引脚
#define TILT_ANGLE 60         // 舵机倾斜角度
#define RESET_ANGLE 0         // 舵机初始角度
#define LIGHT_THRESHOLD 250   // 光敏电阻阈值
#define SIT_TIME_THRESHOLD 20000  // 坐下倒计时时间 20 秒
#define WARNING_TIME 5000     // 提示时间提前 5 秒
//#define POSTURE_THRESHOLD 0.3 // 坐姿偏移阈值（g）

// 初始化传感器和其他硬件
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(12345);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C 地址 0x27 的 16x2 LCD

// 变量定义
unsigned long sitStartTime = 0; // 用户坐下的时间
bool userIsSitting = false;     // 是否有人在椅子上
bool warningShown = false;      // 是否已经提示用户
bool postureIncorrect = false;  // 是否坐姿不正确

// LED函数定义
void blinkLED(int times, int r, int g, int b);
void blinkLEDUntilPostureCorrect();  // 声明 blinkLEDUntilPostureCorrect 函数

void setup() {
  // 初始化舵机
  myServo.attach(SERVO_PIN); // 初始化舵机
  myServo.write(RESET_ANGLE); // 舵机复位

  // 初始化加速传感器
  if (!accel.begin()) {
    Serial.println("LSM303AGR not detected. Check connections!");
    while (1); // 如果无法初始化传感器，停在这里
  }

  // 初始化RGB_LED引脚
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);

  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Smart Seat!");

  // 初始化串口
  Serial.begin(9600);
}

void loop() {
  int lightLevel = analogRead(LDR_PIN); // 读取光敏电阻值
  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  sensors_event_t event;
  accel.getEvent(&event); // 读取加速度数据
  
  // 姿态检测
  // 获取 Z 轴加速度（假设坐标轴方向正确）
  float zAcceleration = event.acceleration.z;

  // 设定判断阈值，假设正确坐姿时 Z 轴加速度接近重力加速度（9.81 m/s²）
  float threshold = 7.0;  // 可以根据实际情况调整该阈值

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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sitting for ");
    lcd.print(sittingDuration / 1000); // 显示秒数
    lcd.print(" s");

    // 判断坐姿是否正确
    if (zAcceleration < threshold) {
      if (!postureIncorrect) {
        postureIncorrect = true;
        // 启动闪烁，直到坐姿恢复
        blinkLEDUntilPostureCorrect();
        lcd.setCursor(0, 1);
        lcd.print("Wrong posture!");
        Serial.println("坐姿不正确，LED 提示");
      }
    } else {
      // 如果坐姿正确，关闭 LED 闪烁
      if (postureIncorrect) {
        postureIncorrect = false;  // 结束闪烁
        analogWrite(RED_PIN, 255);
        analogWrite(GREEN_PIN, 255);
        analogWrite(BLUE_PIN, 255); // 关闭 LED
      }
      lcd.setCursor(0, 1);
      lcd.print("Correct posture!");
    }

    // 提前 5 秒提醒
    if(sittingDuration >= SIT_TIME_THRESHOLD - WARNING_TIME && !warningShown && !postureIncorrect) {
      warningShown = true;
      blinkLED(2, 255, 255, 0); // 蓝色闪烁
      lcd.setCursor(0, 1);  // 更新第二行显示
      lcd.print("Break time soon!");
    }

    if (sittingDuration >= SIT_TIME_THRESHOLD) {
      // 超时提醒
      myServo.write(TILT_ANGLE);
      lcd.clear();
      lcd.print("Time to stand ~");
      Serial.println("超时警告");
    }

  } else { // 用户离开
    if (userIsSitting) {
      userIsSitting = false;
      myServo.write(RESET_ANGLE); // 重置舵机
      warningShown = false;
      postureIncorrect = false;

      lcd.clear();
      lcd.print("Welcome to");
      lcd.setCursor(0, 1);
      lcd.print("Smart Seat!");
      Serial.println("User left. Timer reset. Reset position.");
    }
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
  while (postureIncorrect) {
    analogWrite(RED_PIN, 0);  // 红色
    analogWrite(GREEN_PIN, 0);  // 绿色
    analogWrite(BLUE_PIN, 0);   // 蓝色
    delay(250);  // 闪烁一段时间
    analogWrite(RED_PIN, 255);    // 关闭红色
    analogWrite(GREEN_PIN, 255);  // 关闭绿色
    analogWrite(BLUE_PIN, 255);   // 关闭蓝色
    delay(250);  // 闪烁一段时间

    sensors_event_t event;
    accel.getEvent(&event); // 重新读取加速度数据

    float zAcceleration = event.acceleration.z;
    if (zAcceleration >= 7.0) {  // 如果恢复正确坐姿
      postureIncorrect = false;
    }
  }
}
