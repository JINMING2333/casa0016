#define IR_PIN_LEFT A0  // 左侧 IR 传感器
#define IR_PIN_RIGHT A1 // 右侧 IR 传感器
#define THRESHOLD 10    // 距离变化阈值 (cm)
#define LED_PIN 13      // 提示用 LED

void setup() {
  pinMode(IR_PIN_LEFT, INPUT);
  pinMode(IR_PIN_RIGHT, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // 读取 IR 传感器的距离
  int distanceLeft = analogRead(IR_PIN_LEFT);
  int distanceRight = analogRead(IR_PIN_RIGHT);

  // 将模拟值转换为距离 (根据 IR 传感器数据表调整)
  float leftDist = map(distanceLeft, 0, 1023, 0, 50);  // 假设最大检测范围为 50cm
  float rightDist = map(distanceRight, 0, 1023, 0, 50);

  Serial.print("Left Distance: ");
  Serial.print(leftDist);
  Serial.print(" cm, Right Distance: ");
  Serial.print(rightDist);
  Serial.println(" cm");

  // 检测翘二郎腿
  if (abs(leftDist - rightDist) > THRESHOLD) {
    digitalWrite(LED_PIN, HIGH); // 翘二郎腿提示
    Serial.println("翘二郎腿检测！");
  } else {
    digitalWrite(LED_PIN, LOW);  // 正常坐姿
    Serial.println("坐姿正常");
  }

  delay(5000); // 每 500 毫秒检测一次
}
