#pragma once

#include "gJH24_def.h"
#include "gJH24_qrCode.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"

#include "SimpleButton.h"

const uint8_t PIN_BUTTON_A = 12;
const uint8_t PIN_BUTTON_B = 27;

SimpleButton buttonA(PIN_BUTTON_A);
SimpleButton buttonB(PIN_BUTTON_B);

WiFiCredentials currentCredentials;
uint8_t connectionAttemptCount = 0;
elapsedMillis timeSinceWiFiConnectionCheck;
String timeString;

void handleConfig()
{
  if (buttonA.isPressed())
  {
    turnCodeReaderLedOn();

    currentAppMode = AppMode::readWiFiQRCode;
  }
  else if (buttonB.isPressed())
  {
    currentAppMode = AppMode::loadWiFiData;
  }
}

void handleReadWiFiQRCode()
{
  String qrCodeContent = readQRCode();

  if (qrCodeContent.length() == 0)
  {
    return;
  }

  currentCredentials = extractWiFiCredentials(qrCodeContent);

  if (currentCredentials.m_isComplete)
  {
    turnCodeReaderLedOff();
    connectToWifi(currentCredentials);
    connectionAttemptCount = 0;
    currentAppMode = AppMode::connectToWiFi;
  }
}

void handleConnectToWiFi()
{
  if (timeSinceWiFiConnectionCheck >= 500)
  {
    if (isConnectedToWifi())
    {
      //saveWiFiCredentials(credentials);
      configurateTime();
      currentAppMode = AppMode::clock;
    }
    else
    {
      ++connectionAttemptCount;
    }

    if (connectionAttemptCount == 16)
    {
      currentAppMode = AppMode::connectToWiFiFailed;
    }

    timeSinceWiFiConnectionCheck = 0;
  }
}

void handleConnectToWiFiFailed()
{
  if (buttonA.isPressed() || buttonB.isPressed())
  {
    currentAppMode = AppMode::config;
  }
}

void handleClock()
{
  timeString = getLocalTimeString();
}

const String& getCurrentModeString(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::clock:
      return timeString;
    break;

    case AppMode::bitcoin:
      return "1,000,000";
    break;
  }

  return String();
}

void handleApp(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:
      handleConfig();
    break;

    case AppMode::readWiFiQRCode:
      handleReadWiFiQRCode();
    break;

    case AppMode::loadWiFiData:
      
    break;

    case AppMode::connectToWiFi:
      handleConnectToWiFi();
    break;

    case AppMode::connectToWiFiFailed:
      handleConnectToWiFiFailed();
    break;
    
    case AppMode::clock:
      handleClock();
    break;
  }
}
