#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// 创建两个传感器实例
Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();

// XSHUT 引脚定义
#define XSHUT1_PIN 7
#define XSHUT2_PIN 8

// 距离阈值 (毫米)
#define LEG_POSITION_THRESHOLD 50

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // 等待串口初始化
  }
  Serial.println("VL53L0X dual sensor test");

  // 初始化 XSHUT 引脚
  pinMode(XSHUT1_PIN, OUTPUT);
  pinMode(XSHUT2_PIN, OUTPUT);

  // 确保两个传感器都关闭
  digitalWrite(XSHUT1_PIN, LOW);
  digitalWrite(XSHUT2_PIN, LOW);
  delay(10);

  // 初始化第一个传感器
  digitalWrite(XSHUT1_PIN, HIGH);
  delay(10);
  if (!sensor1.begin(0x30)) {  // 为第一个传感器设置地址 0x30
    Serial.println("Failed to initialize sensor 1!");
    while (1);
  }
  Serial.println("Sensor 1 initialized at address 0x30");

  // 初始化第二个传感器
  digitalWrite(XSHUT2_PIN, HIGH);
  delay(10);
  if (!sensor2.begin(0x31)) {  // 为第二个传感器设置地址 0x31
    Serial.println("Failed to initialize sensor 2!");
    while (1);
  }
  Serial.println("Sensor 2 initialized at address 0x31");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure1, measure2;

  // 读取传感器 1 的数据
  sensor1.rangingTest(&measure1, false);
  if (measure1.RangeStatus != 4) { // 如果测量有效
    Serial.print("Sensor 1: ");
    Serial.print(measure1.RangeMilliMeter);
    Serial.print(" mm ");
  } else {
    Serial.print("Sensor 1: Out of range ");
  }

  // 读取传感器 2 的数据
  sensor2.rangingTest(&measure2, false);
  if (measure2.RangeStatus != 4) { // 如果测量有效
    Serial.print("Sensor 2: ");
    Serial.print(measure2.RangeMilliMeter);
    Serial.print(" mm ");
  } else {
    Serial.print("Sensor 2: Out of range ");
  }

  // 检测翘腿姿势
  if (measure1.RangeStatus != 4 && measure2.RangeStatus != 4) {
    int distanceDifference = abs(measure1.RangeMilliMeter - measure2.RangeMilliMeter);
    if (distanceDifference > LEG_POSITION_THRESHOLD) {
      Serial.println("Posture: Incorrect (Legs crossed)");
    } else {
      Serial.println("Posture: Correct");
    }
  } else {
    Serial.println("Posture: Cannot detect");
  }

  delay(500); // 延迟 500ms
}
