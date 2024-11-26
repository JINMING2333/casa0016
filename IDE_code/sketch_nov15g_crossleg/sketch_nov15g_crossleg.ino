#define TRIG1 2 // 左侧传感器 Trig 引脚
#define ECHO1 3 // 左侧传感器 Echo 引脚
#define TRIG2 4 // 右侧传感器 Trig 引脚
#define ECHO2 5 // 右侧传感器 Echo 引脚

void setup() {
  Serial.begin(9600); // 初始化串口通信
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
}

float measureDistance(int trigPin, int echoPin) {
  // 发送超声波信号
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 读取回声信号
  long duration = pulseIn(echoPin, HIGH);
  // 将时间转换为距离（单位：厘米）
  float distance = (duration * 0.034) / 2;
  return distance;
}

void loop() {
  // 测量左腿和右腿的距离
  float leftDistance = measureDistance(TRIG1, ECHO1);
  float rightDistance = measureDistance(TRIG2, ECHO2);

  // 打印距离到串口监视器
  Serial.print("Left Distance: ");
  Serial.print(leftDistance);
  Serial.print(" cm, Right Distance: ");
  Serial.print(rightDistance);
  Serial.println(" cm");

  // 判断是否翘二郎腿
  if (abs(leftDistance - rightDistance) > 10) { // 距离差大于10cm视为翘腿
    Serial.println("Detected: Crossed Legs!");
  } else {
    Serial.println("Detected: Normal Sitting.");
  }

  delay(5000); // 每秒检测一次
}
