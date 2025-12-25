#include "gJH24_controller.h"

void setup()
{
  Serial.begin(115200);
  elapsedMillis timeSinceSerialbegin;
  while (not Serial && timeSinceSerialbegin < 3000) delay(100);

  Serial.println("setup started");
  
  if (not oled.begin())
  {
    Serial.println("oled.begin() failed!");
  }

  oled.setContrast(brightnessLevel[brightnessLevelIndex]);

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
  updateOled(currentAppMode, getCurrentModeString(currentAppMode), getCurrentModeString2(currentAppMode), getCurrentModeInteger(currentAppMode), currentCredentialsListPtr);
}
