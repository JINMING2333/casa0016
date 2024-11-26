const int flexPin = A1;    // Flex Sensor 连接到 A1 引脚
const int threshold = 800;  // 设置一个阈值，当弯曲值大于该阈值时认为坐姿不正确

void setup() {
  Serial.begin(9600);
}

void loop() {
  int flexValue = analogRead(flexPin);  // 读取模拟值（0-1023）
  float voltage = flexValue * (5 / 1023.0);  // 将模拟值转换为电压
  
  Serial.print("flexValue: ");
  Serial.println(flexValue);

  // 根据电压判断坐姿是否正确
  if (flexValue > threshold) {
    Serial.println("correct posture!");  // 如果弯曲值大于阈值，则认为坐姿不正确
  } else {
    Serial.println("inCorrect posture!");    // 否则认为坐姿正确
  }

  delay(1000); // 延时500毫秒
}
