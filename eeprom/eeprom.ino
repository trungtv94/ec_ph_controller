#include <EEPROM.h>
void setup() {
  Serial.begin(9600);
    writeStringToEEPROM(0, "1.00");
  
    writeStringToEEPROM(10, "400");
    writeStringToEEPROM(20, "1.00");
    writeStringToEEPROM(30, "7.0");
  String eeprom_string_tds_set_value = "";
  eeprom_string_tds_set_value = String(400);
  writeStringToEEPROM(10, eeprom_string_tds_set_value);

  //Serial.print(eeprom_string_tds_set_value);
  Serial.println(readStringFromEEPROM(0).toFloat());
  Serial.println(readStringFromEEPROM(10).toInt());
  Serial.println(readStringFromEEPROM(20).toFloat());
  Serial.println(readStringFromEEPROM(30).toFloat());
  Serial.println(readStringFromEEPROM(40).toInt());
  Serial.println(readStringFromEEPROM(50).toInt());
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
