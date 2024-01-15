#pragma once

#include "gJH24_def.h"

#include "SimpleButton.h"

const uint8_t PIN_BUTTON_A = 12;
const uint8_t PIN_BUTTON_B = 27;

SimpleButton buttonA(PIN_BUTTON_A);
SimpleButton buttonB(PIN_BUTTON_B);

void handleConfig()
{
  if (buttonA.isPressed())
  {
    currentAppMode = AppMode::readWiFiQRCode;
  }
  else if (buttonB.isPressed())
  {
    currentAppMode = AppMode::loadWiFiData;
  }
}

void handleApp(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:
      handleConfig();
    break;

    case AppMode::readWiFiQRCode:
      
    break;

    case AppMode::loadWiFiData:
      
    break;
  }
}