#include <math.h>
#include <EEPROM.h>
#include "GravityTDS.h"

#define TdsSensorPin A1
GravityTDS gravityTds;

float temperature = 25,tdsValue = 0;
float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
float T,logRt,Tf,Tc;

void setup()
{
    Serial.begin(115200);
    Serial.println("Ready");
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
}

float Thermistor(int Vo) {
    logRt = log(1000.0*((1024.0/Vo-1))); 
    T = (1.0 / (A + B*logRt + C*logRt*logRt*logRt));  // We get the temperature value in Kelvin from this Stein-Hart equation
    Tc = T - 273.15;                     // Convert Kelvin to Celcius
    // Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
    return Tc;
}

void loop()
{
    temperature = 25; //Thermistor(analogRead(5));  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    delay(1000);
}
