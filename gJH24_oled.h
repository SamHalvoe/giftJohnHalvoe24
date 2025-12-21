/*

for fonts
- u8g2_font_streamline_interface_essential_other_t
- u8g2_font_streamline_phone_t
see https://streamlinehq.com

*/

#pragma once

#include <vector>

#include <elapsedMillis.h>
#include <U8g2lib.h>

#include "gJH24_def.h"
#include "gJH24_utility.h"

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;

const uint8_t PIN_CS = SS;
const uint8_t PIN_MOSI = MOSI;
const uint8_t PIN_MISO_DC = MISO;
const uint8_t PIN_SCK = SCK;
const uint8_t PIN_RESET = 26;

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI oled(U8G2_R0, PIN_SCK, PIN_MOSI, PIN_CS, PIN_MISO_DC, PIN_RESET);
uint8_t oledBrightness = 255;
bool isOledMirrored = false;
const uint16_t OLED_UPDATE_INTERVAL = 25; // ms
elapsedMillis timeSinceOledUpdate = OLED_UPDATE_INTERVAL;

constexpr const uint16_t iconQRCode = 48 + 6;
constexpr const uint16_t iconFloppy = 48 + 3;
constexpr const uint16_t iconBatteryLow = 48 + 1;
constexpr const uint16_t iconBlockHeight = 48 + 15;
constexpr const uint16_t yOffsetIcon = 32;

size_t credentialsSelectionIndex = 0;
size_t credentialsPageIndex = 0;
const size_t credentialsPageSize = 3;

const int32_t displayIndicator_false = 0;
const int32_t displayIndicatorBitcoinUpdate_true = 1;
const int32_t displayIndicatorBatteryLow_true = 2;
int32_t displayIndicator = displayIndicator_false;

const uint8_t minBrightness = 0;
const uint8_t maxBrightness = 255;
const uint8_t brightnessStep = 64;

bool isBrightnessAdjustmentActive = false;
bool isOledOn = true;
bool isOledAutoOnOff = false;

void turnOledOn()
{
  oled.setPowerSave(0);
}

void turnOledOff()
{
  oled.setPowerSave(1);
}

void incrementBrightness()
{
  if (not isOledOn || isOledAutoOnOff)
  {
    turnOledOn();
    isOledOn = true;
    isOledAutoOnOff = false;

    return;
  }

  int16_t newBrightness = oledBrightness + brightnessStep;

  if (newBrightness < maxBrightness)
  {
    oledBrightness = newBrightness;
  }
  else
  {
    oledBrightness = maxBrightness;
  }
  
  oled.setContrast(oledBrightness);
}

void decrementBrightness()
{
  if (oledBrightness == minBrightness)
  {
    turnOledOff();

    if (currentAppMode == AppMode::bitcoin)
    {
      isOledAutoOnOff = true;
    }
    else
    {
      isOledOn = false;
    }

    return;
  }

  int16_t newBrightness = oledBrightness - brightnessStep;

  if (newBrightness > minBrightness)
  {
    oledBrightness = newBrightness;
  }
  else
  {
    oledBrightness = minBrightness;
  }

  oled.setContrast(oledBrightness);
}

void incrementCredentialsSelectionIndex(size_t in_credentialListSize)
{
  if (credentialsSelectionIndex < in_credentialListSize - 1)
  {
    ++credentialsSelectionIndex;

    if (credentialsSelectionIndex % credentialsPageSize == 0)
    {
      ++credentialsPageIndex;
    }
  }
}

void decrementCredentialsSelectionIndex()
{
  if (credentialsSelectionIndex > 0)
  {
    --credentialsSelectionIndex;

    if ((credentialsSelectionIndex + 1) % credentialsPageSize == 0)
    {
      --credentialsPageIndex;
    }
  }
}

void updateBrightnessAdjustmentIndicator()
{
  if (isBrightnessAdjustmentActive)
  {
    if (oledBrightness == maxBrightness)
    {
      oled.drawBox(0, 0, 5, 5);
    }
    else if (oledBrightness == minBrightness)
    {
      oled.drawFrame(0, 0, 5, 5);
    }
    else
    {
      oled.drawTriangle(0, 0, 5, 0, 0, 5);
    }
  }
}

