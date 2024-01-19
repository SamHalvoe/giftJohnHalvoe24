#include "gJH24_controller.h"

void setup()
{
  Serial.begin(9600);
  elapsedMillis timeSinceSerialbegin;
  while (not Serial && timeSinceSerialbegin < 5000) {}

  Serial.println("setup started");
  
  oled.begin();
  oled.setContrast(oledBrightness);
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
  updateOled(currentAppMode, getCurrentModeString(currentAppMode), getCurrentModeInteger(currentAppMode), currentCredentialsListPtr);
}
