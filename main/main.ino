#include <EEPROM.h>
int TDS_CALIB_ADDRESS = 0;
int TDS_SET_ADDRESS = 10;
int TDS_SET_PUMPS_ADDRESS = 40;
int PH_CALIB_ADDRESS = 20;
int PH_SET_ADDRESS = 30;
int PH_SET_PUMPS_ADDRESS = 50;

// define port
#define TEMP_PORT A0
#define PH_SENSOR_PORT A4
#define TDS_SENSOR_PORT A10

// Sleepping Time
unsigned long TDS_PUMPS_SLEEPING_TIME = 300000;
unsigned long pH_PUMPS_SLEEPING_TIME  = 300000;
unsigned long START_SLEEPING_TIME     = 120000;
boolean _is_first_start     = true;
boolean _is_tds_pump_sleep  = true;
boolean _is_pH_pump_sleep   = true;
unsigned long time_sleep_count  = millis();
unsigned long time_sleep_tds    = millis();
unsigned long time_sleep_pH     = millis();

unsigned long _time_pH_pumps_run = millis();
unsigned long _time_tds_pumps_run = millis();

int BUTTON_0 = 17;
int BUTTON_1 = 19;
int BUTTON_2 = 23;
int BUTTON_3 = 25;
int BUTTON_4 = 21;

// I2C lib
#include <Wire.h>
#include <math.h>
#include <EEPROM.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include "GravityTDS.h"

// RELAY
int RELAY_PH  = 5;
int RELAY_TDS = 15;
int RELAY_03  = 31;

// State relay
uint8_t state_relay_pH  = 1;
uint8_t state_relay_tds = 1;
uint8_t state_relay_03  = 1;

// pH metter initial
float     b;
int       buf[10], temp;
unsigned long int avgValue;

// TDS metter initial
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0;

// LCD Initial
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

// TEMP Initial
float     T, logRt, Tf, Tc;
float     A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;

// SENSOR Value
float     pHvalue = 0;
float     temperature = 25, real_tdsValue = 0, tdsValue = 0;

// SENSOR String Value
String    str_tmp = "";
String    str_tds_vl = "";
String    str_pH_value = "";

// Logic button
uint8_t   retval_tds        = 1;
uint8_t   retval_pump_tds   = 1;
uint32_t  tds_set_value     = 400;
uint32_t  time_pump_tds     = 3;

uint8_t   retval_pH         = 1;
float     pH_set_value      = 7.0;
uint8_t   retval_pump_pH    = 1;
uint32_t  time_pump_pH      = 3;

float     _kValue_tds       = 0.0;

String    current_scrren = "home";
unsigned long  count_time = millis();

// Menu list
const char *main_menu =
  "1.TDS (EC)           \n"
  "2.pH                 \n"
  "3.Manual Control     \n"
  "4.Calibration        ";

const char *tds_menu =
  "1.Setup TDS Value    \n"
  "2.Setup motor time   ";

const char *pH_menu =
  "1.Setup pH Value    \n"
  "2.Setup motor time  ";

const char *motor_menu =
  "1.Pumps pH           \n"
  "2.Pumps TDS          \n"
  "3.Pumps 03           ";

const char *on_off_menu =
  "ON                   \n"
  "OFF                  ";

const char *calib_menu =
  "1.Calibration TDS   \n"
  "2.Calibration pH    ";


// Selection Menu
uint8_t main_selection = 0;
uint8_t tds_selection = 0;
uint8_t pH_selection = 0;
uint8_t pump_selection = 0;
uint8_t on_off_pump = 0;
uint8_t calib_selection = 1;

// Other symbol
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

void setup()
{
  // LCD initial
  u8g2.begin(/*Select=*/ 17, /*Right/Next=*/ 21, /*Left/Prev=*/ 27, /*Up=*/ 23, /*Down=*/ 25, /*Home/Cancel=*/ 19); // Arduboy 10 (Production)
  u8g2.enableUTF8Print();
  u8g2.setColorIndex(1);

  // Initialize Serial
  Serial.begin(9600);
  Serial.println("Ready");

  // Initialize TDS
  pinMode(TDS_SENSOR_PORT, INPUT);

  // Initialize Button
  pinMode(BUTTON_0, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(RELAY_PH, OUTPUT);
  pinMode(RELAY_TDS, OUTPUT);
  pinMode(RELAY_03, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), load_menu, LOW);

  _kValue_tds = readStringFromEEPROM(TDS_CALIB_ADDRESS).toFloat();
  tds_set_value = readStringFromEEPROM(TDS_SET_ADDRESS).toFloat();
  pH_set_value = readStringFromEEPROM(PH_SET_ADDRESS).toFloat();
  time_pump_pH = readStringFromEEPROM(PH_SET_PUMPS_ADDRESS).toInt();
  time_pump_tds = readStringFromEEPROM(TDS_SET_PUMPS_ADDRESS).toInt();
}

