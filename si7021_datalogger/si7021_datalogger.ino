#include "SPI.h"
#include "SD.h"
#include "Adafruit_Si7021.h"
#include "RTClib.h"

Adafruit_Si7021 sensor = Adafruit_Si7021();


const int chipSelect = 10;  // used for Arduino


RTC_PCF8523 rtc;


void setup() {
  Serial.begin(115200);
  
  while (!Serial) {
    delay(100);
  }

   if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    while (1);
  }

  Serial.print("Found model ");
  switch(sensor.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples"); break;
    case SI_7013:
      Serial.print("Si7013"); break;
    case SI_7020:
      Serial.print("Si7020"); break;
    case SI_7021:
      Serial.print("Si7021"); break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  }
  Serial.print(" Rev(");
  Serial.print(sensor.getRevision());
  Serial.print(")");
  Serial.print(" Serial #"); Serial.print(sensor.sernum_a, HEX); Serial.println(sensor.sernum_b, HEX);
}

void loop() {
  
  String dataString = "";

  float h = sensor.readHumidity();
  float t = sensor.readTemperature();

  DateTime now = rtc.now();

  dataString += String(now.unixtime());
  dataString += ",";
  dataString += String(h);
  dataString += ",";
  dataString += String(t);
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  
  else {
    Serial.println("error opening datalog.txt");
  }

  delay(20000);
}
