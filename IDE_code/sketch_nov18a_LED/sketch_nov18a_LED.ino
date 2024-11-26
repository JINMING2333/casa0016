#define RED_PIN 2
#define GREEN_PIN 4
#define BLUE_PIN 5

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  analogWrite(RED_PIN, 60); 
  analogWrite(GREEN_PIN, 60);
  analogWrite(BLUE_PIN, 60);
  delay(1000);
 
  analogWrite(RED_PIN, 60); 
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delay(1000);

  analogWrite(RED_PIN, 255); 
  analogWrite(GREEN_PIN, 60);
  analogWrite(BLUE_PIN, 255);
  delay(1000);

  analogWrite(RED_PIN, 255); 
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 60);
  delay(1000);

  analogWrite(RED_PIN, 60); 
  analogWrite(GREEN_PIN, 60);
  analogWrite(BLUE_PIN, 255);
  delay(1000);
}
