#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

int button0 = 3;

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

void setup(void) {
  u8g2.begin();
  u8g2.setFont(u8g2_font_bitcasual_tr);
  Serial.begin(9600);

  pinMode(button0, INPUT);
}

const char *string_list =
  "5 min               \n"
  "10 min              \n"
  "15 min              \n";
uint8_t current_selection = 0;
uint8_t current_page = 0;

void update_screen_selection_list(int curr_slect) {
  int curslect = u8g2.userInterfaceSelectionList(
                   "Select Timer",
                   curr_slect,
                   string_list);
}

void loop(void) {
  int buttonStatus0 = digitalRead(button0);
  if (buttonStatus0 == HIGH) {
    current_selection += 1;
    Serial.println(current_selection);
    update_screen_selection_list(current_selection);
    Serial.println(current_selection);
    while (buttonStatus0 == HIGH) {
      buttonStatus0 = digitalRead(button0);
      delay(100);
    }
  }

  //  if ( current_selection == 0 ) {
  //    u8g2.userInterfaceMessage(
  //      "Nothing selected.",
  //      "",
  //      "",
  //      " ok ");
  //  }
  //    else {
  //    u8g2.userInterfaceMessage(
  //  "Selection:",
  //  u8x8_GetStringLineStart(current_selection-1, string_list ),
  //  "",
  //  " ok \n cancel ");
  //
  //  }
}
