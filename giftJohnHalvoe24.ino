#include "gJH24_controller.h"
#include "gJH24_oled.h"

void setup()
{
  Serial.begin(9600);

  while (not Serial) {}

  Serial.println("setup started");
  
  oled.begin();
  Wire.begin();
  buttonA.begin();
  buttonB.begin();
  beginEeprom();

  turnCodeReaderLedOff();
  
  Serial.println("setup finished");
}

void loop()
{
  handleApp(currentAppMode);
  updateOled(currentAppMode, getCurrentModeString(currentAppMode), connectionAttemptCount, credentialsListPtr);

  //Serial.print("currentAppMode: ");
  //Serial.println(static_cast<int16_t>(currentAppMode));
}
