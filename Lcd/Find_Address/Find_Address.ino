/*
  제목     : LCD I2C 주소 확인하기
  내용     : LCD에 고유 I2C 주소를 확인해봅시다.
*/
 
#include <Wire.h>
 
void setup()
{
  Wire.begin();
 
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\nI2C Scanner");
}
 
void loop()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);
}

/*
 * 출처
 * https://kocoafab.cc/tutorial/view/733
 */
