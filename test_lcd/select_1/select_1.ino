#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

void setup(void) {
  u8g2.begin(/*Select=*/ 3, /*Right/Next=*/ 4, /*Left/Prev=*/ 5, /*Up=*/ A0, /*Down=*/ 6, /*Home/Cancel=*/ 7); // Arduboy 10 (Production)
  Serial.begin(9600);
  u8g2.setFont(u8g2_font_profont11_tr);
}

const char *main_menu =
  "1.TDS (EC)           \n"
  "2.pH                 \n"
  "3.Manual Control     \n"
  "4.Made by Trung Tran ";

const char *tds_menu =
  "1.Setup TDS Value    \n"
  "2.Setup motor time   \n"
  "3.Manual Control     ";

const char *pH_menu =
  "1.Setup pH Value    \n"
  "2.Setup motor time   \n"
  "3.Manual Control     ";

const char *motor_menu =
  "1.Pumps 01           \n"
  "2.Pumps 02           \n"
  "3.Pumps 03           ";

uint8_t main_selection = 0;
uint8_t tds_selection = 0;
uint8_t pH_selection = 0;
uint8_t pump_selection = 0;

uint8_t inputVal;
uint8_t retVal;

void loop(void) {
  main_selection = u8g2.userInterfaceSelectionList(
                     "MAIN MENU",
                     main_selection,
                     main_menu);

  if ( main_selection == 1 ) {
    tds_selection = u8g2.userInterfaceSelectionList(
                      "TDS MENU",
                      tds_selection,
                      tds_menu);
    if (tds_selection == 1) {
      retVal = u8g2.userInterfaceInputValue(
                 "Select Voltage",
                 "DAC= ",
                 &inputVal, 0, 5, 1, " V");
      Serial.println(retVal);
      Serial.println(inputVal);
      // function to change value tds
    } else {
      if (tds_selection == 2) {
        // function to change value time running
      }
    }
  } else {
    if ( main_selection == 2 ) {
      tds_selection = u8g2.userInterfaceSelectionList(
                        "pH MENU",
                        pH_selection,
                        pH_menu);
    } else {
      if ( main_selection == 3 ) {
        tds_selection = u8g2.userInterfaceSelectionList(
                          "Pumps MENU",
                          pump_selection,
                          motor_menu);
      }
    }
  }
}

void set_tds_value() {

}

void section_choose(uint8_t selection, const char list) {
  if ( selection == 0 ) {
    u8g2.userInterfaceMessage(
      "Nothing selected.",
      "",
      "",
      " ok ");
  } else {
    u8g2.userInterfaceMessage(
      "Selection:",
      u8x8_GetStringLineStart(selection - 1, list),
      "",
      " ok \n cancel ");
  }
}
