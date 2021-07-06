#include <math.h>

float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
float T,logRt,Tf,Tc;
float Thermistor(int Vo) {
 logRt = log(1000.0*((1024.0/Vo-1))); 
 T = (1.0 / (A + B*logRt + C*logRt*logRt*logRt));  // We get the temperature value in Kelvin from this Stein-Hart equation
 Tc = T - 273.15;                     // Convert Kelvin to Celcius
// Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
 return Tc;
}

void setup(){
    Serial.begin(9600);
    Serial.println("Ready");
}

void loop() 
{
   Serial.println(Thermistor(analogRead(5)));
   delay(2000);

}