void toggleMirrorOled()
{
  oled.setDisplayRotation(isOledMirrored ? U8G2_R0 : U8G2_MIRROR);

  isOledMirrored = not isOledMirrored;
}

void drawStringXCenter(uint16_t in_y, const char* in_string)
{
  const uint16_t stringWidth = oled.getStrWidth(in_string);
  oled.drawStr((128 - stringWidth) / 2, in_y, in_string);
}

void updateScreenConfig(const String& in_batteryVoltage)
{
  constexpr const uint16_t xLeftCenter = (64 - 21) / 2;
  constexpr const uint16_t xRightCenter = (196 - 21) / 2;

  oled.setFont(u8g2_font_streamline_phone_t);
  oled.drawGlyph(xLeftCenter, yOffsetIcon, iconQRCode);
  oled.setFont(u8g2_font_streamline_interface_essential_other_t);
  oled.drawGlyph(xRightCenter, yOffsetIcon, iconFloppy);

  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(32, in_batteryVoltage.c_str());
  drawStringXCenter(48, "Select credentials source;");
  drawStringXCenter(60, "Tap left or right;");
}

void updateScreenReadWiFiQRCode()
{
  constexpr const uint16_t xCenter = (128 - 21) / 2;

  oled.setFont(u8g2_font_streamline_phone_t);
  oled.drawGlyph(xCenter, yOffsetIcon, iconQRCode);
  
  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(48, "Read WiFi QR Code;");
  drawStringXCenter(60, "Tap right to cancle;");
}

void drawStringSSID(size_t in_index, const String& in_ssid, bool in_isSSIDSelected)
{
  const u8g2_uint_t stringPositionY = in_index * 12;

  if (in_isSSIDSelected)
  {
    oled.drawBox(3, 3 + stringPositionY, oled.getStrWidth(in_ssid.c_str()) + 2, 11);
    oled.setDrawColor(0);
  }

  oled.drawStr(4, 13 + stringPositionY, in_ssid.c_str());
  
  if (in_isSSIDSelected)
  {
    oled.setDrawColor(1);
  }
}

void drawScreenLoadWiFiCredentials(const CredentialsList& in_credentialList)
{
  oled.setFont(u8g2_font_glasstown_nbp_tr);

  const size_t offset = credentialsPageIndex * credentialsPageSize;

  for (size_t index = 0; index < credentialsPageSize && offset + index < in_credentialList.size(); ++index)
  {
    drawStringSSID(index, in_credentialList[offset + index].m_ssid,
                   offset + index == credentialsSelectionIndex);
  }

  drawStringXCenter(48, "Hold middle to select;");
  drawStringXCenter(60, "Hold left to return;");
}

void drawScreenLoadWiFiCredentialsEmpty()
{
  constexpr const uint16_t xCenter = (128 - 21) / 2;

  oled.setFont(u8g2_font_streamline_interface_essential_other_t);
  oled.drawGlyph(xCenter, yOffsetIcon, iconFloppy);
  
  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(48, "No saved credentials;");
  drawStringXCenter(60, "Tap left to return;");
}

void updateScreenLoadWiFiCredentials(const CredentialsListPtr in_credentialListPtr)
{
  if (in_credentialListPtr)
  {
    drawScreenLoadWiFiCredentials(*in_credentialListPtr);
  }
  else
  {
    drawScreenLoadWiFiCredentialsEmpty();
  }
}

void updateScreenConnectToWiFi(uint8_t in_attemptCount)
{
  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(32, "Connect to WiFi;");

  String dots(".");

  for (uint8_t index = 0; index < in_attemptCount; ++index)
  {
    dots.concat(".");
  }

  drawStringXCenter(48, dots.c_str());
}

void updateScreenConnectToWiFiFailed()
{
  drawStringXCenter(32, "Connection to WiFi failed;");
  drawStringXCenter(48, "Tap middle to continue;");
}

