#pragma once

#include "gJH24_def.h"
#include "gJH24_power.h"
#include "gJH24_qrCode.h"
#include "gJH24_eeprom.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"
#include "gJH24_widget.h"
#include "gJH24_oled.h"
#include "gJH24_touchInput.h"

WiFiCredentials currentCredentials;
uint8_t connectionAttemptCount = 0;
elapsedMillis timeSinceWiFiConnectionCheck;
CredentialsListPtr currentCredentialsListPtr;

const uint16_t QRCODE_READING_INTERVAL = 500; // ms
elapsedMillis timeSinceQRCodeReading;

const uint16_t TIME_UPDATE_INTERVAL = 500; // ms
elapsedMillis timeSinceTimeUpdate = TIME_UPDATE_INTERVAL; // "= TIME_UPDATE_INTERVAL" >> update time after startup

const uint16_t MIN_PRICE_UPDATE_DELAY = 10; // s
const uint16_t PRICE_UPDATE_INTERVAL = 600; // s | 10 minutes
elapsedSeconds timeSincePriceUpdate = PRICE_UPDATE_INTERVAL;

const uint16_t MIN_BLOCK_HEIGHT_UPDATE_DELAY = 10; // s
const uint16_t BLOCK_HEIGHT_UPDATE_INTERVAL = 300; // s | 5 minutes
elapsedSeconds timeSinceBlockHeightUpdate = BLOCK_HEIGHT_UPDATE_INTERVAL;

const uint16_t LONG_PRESS_DURATION = 1000; // ms | 1 second

bool shouldSaveWiFiCredentials = false;

const uint16_t CONFIG_SAVE_INTERVAL = 1200; // s | 20 minutes
elapsedSeconds timeSinceConfigSave = CONFIG_SAVE_INTERVAL - 300; // "= CONFIG_SAVE_INTERVAL - 300" >> save config 5 minutes after wifi connection is made

uint16_t widgetUpdateInterval = 15; // s
elapsedSeconds timeSinceWidgetUpdate = widgetUpdateInterval;

size_t widgetIndex = 0;
String currentWidgetId;

/*CredentialsListPtr mockCredentialsListPtr()
{
  CredentialsListPtr credentialsListPtr = std::make_shared<CredentialsList>();

  for (uint32_t index = 0; index < 10; ++index)
  {
    credentialsListPtr->emplace_back(true, String("ssid_") + index, String("password_") + index);
  }

  return credentialsListPtr;
}*/

void doUpdateBitcoinPrice()
{
  updateBitcoinPrice();
  displayIndicatorBitcoin = displayIndicatorBitcoinUpdate_true;
  timeSincePriceUpdate = 0;
}

void doUpdateBlockHeight()
{
  updateBlockHeight();
  displayIndicatorBlockHeight = displayIndicatorBitcoinUpdate_true;
  timeSinceBlockHeightUpdate = 0;
}

void switchAppMode(AppMode in_newMode)
{
  currentAppMode = in_newMode;
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
    
    if (indexOfLastSelectedCredentials != INVALID_INDEX && indexOfLastSelectedCredentials < currentCredentialsListPtr->size())
    {
      switchAppMode(AppMode::loadLastWiFiCredentials);
    }
    else
    {
      switchAppMode(AppMode::loadWiFiCredentials);
    }
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
        shouldSaveWiFiCredentials = true;
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
    else if (touchInput.isMiddlePressedFor(LONG_PRESS_DURATION))
    {
      shouldSaveWiFiCredentials = false;
      connectToWifi(currentCredentialsListPtr->at(credentialsSelectionIndex));
      connectionAttemptCount = 0;
      switchAppMode(AppMode::connectToWiFi);
    }
    else if (touchInput.isRightPressedFor(LONG_PRESS_DURATION))
    {
      switchAppMode(AppMode::config);
    }
  }
  else if (touchInput.isRightTapped())
  {
    switchAppMode(AppMode::config);
  }
}

void handleLoadLastWiFiCredentials()
{
  if (touchInput.isRightPressedFor(LONG_PRESS_DURATION))
  {
    switchAppMode(AppMode::loadWiFiCredentials);
  }
  else if (touchInput.isMiddlePressedFor(LONG_PRESS_DURATION))
  {
    shouldSaveWiFiCredentials = false;
    credentialsSelectionIndex = indexOfLastSelectedCredentials;
    connectToWifi(currentCredentialsListPtr->at(indexOfLastSelectedCredentials));
    connectionAttemptCount = 0;
    switchAppMode(AppMode::connectToWiFi);
  }
}

