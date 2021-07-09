int button0 = 3;
int button1 = 4;
int button2 = 5;
int button3 = 6;
int button4 = 7;
void setup() {
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  Serial.begin(9600);
}

void loop() {
  int buttonStatus0 = digitalRead(button0);    //Đọc trạng thái button
  if (buttonStatus0 == LOW) { // Nếu mà button bị nhấn
    Serial.println("BUTTON 0");
    while (buttonStatus0 == LOW) {
      buttonStatus0 = digitalRead(button0);
      delay(200);
    }
  }
  int buttonStatus1 = digitalRead(button1);    //Đọc trạng thái button
  if (buttonStatus1 == LOW) { // Nếu mà button bị nhấn
    Serial.println("BUTTON 1");
    while (buttonStatus1 == LOW) {
      buttonStatus1 = digitalRead(button1);
      delay(200);
    }
  }
  int buttonStatus2 = digitalRead(button2);    //Đọc trạng thái button
  if (buttonStatus2 == LOW) { // Nếu mà button bị nhấn
    Serial.println("BUTTON 2");
    while (buttonStatus2 == LOW) {
      buttonStatus2 = digitalRead(button2);
      delay(200);
    }
  }
  int buttonStatus3 = digitalRead(button3);    //Đọc trạng thái button
  if (buttonStatus3 == LOW) { // Nếu mà button bị nhấn
    Serial.println("BUTTON 3");
    while (buttonStatus3 == LOW) {
      buttonStatus3 = digitalRead(button3);
      delay(200);
    }
  }
  int buttonStatus4 = digitalRead(button4);    //Đọc trạng thái button
  if (buttonStatus4 == LOW) { // Nếu mà button bị nhấn
    Serial.println("BUTTON 4");
    while (buttonStatus4 == LOW) {
      buttonStatus4 = digitalRead(button4);
      delay(200);
    }
  }
}