void updateIndicatorBatteryLow(int32_t in_integer)
{
  if (in_integer == displayIndicatorBatteryLow_true)
  {
    oled.setFont(u8g2_font_battery19_tn);
    oled.drawGlyph(120, 64, iconBatteryLow);
  }
}

void updateScreenClock(const String& in_string, int32_t in_integer)
{
  updateIndicatorBatteryLow(in_integer);
  oled.setFont(u8g2_font_mystery_quest_32_tn);
  drawStringXCenter(40, in_string.c_str());
}

void drawBitcoinCharacter(u8g2_uint_t in_x, u8g2_uint_t in_y)
{
  oled.drawStr(in_x, in_y, "B");
  oled.drawBox(in_x + 3, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 6, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 3, in_y, 2, 2);
  oled.drawBox(in_x + 6, in_y, 2, 2);
}

void drawEuroCharacter(u8g2_uint_t in_x, u8g2_uint_t in_y)
{
  oled.drawGlyph(in_x, in_y, 8364 /* '€' */);
}

void drawUsDollarCharacter(u8g2_uint_t in_x, u8g2_uint_t in_y)
{
  oled.drawGlyph(in_x, in_y, 36 /* '$' */);
}

std::vector<String> splitNumberStringAtThousandsPlace(const String& in_string)
{
  String string = splitString(in_string, { '.' })[0]; // get pre-decimal place
  const size_t numberSegmentSize = 3;
  const size_t removeCount = string.length() % numberSegmentSize;
  std::vector<String> stringList;
  String tempString;

  if (removeCount > 0)
  {
    stringList.emplace_back(string.substring(0, removeCount));
    string.remove(0, removeCount);
  }

  while (string.length() > 0)
  {
    tempString.concat(string[0]);
    string.remove(0, 1);

    if (tempString.length() == 3)
    {
      stringList.emplace_back(tempString);
      tempString.remove(0, tempString.length());
    }
  }

  return stringList;
}

void drawNumberString(uint8_t in_xOffset, uint8_t in_yOffset,
                      uint8_t in_spaceWidth, u8g2_uint_t in_characterWidth, 
                      const std::vector<String>& in_numberStringList, Currency in_currencyToDraw)
{
  uint8_t currencyCharacterOffset = (in_currencyToDraw == Currency::none ? 2 : 1);
  size_t index = 0;

  if (in_numberStringList.size() >= 3)
  {
    oled.drawStr(in_xOffset, in_yOffset,
                 in_numberStringList[index].c_str());

    ++index;
  }

  if (in_numberStringList.size() >= 2)
  {
    oled.drawStr(in_xOffset + (currencyCharacterOffset * in_spaceWidth) + ((currencyCharacterOffset + (3 - in_numberStringList[index].length())) * in_characterWidth),
                 in_yOffset,
                 in_numberStringList[index].c_str());

    ++index;
  }

  if (in_numberStringList.size() >= 1)
  {
    oled.drawStr(in_xOffset + ((1 + currencyCharacterOffset) * in_spaceWidth) + ((currencyCharacterOffset + 3 + (3 - in_numberStringList[index].length())) * in_characterWidth),
                 in_yOffset,
                 in_numberStringList[index].c_str());
  }
  
  switch (in_currencyToDraw)
  {
    case Currency::euro:
      drawEuroCharacter(in_xOffset + (3 * in_spaceWidth) + (7 * in_characterWidth), in_yOffset);
      break;

    case Currency::usDollar:
      drawUsDollarCharacter(in_xOffset + (3 * in_spaceWidth) + (7 * in_characterWidth), in_yOffset);
      break;
  }
}

void updateIndicatorBitcoinUpdate(int32_t in_integer)
{
  if (in_integer == displayIndicatorBitcoinUpdate_true)
  {
    oled.drawFrame(0, 0, 128, 64);
  }
}

