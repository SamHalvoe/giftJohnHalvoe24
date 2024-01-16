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
bool isOledMirrored = false;
const uint16_t OLED_UPDATE_INTERVAL = 25; // ms
elapsedMillis timeSinceOledUpdate = OLED_UPDATE_INTERVAL;

constexpr const uint16_t iconQRCode = 48 + 6;
constexpr const uint16_t iconFloppy = 48 + 3;
constexpr const uint16_t yOffsetIcon = 32;

void toggleMirrorOled()
{
  oled.setDisplayRotation(isOledMirrored ? U8G2_R0 : U8G2_MIRROR);

  isOledMirrored = not isOledMirrored;
}

void drawBitcoinCharacter(u8g2_uint_t in_x, u8g2_uint_t in_y)
{
  oled.setFont(u8g2_font_chargen_92_mr);
  oled.drawStr(in_x, in_y, "B");
  oled.drawBox(in_x + 3, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 6, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 3, in_y, 2, 2);
  oled.drawBox(in_x + 6, in_y, 2, 2);
}

void drawStringXCenter(uint16_t in_y, const char* in_string)
{
  const uint16_t stringWidth = oled.getStrWidth(in_string);
  oled.drawStr((128 - stringWidth) / 2, in_y, in_string);
}

void updateScreenConfig()
{
  constexpr const uint16_t xLeftCenter = (64 - 21) / 2;
  constexpr const uint16_t xRightCenter = (196 - 21) / 2;

  oled.setFont(u8g2_font_streamline_phone_t);
  oled.drawGlyph(xLeftCenter, yOffsetIcon, iconQRCode);
  oled.setFont(u8g2_font_streamline_interface_essential_other_t);
  oled.drawGlyph(xRightCenter, yOffsetIcon, iconFloppy);

  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(48, "Select credentials source;");
  drawStringXCenter(60, "Press left or right;");
}

void updateScreenReadWiFiQRCode()
{
  constexpr const uint16_t xCenter = (128 - 21) / 2;

  oled.setFont(u8g2_font_streamline_phone_t);
  oled.drawGlyph(xCenter, yOffsetIcon, iconQRCode);
  
  oled.setFont(u8g2_font_glasstown_nbp_tr);
  drawStringXCenter(48, "Read WiFi QR Code;");
  drawStringXCenter(60, "Press right to cancle;");
}

void updateScreenLoadWiFiCredentials(const CredentialsListPtr in_credentialListPtr)
{
  if (not in_credentialListPtr)
  {
    constexpr const uint16_t xCenter = (128 - 21) / 2;

    oled.setFont(u8g2_font_streamline_interface_essential_other_t);
    oled.drawGlyph(xCenter, yOffsetIcon, iconFloppy);
    
    oled.setFont(u8g2_font_glasstown_nbp_tr);
    drawStringXCenter(48, "No saved credentials;");
    drawStringXCenter(60, "Press left to return;");
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
  drawStringXCenter(48, "Press any button to continue;");
}

void updateScreenClock(const String& in_string)
{
  oled.setFont(u8g2_font_mystery_quest_32_tn);
  drawStringXCenter(40, in_string.c_str());
}

void updateScreenBitcoin(const String& in_string)
{
  oled.setFont(u8g2_font_chargen_92_mr);
  const auto characterWidth = oled.getStrWidth("c");
  const uint8_t spaceWidth = 4;
  const uint8_t xOffset = 10;
  const uint8_t yOffsetA = 24;
  const uint8_t yOffsetB = yOffsetA + 8 + 14;

  oled.drawStr(xOffset + spaceWidth + (3 * characterWidth), yOffsetA, "1");
  drawBitcoinCharacter(xOffset + (2 * spaceWidth) + (4 * characterWidth), yOffsetA);

  auto numberStringList = splitString(in_string, { ',' });
  size_t index = 0;

  if (numberStringList.size() >= 3)
  {
    oled.drawStr(xOffset, yOffsetB, numberStringList[index].c_str());

    ++index;
  }

  if (numberStringList.size() >= 2)
  {
    oled.drawStr(xOffset + spaceWidth + ((1 + (3 - numberStringList[index].length())) * characterWidth), yOffsetB, numberStringList[index].c_str());

    ++index;
  }

  if (numberStringList.size() >= 1)
  {
    oled.drawStr(xOffset + (2 * spaceWidth) + ((4 + (3 - numberStringList[index].length())) * characterWidth), yOffsetB, numberStringList[index].c_str());
  }
  
  oled.drawStr(xOffset + (3 * spaceWidth) + (7 * characterWidth), yOffsetB, "$");
}

void updateOled(AppMode in_appMode, const String& in_string, int32_t in_integer, const CredentialsListPtr in_credentialListPtr)
{
  if (timeSinceOledUpdate >= OLED_UPDATE_INTERVAL)
  {
    oled.clearBuffer();
    
    switch (in_appMode)
    {
      case AppMode::config:
        updateScreenConfig();
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
        updateScreenClock(in_string);
      break;

      case AppMode::bitcoin:
        updateScreenBitcoin(in_string);
      break;
    }

    oled.sendBuffer();

    timeSinceOledUpdate = 0;
  }
}
