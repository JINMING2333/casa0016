#include <Wire.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_LSM303DLH_Mag.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(30302);

void setup() {
  Serial.begin(9600);
  if (!accel.begin() || !mag.begin()) {
    Serial.println("无法找到 LSM303AGR 传感器!");
    while (1);
  }
}

void loop() {
  sensors_event_t a, m;
  accel.getEvent(&a);  // 读取加速度数据
  mag.getEvent(&m);    // 读取磁力计数据

  Serial.print("加速度 (m/s^2): X = "); Serial.print(a.acceleration.x);
  Serial.print(" \tY = "); Serial.print(a.acceleration.y);
  Serial.print(" \tZ = "); Serial.println(a.acceleration.z);

  Serial.print("磁场 (gauss): X = "); Serial.print(m.magnetic.x);
  Serial.print(" \tY = "); Serial.print(m.magnetic.y);
  Serial.print(" \tZ = "); Serial.println(m.magnetic.z);

  delay(500);
}