void load_menu() {
  detachInterrupt(digitalPinToInterrupt(BUTTON_1));
  delay(100);
  for (;;) {
    int interrupts_status = digitalRead(BUTTON_1);
    if (interrupts_status == HIGH)
      break;
  }
  if (current_scrren == "home")
    current_scrren = "main_menu";
}

float pHmetter()
{
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(PH_SENSOR_PORT);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;
  return phValue;
}

// Read Temperature senor
float Thermistor(int Vo) {
  logRt = log(1000.0 * ((1024.0 / Vo - 1)));
  T = (1.0 / (A + B * logRt + C * logRt * logRt * logRt)); // We get the temperature value in Kelvin from this Stein-Hart equation
  Tc = T - 273.15;                     // Convert Kelvin to Celcius
  // Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
  return Tc;
}

// Read TDS sensor
float read_tds_sensor() {
  do {
    analogBuffer[analogBufferIndex] = analogRead(TDS_SENSOR_PORT);    //read the analog value and store into the buffer
    analogBufferIndex++;
  } while (analogBufferIndex < SCOUNT);
  analogBufferIndex = 0;

  for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
  averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVolatge = averageVoltage / compensationCoefficient;
  real_tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;

  return real_tdsValue;
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void read_sensor_value() {
  str_pH_value = "";
  pHvalue = pHmetter();
  str_pH_value.concat(pHvalue);

  str_tds_vl = "";
  tdsValue = read_tds_sensor() * _kValue_tds;
  int tds_vl = round(tdsValue);
  str_tds_vl.concat(tds_vl);

  str_tmp = "";
  int tmp = round(Thermistor(analogRead(TEMP_PORT)));
  str_tmp.concat(tmp);
}

void auto_control_relay() {
  if (pHvalue < (pH_set_value - 0.1)) {
    if (_is_pH_pump_sleep == false) {
      digitalWrite(RELAY_PH, HIGH);
      delay(100);
      _is_pH_pump_sleep = true;
      _time_pH_pumps_run = millis();
      Serial.println("pH START");
      time_sleep_pH = millis();
    }
  }
  Serial.print("TDS Value: "); Serial.print(tdsValue); Serial.print(" "); Serial.println(tds_set_value);
  if (tdsValue < (tds_set_value - 10)) {
    if (_is_tds_pump_sleep == false) {
      digitalWrite(RELAY_TDS, HIGH);
      delay(100);
      _is_tds_pump_sleep = true;
      _time_tds_pumps_run = millis();
      time_sleep_tds = millis();
      Serial.println("TDS START");
    }
  }
}

void check_stop_pumps() {
  int _is_tds_relay_start = digitalRead(RELAY_TDS);
  if (_is_tds_relay_start == HIGH){
    if ((unsigned long) (millis() - _time_tds_pumps_run) > (time_pump_tds * 1000)){
      digitalWrite(RELAY_TDS, LOW);
      Serial.println("TDS STOP");
    }
  }
  int _is_pH_relay_start = digitalRead(RELAY_PH);
  if (_is_pH_relay_start == true){
    if ((unsigned long) (millis() - _time_pH_pumps_run) > (time_pump_pH * 1000)) {
      digitalWrite(RELAY_PH, LOW);      
      Serial.println("pH STOP");
    }
  }
}

void loop() {
  u8g2.firstPage();
  do {
    if (current_scrren == "home") {
      read_sensor_value();
      draw_value(str_tds_vl, str_tmp, str_pH_value);
      check_stop_pumps();

      if (_is_first_start == true) {
        if ((unsigned long) (millis() - time_sleep_count) > START_SLEEPING_TIME){
          _is_first_start = false;
        }
      } else {
        if ((unsigned long) (millis() - time_sleep_tds) > TDS_PUMPS_SLEEPING_TIME) {
          _is_tds_pump_sleep = false;
        }
        if ((unsigned long) (millis() - time_sleep_pH) > pH_PUMPS_SLEEPING_TIME) {
          _is_pH_pump_sleep = false;
        }
        auto_control_relay();
      }
      Serial.print(_is_tds_pump_sleep); Serial.print(" pH: "); Serial.println(_is_pH_pump_sleep); 
      delay(1000);
    } else {
      draw_menu();
    }
  } while ( u8g2.nextPage() );
}

// DRAW UI ------------------------------------------------------------

void draw_cover_box() {
  u8g2.drawFrame(0, 0, 64, 33);
  u8g2.drawFrame(63, 0, 64, 33);
  u8g2.drawFrame(0, 32, 64, 32);
  u8g2.drawFrame(63, 32, 64, 32);
}

void draw_base_text() {
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawUTF8(98, 27, DEGREE_SYMBOL);

  u8g2.setFont(u8g2_font_profont11_tr);

  String tds_set_value_main = "";
  tds_set_value_main = String(tds_set_value);
  String tds_set_value_base_main = "TDS (" + tds_set_value_main + ")";
  u8g2.drawStr(4, 10, tds_set_value_base_main.c_str());

  u8g2.drawStr(44, 25, "PPM");
  u8g2.drawStr(66, 10, "Temp.");
  u8g2.drawStr(105, 23, "C");

  String pH_set_value_main = ""; 
  pH_set_value_main = String(pH_set_value, 1);
  String pH_set_value_base_main = "pH (" + pH_set_value_main + ")";
  u8g2.drawStr(4, 42, pH_set_value_base_main.c_str());
}

void draw_value(String tds_val, String tmp_val, String pH_val) {
  u8g2.clearBuffer();
  draw_cover_box();
  draw_base_text();
  u8g2.setFont(u8g2_font_t0_15b_mn);
  int tds_start = 34 - (tds_val.length() - 1) * 8;
  u8g2.drawStr(tds_start, 27, tds_val.c_str());
  int tmp_start = 90 - (tmp_val.length() - 1) * 8;
  u8g2.drawStr(tmp_start, 27, tmp_val.c_str());
  int pH_start = 40 - (pH_val.length() - 1) * 8;
  u8g2.drawStr(pH_start, 58, pH_val.c_str());
}

void draw_menu() {
  u8g2.setFont(u8g2_font_profont11_tr);
  if (current_scrren == "main_menu") {
    main_selection = u8g2.userInterfaceSelectionList(
                       "MAIN MENU",
                       main_selection,
                       main_menu);
    if (main_selection == 0) {
      current_scrren = "home";
      EIFR = 1;
      delay(100);
      attachInterrupt(digitalPinToInterrupt(BUTTON_1), load_menu, LOW);
      EIFR = 0;
    } else if (main_selection == 1)
      current_scrren = "tds_menu";
    else if (main_selection == 2)
      current_scrren = "pH_menu";
    else if (main_selection == 3)
      current_scrren = "pump_menu";
    else if (main_selection == 4)
      current_scrren = "calib_menu";
  }

  if (current_scrren == "tds_menu") {
    tds_selection = u8g2.userInterfaceSelectionList(
                      "TDS MENU",
                      tds_selection,
                      tds_menu);
    if (tds_selection == 0)
      current_scrren = "main_menu";
    else {
      if (tds_selection == 1)
        retval_tds = input_interface_tds();
      else if (tds_selection == 2)
        retval_pump_tds = input_interface_pump_tds();
      current_scrren = "tds_menu";
    }
  }

  if (current_scrren == "pH_menu") {
    pH_selection = u8g2.userInterfaceSelectionList(
                     "pH MENU",
                     pH_selection,
                     pH_menu);
    if (pH_selection == 0)
      current_scrren = "main_menu";
    else {
      if (pH_selection == 1)
        retval_pH = input_interface_pH();
      else if (pH_selection == 2)
        retval_pump_pH = input_interface_pump_pH();
      current_scrren = "pH_menu";
    }
  }

  if (current_scrren == "pump_menu") {
    pump_selection = u8g2.userInterfaceSelectionList(
                       "Pumps MENU",
                       pump_selection,
                       motor_menu);
    if (pump_selection == 0)
      current_scrren = "main_menu";
    else { 
      if (pump_selection == 1) {
        state_relay_pH = u8g2.userInterfaceSelectionList(
                       "Pumps pH MENU",
                       on_off_pump,
                       on_off_menu);
        if (state_relay_pH == 1)
          digitalWrite(RELAY_PH, HIGH);
        else if (state_relay_pH == 2) 
          digitalWrite(RELAY_PH, LOW);
        delay(200);
      } else if (pump_selection == 2) {
        state_relay_tds = u8g2.userInterfaceSelectionList(
                       "Pumps TDS MENU",
                       on_off_pump,
                       on_off_menu);
        if (state_relay_tds == 1)
          digitalWrite(RELAY_TDS, HIGH);
        else if (state_relay_tds == 2) 
          digitalWrite(RELAY_TDS, LOW);           
        delay(200);
      }
      else if (pump_selection == 3) {
        state_relay_03 = !state_relay_03;
        digitalWrite(RELAY_03, state_relay_03);
        delay(200);
      }
      current_scrren = "pump_menu";
    }
  }

  if (current_scrren == "calib_menu") {
    calib_selection = u8g2.userInterfaceSelectionList(
                        "Calibaration MENU",
                        calib_selection,
                        calib_menu);
    if (calib_selection == 0)
      current_scrren = "main_menu";
    else {
      if (calib_selection == 1) {
        _kValue_tds = 1000 / calib_interface_tds();
        String string_kValue_tds = String(_kValue_tds, 3);
        writeStringToEEPROM(TDS_CALIB_ADDRESS, string_kValue_tds);
        Serial.println(string_kValue_tds);
      }
      else if (calib_selection == 2)
      {}//retval_pump_pH = input_interface_pump_pH();
      // writeStringToEEPROM(PH_CALIB_ADDRESS, pH_Set_Value);
      current_scrren = "calib_menu";
    }
  }
}

float calib_interface_tds() {

  unsigned long time = millis();
  unsigned long total = 0;
  int _count = 0;
  
  u8g2.clearBuffer();
  u8g2.drawStr(18, 34, "Calibrating . . .");

  do {
    u8g2.clearBuffer();
    u8g2.drawStr(18, 34, "Calibrating . . .");

    total = total + read_tds_sensor();
    _count ++;

    if ((unsigned long) (millis() - time) > 30000) {
      return (total / _count);
    }
    delay(1000);
  } while ( not u8g2.nextPage());
}

uint8_t input_interface_tds() {
  boolean is_tds_update = true;
  uint32_t local_tds = tds_set_value;
  u8g2.clearBuffer();
  do {
    String tds_set_value_string = "";
    if (is_tds_update == true) {
      u8g2.drawStr(4, 10, "  SET TDS POINT:");
      u8g2.drawFrame(0, 26, 128, 20);
      is_tds_update = false;
      tds_set_value_string.concat(local_tds);
      int start_tds = 55 - tds_set_value_string.length() % 2;
      u8g2.drawStr(start_tds, 39, tds_set_value_string.c_str());
      delay(100);
    }

    int buttotnStatus0 = digitalRead(BUTTON_0);
    if (buttotnStatus0 == LOW) {
      tds_set_value = local_tds;
      String eeprom_string_tds_set_value = String(tds_set_value);
      writeStringToEEPROM(TDS_SET_ADDRESS, eeprom_string_tds_set_value);
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
      local_tds++;
      is_tds_update = true;
      delay(100);
    }
    int buttotnStatus3 = digitalRead(BUTTON_3);
    if (buttotnStatus3 == LOW) {
      local_tds--;
      is_tds_update = true;
      delay(100);
    }
  } while ( not u8g2.nextPage());
}

uint8_t input_interface_pump_tds() {
  boolean is_pump_tds_update = true;
  uint32_t local_pump_tds = time_pump_tds;
  u8g2.clearBuffer();
  do {
    String pump_tds_set_value_string = "";
    if (is_pump_tds_update == true) {
      u8g2.drawStr(4, 10, "  SET TDS Pump POINT:");
      u8g2.drawFrame(0, 26, 128, 20);
      is_pump_tds_update = false;
      pump_tds_set_value_string.concat(local_pump_tds);
      int start_pump_tds = 55 - pump_tds_set_value_string.length() % 2;
      u8g2.drawStr(start_pump_tds, 39, pump_tds_set_value_string.c_str());
      delay(100);
    }

    int pump_tds_buttotnStatus0 = digitalRead(BUTTON_0);
    if (pump_tds_buttotnStatus0 == LOW) {
      time_pump_tds = local_pump_tds;
      String eeprom_string_time_pump_tds = String(time_pump_tds);
      writeStringToEEPROM(TDS_SET_PUMPS_ADDRESS, eeprom_string_time_pump_tds);
      delay(100);
      return 1;
    }
    int pump_tds_buttotnStatus1 = digitalRead(BUTTON_1);
    if (pump_tds_buttotnStatus1 == LOW) {
      delay(100);
      return 0;
    }
    int pump_tds_buttotnStatus2 = digitalRead(BUTTON_2);
    if (pump_tds_buttotnStatus2 == LOW) {
      local_pump_tds++;
      is_pump_tds_update = true;
      delay(100);
    }
    int pump_tds_buttotnStatus3 = digitalRead(BUTTON_3);
    if (pump_tds_buttotnStatus3 == LOW) {
      local_pump_tds--;
      is_pump_tds_update = true;
      delay(100);
    }
  } while ( not u8g2.nextPage());
}

uint8_t input_interface_pH() {
  boolean is_pH_update = true;
  float local_pH = pH_set_value;
  u8g2.clearBuffer();
  do {
    String pH_set_value_string = "";
    if (is_pH_update == true) {
      u8g2.drawStr(4, 10, "  SET pH POINT:");
      u8g2.drawFrame(0, 26, 128, 20);
      is_pH_update = false;
      pH_set_value_string = String(local_pH, 1);
      int start_pH = 55 - pH_set_value_string.length() % 2;
      u8g2.drawStr(start_pH, 39, pH_set_value_string.c_str());
      delay(100);
    }

    int buttotnStatus0 = digitalRead(BUTTON_0);
    if (buttotnStatus0 == LOW) {
      pH_set_value = local_pH;
      String eeprom_string_pH_set_value = String(pH_set_value,1);
      writeStringToEEPROM(PH_SET_ADDRESS, eeprom_string_pH_set_value);
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
      local_pH = local_pH + 0.1;
      is_pH_update = true;
      delay(100);
    }
    int buttotnStatus3 = digitalRead(BUTTON_3);
    if (buttotnStatus3 == LOW) {
      local_pH = local_pH - 0.1;
      is_pH_update = true;
      delay(100);
    }
  } while ( not u8g2.nextPage());
}

uint8_t input_interface_pump_pH() {
  boolean is_pump_pH_update = true;
  uint32_t local_pump_pH = time_pump_pH;
  u8g2.clearBuffer();
  do {
    String pump_pH_set_value_string = "";
    if (is_pump_pH_update == true) {
      u8g2.drawStr(4, 10, "  SET pH Pump POINT:");
      u8g2.drawFrame(0, 26, 128, 20);
      is_pump_pH_update = false;
      pump_pH_set_value_string.concat(local_pump_pH);
      int start_pump_pH = 55 - pump_pH_set_value_string.length() % 2;
      u8g2.drawStr(start_pump_pH, 39, pump_pH_set_value_string.c_str());
      delay(100);
    }

    int pump_pH_buttotnStatus0 = digitalRead(BUTTON_0);
    if (pump_pH_buttotnStatus0 == LOW) {
      time_pump_pH = local_pump_pH;
      String eeprom_string_time_pump_pH = String(time_pump_pH);
      writeStringToEEPROM(PH_SET_PUMPS_ADDRESS, eeprom_string_time_pump_pH);
      delay(100);
      return 1;
    }
    int pump_pH_buttotnStatus1 = digitalRead(BUTTON_1);
    if (pump_pH_buttotnStatus1 == LOW) {
      delay(100);
      return 0;
    }
    int pump_pH_buttotnStatus2 = digitalRead(BUTTON_2);
    if (pump_pH_buttotnStatus2 == LOW) {
      local_pump_pH++;
      is_pump_pH_update = true;
      delay(100);
    }
    int pump_pH_buttotnStatus3 = digitalRead(BUTTON_3);
    if (pump_pH_buttotnStatus3 == LOW) {
      local_pump_pH--;
      is_pump_pH_update = true;
      delay(100);
    }
  } while ( not u8g2.nextPage());
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}
