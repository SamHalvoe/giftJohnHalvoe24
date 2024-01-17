#pragma once

#include "gJH24_def.h"
#include "gJH24_qrCode.h"
#include "gJH24_eeprom.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"
#include "gJH24_oled.h"

#include "SimpleButton.h"

const uint8_t PIN_BUTTON_A = 12;
const uint8_t PIN_BUTTON_B = 27;

SimpleButton buttonA(PIN_BUTTON_A);
SimpleButton buttonB(PIN_BUTTON_B);

WiFiCredentials currentCredentials;
uint8_t connectionAttemptCount = 0;
elapsedMillis timeSinceWiFiConnectionCheck;
CredentialsListPtr currentCredentialsListPtr;

const uint16_t TIME_UPDATE_INTERVAL = 300;
elapsedMillis timeSinceTimeUpdate = TIME_UPDATE_INTERVAL;

String bitcoinString;

CredentialsListPtr mockCredentialsListPtr()
{
  CredentialsListPtr credentialsListPtr = std::make_shared<CredentialsList>();

  for (uint32_t index = 0; index < 10; ++index)
  {
    credentialsListPtr->emplace_back(true, String("ssid_") + index, String("password_") + index);
  }

  return credentialsListPtr;
}

void handleConfig()
{
  if (buttonA.isJustReleased())
  {
    turnCodeReaderLedOn();

    currentAppMode = AppMode::readWiFiQRCode;
  }
  else if (buttonB.isJustReleased())
  {
    currentCredentialsListPtr = mockCredentialsListPtr();
    //currentCredentialsListPtr = readCredentialsList();

    currentAppMode = AppMode::loadWiFiCredentials;
  }
}

void handleReadWiFiQRCode()
{
  if (buttonB.isJustReleased())
  {
    currentAppMode = AppMode::config;
  }

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

void handleLoadWiFiCredentials()
{
  if (currentCredentialsListPtr && currentCredentialsListPtr->size() > 0)
  {
    if (buttonA.isJustReleased())
    {
      decrementCredentialsSelectionIndex();
    }
    else if (buttonB.isJustReleased())
    {
      incrementCredentialsSelectionIndex(currentCredentialsListPtr->size());
    }
    else if (buttonA.isPressed() && buttonB.isPressed())
    {
      connectToWifi(currentCredentialsListPtr->at(credentialsSelectionIndex));
      connectionAttemptCount = 0;
      currentAppMode = AppMode::connectToWiFi;
    }
  }
  else if (buttonA.isJustReleased())
  {
    currentAppMode = AppMode::config;
  }
}

void handleConnectToWiFi()
{
  if (timeSinceWiFiConnectionCheck >= 500)
  {
    if (isConnectedToWifi())
    {
      saveCredentials(currentCredentials);
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
  if (buttonA.isJustReleased() || buttonB.isJustReleased())
  {
    currentAppMode = AppMode::config;
  }
}

void handleClock()
{
  if (timeSinceTimeUpdate >= TIME_UPDATE_INTERVAL)
  {
    updateLocalTimeString();
    
    timeSinceTimeUpdate = 0;
  }
}

const String& getCurrentModeString(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::clock:
      return timeString;
    break;

    case AppMode::bitcoin:
      return bitcoinString;
    break;
  }

  return String();
}

void handleApp(AppMode in_appMode)
{
  buttonA.update();
  buttonB.update();

  switch (in_appMode)
  {
    case AppMode::config:
      handleConfig();
    break;

    case AppMode::readWiFiQRCode:
      handleReadWiFiQRCode();
    break;

    case AppMode::loadWiFiCredentials:
      handleLoadWiFiCredentials();
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

    case AppMode::bitcoin:
      
    break;
  }
}
