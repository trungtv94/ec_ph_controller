// define port
#define TEMP_PORT A5
#define TDS_SENSOR_PORT A1
#define PH_SENSOR_PORT A0

// I2C lib
#include <Wire.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <math.h>

// pH metter initial
float b;
int buf[10], temp;
float pHvalue = 0;
unsigned long int avgValue;

// TDS metter initial
GravityTDS gravityTds;
float temperature = 25, tdsValue = 0;

// LCD Initial
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

// TEMP Initial
float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
float T, logRt, Tf, Tc;

String readString = "";
unsigned long interval = millis();

void setup()
{
  // LCD initial
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setColorIndex(1);

  // Initialize Serial
  Serial.begin(9600);
  Serial.println("Ready");

  // Initialize TDS
  gravityTds.setPin(TDS_SENSOR_PORT);
  gravityTds.setAref(5.0);          //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);     //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();               //initialization

  // Waiting output port
  pinMode(13, OUTPUT);
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

float Thermistor(int Vo) {
  logRt = log(1000.0 * ((1024.0 / Vo - 1)));
  T = (1.0 / (A + B * logRt + C * logRt * logRt * logRt)); // We get the temperature value in Kelvin from this Stein-Hart equation
  Tc = T - 273.15;                     // Convert Kelvin to Celcius
  // Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
  return Tc;
}

float read_sensor_value() {

  temperature = 25; //Thermistor(analogRead(TEMP_PORT));

  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  return tdsValue;
}

void loop() {
  u8g2.firstPage();
  do {
    pHvalue = pHmetter();
    String str_pH_value = "";
    str_pH_value.concat(pHvalue);
    delay(100);

    int tds_vl = round(read_sensor_value());
    String str_tds_vl = "";
    str_tds_vl.concat(tds_vl);
    delay(100);

    int tmp = round(Thermistor(analogRead(TEMP_PORT)));
    String str_tmp = "";
    str_tmp.concat(tmp);
    delay(100);

    // draw_base();
    draw_value(str_tds_vl, str_tmp, str_pH_value);
    //draw_value("2345", "123", "7.6");
    delay(1700);
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
  u8g2.drawUTF8(100, 27, DEGREE_SYMBOL);

  u8g2.setFont(u8g2_font_profont11_tr);
  u8g2.drawStr(4, 10, "TDS (400)");
  u8g2.drawStr(44, 25, "PPM");
  u8g2.drawStr(66, 10, "Temp.");
  u8g2.drawStr(110, 23, "C");

  u8g2.drawStr(5, 42, "pH  (6.0)");
}

void draw_value(String tds_val, String tmp_val, String pH_val) {
  u8g2.clearBuffer();
  draw_cover_box();
  draw_base_text();

  Serial.print(pH_val); Serial.print(": "); Serial.println(pH_val.length());
  Serial.print(tds_val); Serial.print(": "); Serial.println(tds_val.length());
  Serial.print(tmp_val); Serial.print(": "); Serial.println(tmp_val.length());

  u8g2.setFont(u8g2_font_t0_15b_mn);

  int tds_start = 34 - (tds_val.length() - 1) * 8;
  u8g2.drawStr(tds_start, 27, tds_val.c_str());

  int tmp_start = 90 - (tmp_val.length() - 1) * 8;
  u8g2.drawStr(tmp_start, 27, tmp_val.c_str());

  int pH_start = 40 - (pH_val.length() - 1) * 8;
  u8g2.drawStr(pH_start, 58, pH_val.c_str());

}