void handleErrorWiFiCredentials()
{
  if (touchInput.isMiddleTapped())
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
      if (shouldSaveWiFiCredentials)
      {
        if (saveCredentials(currentCredentials))
        {
          // set indexOfLastSelectedCredentials to index of added credentials
          indexOfLastSelectedCredentials = currentCredentialsListPtr->size();
        }
        else
        {
          switchAppMode(AppMode::errorWiFiCredentials);
          return;
        }
      }
      else
      {
        indexOfLastSelectedCredentials = credentialsSelectionIndex;
      }

      configurateTime();
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
  if (touchInput.isMiddleTapped())
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

  if (touchInput.isRightPressedFor(LONG_PRESS_DURATION) && timeSincePriceUpdate > MIN_PRICE_UPDATE_DELAY)
  {
    currentCurrency = (currentCurrency == Currency::euro ? Currency::usDollar : Currency::euro);
    doUpdateBitcoinPrice();
  }

  if (touchInput.isLeftTapped())
  {
    switchAppMode(AppMode::blockHeight);
  }

  if (touchInput.isLeftPressedFor(LONG_PRESS_DURATION) && timeSincePriceUpdate >= MIN_PRICE_UPDATE_DELAY)
  {
    doUpdateBitcoinPrice();
  }
}

void handleBlockHeight()
{
  if (touchInput.isRightTapped())
  {
    switchAppMode(AppMode::bitcoin);
  }

  if (touchInput.isLeftPressedFor(LONG_PRESS_DURATION) && timeSinceBlockHeightUpdate >= MIN_BLOCK_HEIGHT_UPDATE_DELAY)
  {
    doUpdateBlockHeight();
  }

  if (touchInput.isLeftTapped())
  {
    getWidgetIds();

    if (widgetIds.size() > 0)
    {
      currentWidgetId = widgetIds[0];
      getWidgetScreen(widgetIds[widgetIndex]);
      switchAppMode(AppMode::widget);
    }
  }
}

String getCurrentModeString(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:      return String(getBatteryVoltage()) + " V";
    case AppMode::clock:       return timeString;
    case AppMode::bitcoin:     return bitcoinPrice;
    case AppMode::blockHeight: return blockHeight;
  }

  return String();
}

String getCurrentModeString2(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::bitcoin:     return bitcoinPriceTimestamp;
    case AppMode::blockHeight: return blockHeightTimestamp;
  }

  return String();
}

int32_t getCurrentModeInteger(AppMode in_appMode)
{
  switch (in_appMode)
  {
    case AppMode::config:               return (getBatteryVoltage() <= LOW_BATTERY_VOLTAGE ? 0 : 1);
    case AppMode::errorWiFiCredentials: return isMaxCredentialCountReached;
    case AppMode::connectToWiFi:        return connectionAttemptCount;
    case AppMode::clock:                return displayIndicatorClock;
    case AppMode::bitcoin:              return displayIndicatorBitcoin;
    case AppMode::blockHeight:          return displayIndicatorBlockHeight;
  }

  return -1;
}

void handleFontSelection()
{
  if (touchInput.isRightPressedFor(LONG_PRESS_DURATION))
  {
    isFontSelectionActive = not isFontSelectionActive;
  }

  if (isFontSelectionActive)
  {
    if (touchInput.isRightTapped())
    {
      decrementFontIndex();
    }
    else if (touchInput.isLeftTapped())
    {
      incrementFontIndex();
    }
  }
}

void handleBrightnessAdjustment()
{
  if (isOledOn && not isOledAutoOnOff && touchInput.isMiddlePressedFor(LONG_PRESS_DURATION))
  {
    isBrightnessAdjustmentActive = not isBrightnessAdjustmentActive;
  }

  if (isBrightnessAdjustmentActive)
  {
    if (touchInput.isRightTapped())
    {
      decrementBrightness();
    }
    else if (touchInput.isLeftTapped())
    {
      incrementBrightness();
    }
  }
}

void matchWidgetIndexToId()
{
  widgetIndex = 0;

  for (size_t index = 0; index < widgetIds.size(); ++index)
  {
    if (widgetIds[index] == currentWidgetId)
    {
      widgetIndex = index;
      break;
    }
  }
}

void gotoNextWidget()
{
  if (widgetIndex < widgetIds.size() - 1)
  {
    ++widgetIndex;
    currentWidgetId = widgetIds[widgetIndex];
  }
}

void gotoPreviousWidget()
{
  if (widgetIndex > 0)
  {
    --widgetIndex;
    currentWidgetId = widgetIds[widgetIndex];
  }
}

