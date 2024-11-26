#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_Accel.h>
//#include <Adafruit_LSM303_U.h>

#define LED_PIN 13  // 假设使用 13 号引脚控制 LED

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified();

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // 初始化加速度计
  if (!accel.begin()) {
    Serial.println("无法找到 LSM303加速度计!");
    while (1);  // 如果无法初始化传感器，停在这里
  }

  pinMode(LED_PIN, OUTPUT);  // 初始化 LED 引脚
  digitalWrite(LED_PIN, LOW);  // 初始状态 LED 关闭
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // 获取 Z 轴加速度（假设坐标轴方向正确）
  float zAcceleration = event.acceleration.z;

  // 设定判断阈值，假设正确坐姿时 Z 轴加速度接近重力加速度（9.81 m/s²）
  float threshold = 7.0;  // 可以根据实际情况调整该阈值

  // 判断坐姿是否正确
  if (zAcceleration < threshold) {
    // 如果 Z 轴加速度小于阈值，认为坐姿不正确，点亮 LED
    digitalWrite(LED_PIN, HIGH);
    Serial.println("坐姿不正确，LED 提示");
  } else {
    // 如果坐姿正确，关闭 LED
    digitalWrite(LED_PIN, LOW);
    Serial.println("坐姿正确");
  }

  delay(500);  // 每 500 毫秒检查一次
}
