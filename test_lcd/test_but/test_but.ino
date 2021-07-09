int button = 11;
int led = 13;
void setup() {
  pinMode(button, INPUT);  //Cài đặt chân D11 ở trạng thái đọc dữ liệu
  pinMode(led,OUTPUT); // Cài đặt chân D2 dưới dạng OUTPUT
}

void loop() {
  int buttonStatus = digitalRead(button);    //Đọc trạng thái button
  if (buttonStatus == HIGH) { // Nếu mà button bị nhấn
    digitalWrite(led,HIGH); // Đèn led sáng
  } else { // ngược lại
    digitalWrite(led,LOW);
  }
}
