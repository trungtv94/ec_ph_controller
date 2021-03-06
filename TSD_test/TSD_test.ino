#define TdsSensorPin A10
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

void setup()
{
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);
}

void loop()
{
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    averageVoltage = getMedianNum(analogBuffer) * (float)VREF / 1024.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVolatge = averageVoltage / compensationCoefficient;
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5*2.41; //convert voltage value to tds value

    Serial.print(31 & 1);
    Serial.print("          voltage:  ");
    Serial.print(averageVoltage, 2);
    Serial.print(" V   ");
    Serial.print("TDS Value:  ");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");
    

  }
}
int getMedianNum(int bTab[])
{
  int i, j, bTemp;
  for (j = 0; j < SCOUNT - 1; j++)
  {
    for (i = 0; i < SCOUNT - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((SCOUNT & 1) > 0)
    bTemp = bTab[(SCOUNT - 1) / 2];
  else
    bTemp = (bTab[SCOUNT / 2] + bTab[SCOUNT / 2 - 1]) / 2;
  return bTemp;
}
