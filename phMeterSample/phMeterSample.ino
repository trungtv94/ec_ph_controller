unsigned long int avgValue; //Store the average value of the sensor feedback
int SensorPin = A4;
float     _acidVoltage      = 234.17;
float     _neutralVoltage   = 404.44;

float pH_avgVol = 0.0;
float phValue = 0.0;
int buf[10]; //buffer for read analog
void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println("Ready"); //Test the serial monitor
}

void get_avgVal()
{
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        int temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++) //take the average value of 6 center sample
    avgValue += buf[i];

  pH_avgVol = (float)avgValue / 6.0;
}

void loop()
{
  get_avgVal();
  Serial.print("Voltage: "); Serial.println(pH_avgVol);
  delay(1000);
  float slope = 3.0 / (_neutralVoltage - _acidVoltage);
  float intercept = 7.0 - _neutralVoltage * slope;
  phValue = slope * pH_avgVol + intercept;
  Serial.print("pH: "); Serial.println(phValue);
  
}
