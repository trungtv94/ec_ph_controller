#include <EEPROM.h>
int mode = 1;
int TDS_CALIB_ADDRESS = 0;
int TDS_SET_ADDRESS = 10;
int TDS_SET_PUMPS_ADDRESS = 20;
int PH_NEUTRAL_ADDRESS = 30;
int PH_ACID_ADDRESS = 40;
int PH_SET_ADDRESS = 50;
int PH_SET_PUMPS_ADDRESS = 60;

void setup() {
  Serial.begin(9600);
  if (mode == 1) {
    writeStringToEEPROM(TDS_CALIB_ADDRESS, "1.00");
    writeStringToEEPROM(TDS_SET_ADDRESS, "400");
    writeStringToEEPROM(TDS_SET_PUMPS_ADDRESS, "5");
    
    writeStringToEEPROM(PH_NEUTRAL_ADDRESS, "234.17");
    writeStringToEEPROM(PH_ACID_ADDRESS, "404.44");
    writeStringToEEPROM(PH_SET_ADDRESS, "6.9");
    writeStringToEEPROM(PH_SET_PUMPS_ADDRESS, "5");

    //    String eeprom_string_tds_set_value = "";
    //    eeprom_string_tds_set_value = String(400);
    //    writeStringToEEPROM(10, eeprom_string_tds_set_value);

    //Serial.print(eeprom_string_tds_set_value);
  }
  else {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
  }
  Serial.println(readStringFromEEPROM(TDS_CALIB_ADDRESS).toFloat());
  Serial.println(readStringFromEEPROM(TDS_SET_ADDRESS).toInt());
  Serial.println(readStringFromEEPROM(TDS_SET_PUMPS_ADDRESS).toInt());
  Serial.println(readStringFromEEPROM(PH_NEUTRAL_ADDRESS).toFloat());
  Serial.println(readStringFromEEPROM(PH_ACID_ADDRESS).toFloat());
  Serial.println(readStringFromEEPROM(PH_SET_ADDRESS).toFloat());
  Serial.println(readStringFromEEPROM(PH_SET_PUMPS_ADDRESS).toInt());
}

void loop() {}

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
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}
