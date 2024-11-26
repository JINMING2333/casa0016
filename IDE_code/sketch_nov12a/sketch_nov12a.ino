#include <Servo.h>

//#define SEAT_SENSOR_PIN 2   // 座位传感器连接的引脚
#define LIGHT_THRESHOLD 500  // 亮度阈值（可调）
#define LDR_PIN A0           // LDR 连接的模拟引脚

#define SERVO_PIN 9         // 舵机连接的PWM引脚
#define TILT_ANGLE 60       // 座椅倾斜的角度
#define RESET_ANGLE 0       // 座椅的初始角度
#define SIT_TIME_THRESHOLD 20000  // 倒计时时间（毫秒），即30秒

Servo myServo;
unsigned long sitStartTime = 0;   // 记录用户坐下时的时间
bool userIsSitting = false;       // 用户是否正在坐着

void setup() {
  //pinMode(SEAT_SENSOR_PIN, INPUT_PULLUP);  // 使用上拉电阻读取座位传感器状态
  myServo.attach(SERVO_PIN);               // 初始化舵机
  myServo.write(RESET_ANGLE);              // 将舵机位置设为初始角度
  Serial.begin(9600);                      // 初始化串口监视器，用于调试
}

void loop() {
  //bool seatOccupied = digitalRead(SEAT_SENSOR_PIN) == LOW;  // 检测座位上是否有人
  int lightLevel = analogRead(LDR_PIN);  // 读取光敏电阻的电压值
  Serial.print("Light Level: ");
  Serial.println(lightLevel);  // 输出光线强度到串口监视器

  if (lightLevel > LIGHT_THRESHOLD) {
    if (!userIsSitting) {
      // 如果检测到用户刚坐下，记录时间
      userIsSitting = true;
      sitStartTime = millis();
      Serial.println("User is sitting. Timer started.");
    } else {
      // 检查用户坐了多长时间
      unsigned long sittingDuration = millis() - sitStartTime;
      if (sittingDuration >= SIT_TIME_THRESHOLD) {
        // 如果超过30秒，将座椅倾斜
        myServo.write(TILT_ANGLE);
        Serial.println("Tilted position.");
        //delay(10000);
        //myServo.write(RESET_ANGLE);
      }
    }
  } else {
    if (userIsSitting) {
      // 如果用户中途起身，重置计时和舵机位置
      userIsSitting = false;
      myServo.write(RESET_ANGLE);
      Serial.println("User left. Timer reset. Reset position.");
    }
  }

  delay(1000);  // 避免频繁读取
}
