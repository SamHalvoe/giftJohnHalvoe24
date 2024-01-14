#include "binary.h"
#pragma once

#include <elapsedMillis.h>
#include <U8g2lib.h>

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

enum class ScreenMode
{
  clock = 0,
  bitcoin = 1
};

void drawBitcoinCharacter(u8g2_uint_t in_x, u8g2_uint_t in_y)
{
  oled.setFont(u8g2_font_chargen_92_mr);
  oled.drawStr(in_x, in_y, "B");
  oled.drawBox(in_x + 3, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 6, in_y - 2 - 14, 2, 2);
  oled.drawBox(in_x + 3, in_y, 2, 2);
  oled.drawBox(in_x + 6, in_y, 2, 2);
}

void updateOled(ScreenMode in_screenMode, const String& in_string)
{
  if (timeSinceOledUpdate >= OLED_UPDATE_INTERVAL)
  {
    oled.clearBuffer();

    //oled.setFont(u8g2_font_mystery_quest_32_tn);
    //oled.drawStr(4, 48, in_string.c_str());
    
    if (in_screenMode == ScreenMode::bitcoin)
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

    oled.sendBuffer();

    timeSinceOledUpdate = 0;
  }
}