void handleWidget()
{
  if (touchInput.isRightTapped())
  {
    if (widgetIndex == 0)
    {
      switchAppMode(AppMode::blockHeight);
    }
    else
    {
      getWidgetIds();
      matchWidgetIndexToId();
      gotoPreviousWidget();
      getWidgetScreen(widgetIds[widgetIndex]);
    }
  }
  
  // ToDo: Implement updates based on  header "update-interval"!
  /*if (timeSinceWidgetUpdate >= widgetUpdateInterval)
  {
    getWidgetScreen(widgetIds[widgetIndex]);
    timeSinceWidgetUpdate = 0;
  }*/

  if (touchInput.isLeftTapped())
  {
    getWidgetIds();
    matchWidgetIndexToId();
    gotoNextWidget();
    getWidgetScreen(widgetIds[widgetIndex]);
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

    case AppMode::loadLastWiFiCredentials:
      handleLoadLastWiFiCredentials();
      break;

    case AppMode::errorWiFiCredentials:
      handleErrorWiFiCredentials();
      break;

    case AppMode::connectToWiFi:
      handleConnectToWiFi();
      break;

    case AppMode::connectToWiFiFailed:
      handleConnectToWiFiFailed();
      break;
    
    case AppMode::clock:
      if (not isFontSelectionActive)
      {
        handleBrightnessAdjustment();
      }
      
      if (not isBrightnessAdjustmentActive)
      {
        handleFontSelection();
      }
      
      if (not isBrightnessAdjustmentActive && not isFontSelectionActive)
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

    case AppMode::blockHeight:
      handleBrightnessAdjustment();

      if (not isBrightnessAdjustmentActive)
      {
        handleBlockHeight();
      }
      break;

    case AppMode::widget:
      handleWidget();
      break;
  }

  if (timeSinceTimeUpdate >= TIME_UPDATE_INTERVAL)
  {
    updateTime();
    getLocalTimeString();
    timeSinceTimeUpdate = 0;
  }

  if (timeSincePriceUpdate >= PRICE_UPDATE_INTERVAL)
  {
    doUpdateBitcoinPrice();

    if (currentAppMode == AppMode::bitcoin && isOledAutoOnOff)
    {
      turnOledOn();
    }
  }
  else if (currentAppMode == AppMode::bitcoin && timeSincePriceUpdate > 10)
  {
    if (isOledAutoOnOff)
    {
      turnOledOff();
    }

    displayIndicatorBitcoin = displayIndicator_false;
  }

  if (timeSinceBlockHeightUpdate >= BLOCK_HEIGHT_UPDATE_INTERVAL)
  {
    doUpdateBlockHeight();

    if (currentAppMode == AppMode::blockHeight && isOledAutoOnOff)
    {
      turnOledOn();
    }
  }
  else if (currentAppMode == AppMode::blockHeight && timeSinceBlockHeightUpdate > 10)
  {
    if (isOledAutoOnOff)
    {
      turnOledOff();
    }

    displayIndicatorBlockHeight = displayIndicator_false;
  }

  if (getBatteryVoltage() <= LOW_BATTERY_VOLTAGE)
  {
    if (not isOledOn)
    {
      turnOledOn();
    }

    displayIndicatorClock = displayIndicatorBatteryLow_true;
    displayIndicatorBitcoin = displayIndicatorBatteryLow_true;
    displayIndicatorBlockHeight = displayIndicatorBatteryLow_true;
  }
  else 
  {
    if (displayIndicatorClock == displayIndicatorBatteryLow_true)
    {
      displayIndicatorClock = displayIndicator_false;
    }

    if (displayIndicatorBitcoin == displayIndicatorBatteryLow_true)
    {
      displayIndicatorBitcoin = displayIndicator_false;
    }

    if (displayIndicatorBlockHeight == displayIndicatorBatteryLow_true)
    {
      displayIndicatorBlockHeight = displayIndicator_false;
    }
  }

  if (currentAppMode >= AppMode::clock)
  {
    if (timeSinceConfigSave >= CONFIG_SAVE_INTERVAL)
    {
      if (saveConfig())
      {
        Serial.println("Config saved.");
      }
      else
      {
        Serial.println("Error: saveConfig() failed!");
      }

      timeSinceConfigSave = 0;
    }
  }
  else
  {
    timeSinceConfigSave = CONFIG_SAVE_INTERVAL - 300; // "= CONFIG_SAVE_INTERVAL - 300" >> save config 5 minutes after wifi connection is made
  }
}