void updateScreenBitcoin(const String& in_price, const String& in_priceTimestamp, int32_t in_integer)
{
  updateIndicatorBatteryLow(in_integer);
  updateIndicatorBitcoinUpdate(in_integer);

  static const uint8_t spaceWidth = 4;
  static const uint8_t xOffset = 10;
  static const uint8_t yOffsetA = 24;
  static const uint8_t yOffsetB = yOffsetA + 8 + 14;

  oled.setFont(u8g2_font_baby_tr);
  oled.drawStr(xOffset, yOffsetA / 2, in_priceTimestamp.c_str());

  oled.setFont(u8g2_font_chargen_92_me);
  static const u8g2_uint_t characterWidth = oled.getStrWidth("c");
  oled.drawStr(xOffset + (2 * spaceWidth) + (6 * characterWidth), yOffsetA, "1");
  drawBitcoinCharacter(xOffset + (3 * spaceWidth) + (7 * characterWidth), yOffsetA);

  if (in_price.startsWith("Pri"))
  {
    drawStringXCenter(yOffsetB, in_price.c_str());
  }
  else
  {
    drawNumberString(xOffset, yOffsetB, spaceWidth, characterWidth, splitNumberStringAtThousandsPlace(in_price), Currency::euro);
  }
}

void updateScreenBlockHeight(const String& in_blockHeight, const String& in_blockHeightTimestamp, int32_t in_integer)
{
  updateIndicatorBatteryLow(in_integer);

  static const uint8_t spaceWidth = 4;
  static const uint8_t xOffset = 10;
  static const uint8_t yOffsetA = 24;
  static const uint8_t yOffsetB = yOffsetA + 8 + 14;

  oled.setFont(u8g2_font_baby_tr);
  oled.drawStr(xOffset, yOffsetA / 2, in_blockHeightTimestamp.c_str());

  oled.setFont(u8g2_font_chargen_92_me);
  static const u8g2_uint_t characterWidth = oled.getStrWidth("c");
  drawBitcoinCharacter(xOffset + (3 * spaceWidth) + (7 * characterWidth), yOffsetA);

  oled.setFont(u8g2_font_streamline_design_t);
  oled.drawGlyph(xOffset + (3 * spaceWidth) + (5 * characterWidth), yOffsetA + 4, iconBlockHeight);

  oled.setFont(u8g2_font_chargen_92_me);

  if (in_blockHeight.startsWith("Hei"))
  {
    drawStringXCenter(yOffsetB, in_blockHeight.c_str());
  }
  else
  {
    drawNumberString(xOffset, yOffsetB, spaceWidth, characterWidth, splitNumberStringAtThousandsPlace(in_blockHeight), Currency::none);
  }
}

void updateOled(AppMode in_appMode, const String& in_string, const String& in_string2, int32_t in_integer, const CredentialsListPtr in_credentialListPtr)
{
  if (timeSinceOledUpdate >= OLED_UPDATE_INTERVAL)
  {
    oled.clearBuffer();
    
    switch (in_appMode)
    {
      case AppMode::config:
        updateScreenConfig(in_string);
      break;

      case AppMode::readWiFiQRCode:
        updateScreenReadWiFiQRCode();
      break;

      case AppMode::loadWiFiCredentials:
        updateScreenLoadWiFiCredentials(in_credentialListPtr);
      break;

      case AppMode::connectToWiFi:
        updateScreenConnectToWiFi(in_integer);
      break;

      case AppMode::connectToWiFiFailed:
        updateScreenConnectToWiFiFailed();
      break;

      case AppMode::clock:
        updateScreenClock(in_string, in_integer);
        updateBrightnessAdjustmentIndicator();
      break;

      case AppMode::bitcoin:
        updateScreenBitcoin(in_string, in_string2, in_integer);
        updateBrightnessAdjustmentIndicator();
      break;

      case AppMode::blockHeight:
        updateScreenBlockHeight(in_string, in_string2, in_integer);
        updateBrightnessAdjustmentIndicator();
      break;
    }

    oled.sendBuffer();
    timeSinceOledUpdate = 0;
  }
}
