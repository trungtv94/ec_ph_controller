#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//OLED selection SH1106
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);


void setup(void) {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
 u8g2.begin(/*Select=*/ 0, /*Right/Next=*/ 1, /*Left/Prev=*/ 2, /*Up=*/ 3, /*Down=*/ 4, /*Home/Cancel=*/ 5); // Arduboy DevKit

  u8g2.setFont(u8g2_font_6x12_tr);
}

const char *string_list = 
  "5 min\n"
  "10 min\n"
  "15 min\n";
uint8_t current_selection = 1;


void loop(void) {

  current_selection = u8g2.userInterfaceSelectionList(
    "Select Timer",
    current_selection, 
    string_list);

  if ( current_selection == 0 ) {
    u8g2.userInterfaceMessage(
  "Nothing selected.", 
  "",
  "",
  " ok ");
  } else {
    u8g2.userInterfaceMessage(
  "Selection:", 
  u8x8_GetStringLineStart(current_selection-1, string_list ),
  "",
  " ok \n cancel ");

  }
}
void blink_1x() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
void blink_2x() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
