#include <Arduino.h>
#include <U8g2lib.h>

int BUTTON_0 = 17;
int BUTTON_1 = 19;
int BUTTON_2 = 23;
int BUTTON_3 = 25;
int BUTTON_4 = 21;

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
uint32_t tds_set_value = 3000;
uint8_t  retval_tds = 1;

void setup(void) {
  u8g2.begin(/*Select=*/ 17, /*Right/Next=*/ 21, /*Left/Prev=*/ 27, /*Up=*/ 23, /*Down=*/ 25, /*Home/Cancel=*/ 19); // Arduboy 10 (Production)
  Serial.begin(9600);
  u8g2.enableUTF8Print();
  u8g2.setColorIndex(1);
  u8g2.setFont(u8g2_font_profont11_tr);
}

uint8_t input_interface() {
  boolean is_update = true;
  u8g2.clearBuffer();
  do {
    String tds_set_value_string = "";
    if (is_update == true) {
      Serial.println("update");
      u8g2.drawStr(4, 10, "  SET TDS POINT:");
      u8g2.drawFrame(0, 26, 128, 20);
      is_update = false;
      tds_set_value_string.concat(tds_set_value);
      int start_tds = 55 - tds_set_value_string.length()%2;
      u8g2.drawStr(start_tds, 38, tds_set_value_string.c_str());
      delay(100);
    }
    
    int buttotnStatus0 = digitalRead(BUTTON_0);
    if (buttotnStatus0 == LOW) {
      delay(100);
      return 1;
    }
    int buttotnStatus1 = digitalRead(BUTTON_1);
    if (buttotnStatus1 == LOW) {
      delay(100);
      return 0;
    }
    int buttotnStatus2 = digitalRead(BUTTON_2);
    if (buttotnStatus2 == LOW) {
      tds_set_value++;
      is_update = true;
      delay(100);
    }
    int buttotnStatus3 = digitalRead(BUTTON_3);
    if (buttotnStatus3 == LOW) {
      tds_set_value--;
      is_update = true;
      delay(100);
    }
  } while ( not u8g2.nextPage());
}

void loop(void) {
  u8g2.firstPage();
  do { 
    Serial.println("Start");
    retval_tds = input_interface();
    
  } while ( u8g2.nextPage() );
}
