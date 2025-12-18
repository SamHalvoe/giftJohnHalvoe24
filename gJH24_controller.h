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

const uint16_t QRCODE_READING_INTERVAL = 500; // ms
elapsedMillis timeSinceQRCodeReading;

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

void switchAppMode(AppMode in_newMode)
{
  currentAppMode = in_newMode;
  touchInput.reset();
}

void handleConfig()
{
  if (touchInput.isRightTapped())
  {
    switchAppMode(AppMode::readWiFiQRCode);
  }
  else if (touchInput.isLeftTapped())
  {
    currentCredentialsListPtr = readCredentialsList(); //mockCredentialsListPtr();

    switchAppMode(AppMode::loadWiFiCredentials);
  }
}

void handleReadWiFiQRCode()
{
  if (touchInput.isLeftTapped())
  {
    switchAppMode(AppMode::config);
  }

  if (timeSinceQRCodeReading >= QRCODE_READING_INTERVAL)
  {
    String qrCodeContent = readQRCode();

    if (qrCodeContent.length() > 0)
    {
      currentCredentials = extractWiFiCredentials(qrCodeContent);

      if (currentCredentials.m_isComplete)
      {
        connectToWifi(currentCredentials);
        connectionAttemptCount = 0;
        switchAppMode(AppMode::connectToWiFi);
      }
    }

    timeSinceQRCodeReading = 0;
  }
}

void handleLoadWiFiCredentials()
{
  if (currentCredentialsListPtr && currentCredentialsListPtr->size() > 0)
  {
    if (touchInput.isRightTapped())
    {
      decrementCredentialsSelectionIndex();
    }
    else if (touchInput.isLeftTapped())
    {
      incrementCredentialsSelectionIndex(currentCredentialsListPtr->size());
    }
    else if (false) //buttonA.isPressed() && buttonB.isPressed()
    {
      connectToWifi(currentCredentialsListPtr->at(credentialsSelectionIndex));
      connectionAttemptCount = 0;
      switchAppMode(AppMode::connectToWiFi);
    }
    else if (false) //buttonA.wasPressedFor(2000)
    {
      switchAppMode(AppMode::config);
    }
  }
  else if (touchInput.isRightTapped())
  {
    switchAppMode(AppMode::config);
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
      switchAppMode(AppMode::clock);
    }
    else
    {
      ++connectionAttemptCount;
    }

    if (connectionAttemptCount == 24)
    {
      disconnectFromWifi();

      switchAppMode(AppMode::connectToWiFiFailed);
    }

    timeSinceWiFiConnectionCheck = 0;
  }
}

void handleConnectToWiFiFailed()
{
  if (touchInput.isLeftTapped() || touchInput.isRightTapped()) //buttonA.wasReleased() || buttonB.wasReleased()
  {
    switchAppMode(AppMode::config);
  }
}

void handleClock()
{
  if (touchInput.isLeftTapped())
  {
    switchAppMode(AppMode::bitcoin);
  }
}

void handleBitcoin()
{
  if (touchInput.isRightTapped())
  {
    switchAppMode(AppMode::clock);
  }

  if (false) //buttonA.wasPressedFor(2000) && buttonB.wasReleased()
  {
    updateBitcoinPrice();
  }
}

String getCurrentModeString(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:  return String(getBatteryVoltage()) + " V";
    case AppMode::clock:   return timeString;
    case AppMode::bitcoin: return bitcoinPrice;
  }

  return String();
}

int32_t getCurrentModeInteger(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::connectToWiFi: return connectionAttemptCount;
    case AppMode::clock:         return displayIndicator;
    case AppMode::bitcoin:       return displayIndicator;
  }

  return -1;
}

void handleBrightnessAdjustment()
{
  if (isOledOn && not isOledAutoOnOff && false) //buttonA.wasPressedFor(2000) && buttonB.wasPressedFor(2000)
  {
    touchInput.reset();
    isBrightnessAdjustmentActive = not isBrightnessAdjustmentActive;
  }

  if (isBrightnessAdjustmentActive)
  {
    if (touchInput.isRightTapped()) //buttonA.wasReleased()
    {
      decrementBrightness();
    }
    else if (touchInput.isLeftTapped()) //buttonB.wasReleased()
    {
      incrementBrightness();
    }
  }
}

void handleApp(AppMode in_appMode)
{
  touchInput.update();

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
