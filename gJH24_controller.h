#pragma once

#include "gJH24_def.h"
#include "gJH24_power.h"
#include "gJH24_qrCode.h"
#include "gJH24_eeprom.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"
#include "gJH24_oled.h"
#include "gJH24_touchInput.h"

WiFiCredentials currentCredentials;
uint8_t connectionAttemptCount = 0;
elapsedMillis timeSinceWiFiConnectionCheck;
CredentialsListPtr currentCredentialsListPtr;

const uint16_t TIME_UPDATE_INTERVAL = 300; // ms
elapsedMillis timeSinceTimeUpdate = TIME_UPDATE_INTERVAL;

const uint16_t BITCOIN_UPDATE_INTERVAL = 600; // s
elapsedSeconds timeSinceBitcoinUpdate;

const uint16_t MIN_BRIGHTNESS_ADJUSTMENT_INTERVAL = 200; // ms
elapsedMillis timeSinceBrightnessAdjustment;

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
  if (touchInput.m_touchSensor.isLeftTouched()) //buttonA.wasReleased()
  {
    currentAppMode = AppMode::readWiFiQRCode;
  }
  else if (touchInput.m_touchSensor.isRightTouched()) //buttonB.wasReleased()
  {
    currentCredentialsListPtr = readCredentialsList(); //mockCredentialsListPtr();

    currentAppMode = AppMode::loadWiFiCredentials;
  }
}

void handleReadWiFiQRCode()
{
  if (false) //buttonB.wasReleased()
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
    connectToWifi(currentCredentials);
    connectionAttemptCount = 0;
    currentAppMode = AppMode::connectToWiFi;
  }
}

void handleLoadWiFiCredentials()
{
  if (currentCredentialsListPtr && currentCredentialsListPtr->size() > 0)
  {
    if (false) //buttonA.wasReleased()
    {
      decrementCredentialsSelectionIndex();
    }
    else if (false) //buttonB.wasReleased()
    {
      incrementCredentialsSelectionIndex(currentCredentialsListPtr->size());
    }
    else if (false) //buttonA.isPressed() && buttonB.isPressed()
    {
      connectToWifi(currentCredentialsListPtr->at(credentialsSelectionIndex));
      connectionAttemptCount = 0;
      currentAppMode = AppMode::connectToWiFi;
    }
    else if (false) //buttonA.wasPressedFor(2000)
    {
      //buttonA.disableNext();
      
      currentAppMode = AppMode::config;
    }
  }
  else if (false) //buttonA.wasReleased()
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
      updateBitcoinPrice();
      currentAppMode = AppMode::clock;
    }
    else
    {
      ++connectionAttemptCount;
    }

    if (connectionAttemptCount == 24)
    {
      disconnectFromWifi();

      currentAppMode = AppMode::connectToWiFiFailed;
    }

    timeSinceWiFiConnectionCheck = 0;
  }
}

void handleConnectToWiFiFailed()
{
  if (false) //buttonA.wasReleased() || buttonB.wasReleased()
  {
    currentAppMode = AppMode::config;
  }
}

void handleClock()
{
  if (false) //buttonB.wasReleased()
  {
    currentAppMode = AppMode::bitcoin;
  }
}

void handleBitcoin()
{
  if (false) //buttonA.wasReleased()
  {
    currentAppMode = AppMode::clock;
  }

  if (false) //buttonA.wasPressedFor(2000) && buttonB.wasReleased()
  {
    //buttonA.disableNext();
    updateBitcoinPrice();
  }
}

String getCurrentModeString(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:
      return String(getBatteryVoltage()) + " V";
    break;

    case AppMode::clock:
      return timeString;
    break;

    case AppMode::bitcoin:
      return bitcoinPrice;
    break;
  }

  return String();
}

int32_t getCurrentModeInteger(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::connectToWiFi:
      return connectionAttemptCount;
    break;

    case AppMode::clock:
      return displayIndicator;
    break;

    case AppMode::bitcoin:
      return displayIndicator;
    break;
  }

  return -1;
}

void handleBrightnessAdjustment()
{
  if (isOledOn && not isOledAutoOnOff && false) //buttonA.wasPressedFor(2000) && buttonB.wasPressedFor(2000)
  {
    //buttonA.disableNext();
    //buttonB.disableNext();
    isBrightnessAdjustmentActive = not isBrightnessAdjustmentActive;
  }

  if (isBrightnessAdjustmentActive)
  {
    if (true) //buttonA.wasReleased()
    {
      decrementBrightness();
    }
    else if (false) //buttonB.wasReleased()
    {
      incrementBrightness();
    }
  }
}

void handleApp(AppMode in_appMode)
{
  //buttonA.update();
  //buttonB.update();

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
      handleBrightnessAdjustment();

      if (not isBrightnessAdjustmentActive)
      {
        handleClock();
      }
    break;

    case AppMode::bitcoin:
      handleBrightnessAdjustment();

      if (not isBrightnessAdjustmentActive)
      {
        handleBitcoin();
      }
    break;
  }

  if (timeSinceTimeUpdate >= TIME_UPDATE_INTERVAL)
  {
    updateTime();
    getLocalTimeString();
    
    timeSinceTimeUpdate = 0;
  }

  if (timeSinceBitcoinUpdate >= BITCOIN_UPDATE_INTERVAL)
  {
    updateBitcoinPrice();

    if (isOledAutoOnOff)
    {
      turnOledOn();
    }
    else
    {
      displayIndicator = displayIndicatorBitcoinUpdate_true;
    }

    timeSinceBitcoinUpdate = 0;
  }
  else if (timeSinceBitcoinUpdate > 10)
  {
    if (isOledAutoOnOff)
    {
      turnOledOff();
    }

    displayIndicator = displayIndicator_false;
  }

  if (getBatteryVoltage() < 3.70F)
  {
    displayIndicator = displayIndicatorBatteryLow_true;
  }
  else
  {
    displayIndicator = displayIndicator_false;
  }
}
