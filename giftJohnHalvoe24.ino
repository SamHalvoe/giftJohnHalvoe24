#include "gJH24_controller.h"

void setup()
{
  Serial.begin(9600);
  elapsedMillis timeSinceSerialbegin;
  while (not Serial && timeSinceSerialbegin < 5000) { delay(10); }

  Serial.println("setup started");
  
  if (not oled.begin())
  {
    Serial.println("oled.begin() failed!");
  }

  oled.setContrast(oledBrightness);

  if (not Wire.begin())
  {
    Serial.println("Wire.begin() failed!");
  }

  if (not beginEeprom())
  {
    Serial.println("beginEeprom() failed!");
  }

  if (not touchInput.begin())
  {
    Serial.println("touchInput.begin() failed!");
  }
  
  Serial.println("setup finished");
}

void loop()
{
  handleApp(currentAppMode);
  updateOled(currentAppMode, getCurrentModeString(currentAppMode), getCurrentModeInteger(currentAppMode), currentCredentialsListPtr);
}
