#include "SPI.h"
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"




const int chipSelect = 10;  // used for Arduino

int co2Addr = 0x69;
// This has been shifted from the default 0x68 to the above using eeprom_change_K30.

RTC_PCF8523 rtc;


void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
  }
  
  Serial.println("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  Wire.begin();
  pinMode(13, OUTPUT); // address of the Arduino LED indicator
  Serial.println("Application Note AN-102: Interface Arduino to K-30");

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}


///////////////////////////////////////////////////////////////////
// Function : int readCO2()
// Returns : CO2 Value upon success, 0 upon checksum failure
// Assumes : - Wire library has been imported successfully.
// - LED is connected to IO pin 13
// - CO2 sensor address is defined in co2_addr
///////////////////////////////////////////////////////////////////
int readCO2()
{
  int co2_value = 0; // Store the CO2 value inside this variable.
  digitalWrite(13, HIGH); // turn on LED
  // On most Arduino platforms this pin is used as an indicator light.
  //////////////////////////
  /* Begin Write Sequence */
  //////////////////////////
  Wire.beginTransmission(co2Addr);
  Wire.write(0x22);
  Wire.write(0x00);
  Wire.write(0x08);
  Wire.write(0x2A);
  Wire.endTransmission();
  /////////////////////////
  /* End Write Sequence. */
  /////////////////////////
  /*
  Wait 10ms for the sensor to process our command. The sensors's
  primary duties are to accurately measure CO2 values. Waiting 10ms
  ensures the data is properly written to RAM
  */
  delay(10);
  /////////////////////////
  /* Begin Read Sequence */
  /////////////////////////
  /*
  Since we requested 2 bytes from the sensor we must read in 4 bytes.
  This includes the payload, checksum, and command status byte.
  */
  Wire.requestFrom(co2Addr, 4);
  byte i = 0;
  byte buffer[4] = {0, 0, 0, 0};
  /*
  Wire.available() is not necessary. Implementation is obscure but we
  leave it in here for portability and to future proof our code
  */
while (Wire.available())
{
  buffer[i] = Wire.read();
  i++;
}
///////////////////////
/* End Read Sequence */
///////////////////////
/*
Using some bitwise manipulation we will shift our buffer
into an integer for general consumption
*/
co2_value = 0;
co2_value |= buffer[1] & 0xFF;
co2_value = co2_value << 8;
co2_value |= buffer[2] & 0xFF;
byte sum = 0; //Checksum Byte
sum = buffer[0] + buffer[1] + buffer[2]; //Byte addition utilizes overflow
if (sum == buffer[3])
{
  // Success!
  digitalWrite(13, LOW);
  return co2_value;
}
  else
{
  // Failure!
  /*
  Checksum failure can be due to a number of factors,
  fuzzy electrons, sensor busy, etc.
  */
  digitalWrite(13, LOW);
  return 0;
  }
}

void loop() {
  
  String dataString = "";
  
  int co2Value = readCO2();

  if (co2Value > 0)
  {
    DateTime now = rtc.now();
  
    dataString += String(now.unixtime());
    dataString += ",";
    dataString += String(co2Value);
  
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }
      else {
        Serial.println("error opening datalog.txt");
      }
  }
  
  else
  {
    Serial.println("Checksum failed / Communication failure");
  }
   
  delay(2000);
}
