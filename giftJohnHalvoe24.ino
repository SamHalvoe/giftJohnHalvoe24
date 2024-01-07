#include <elapsedMillis.h>
#include <U8g2lib.h>
#include <tiny_code_reader.h>

#include "gJH24_utility.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"

#include "SimpleButton.h"

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;

const uint8_t PIN_CS = SS;
const uint8_t PIN_MOSI = MOSI;
const uint8_t PIN_MISO_DC = MISO;
const uint8_t PIN_SCK = SCK;
const uint8_t PIN_RESET = 26;

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI oled(U8G2_R0, PIN_SCK, PIN_MOSI, PIN_CS, PIN_MISO_DC, PIN_RESET);
bool isOledMirrored = false;
elapsedMillis timeSinceOledUpdate;

const uint8_t PIN_BUTTON = 12;
SimpleButton button(PIN_BUTTON);

tiny_code_reader_results_t tinyCodeReaderResults = {};
elapsedMillis timeSinceCodeRead;

void onPressed()
{
  oled.setDisplayRotation(isOledMirrored ? U8G2_R0 : U8G2_MIRROR);

  isOledMirrored = not isOledMirrored;
}

void setup()
{
  Serial.begin(9600);

  while (not Serial) {}

  Serial.println("setup started");
  
  oled.begin();
  Wire.begin();
  button.begin();

  timeSinceOledUpdate = 0;
  timeSinceCodeRead = 0;

  Serial.println("setup finished");
}

void loop()
{
  if (button.isPressed())
  {
    onPressed();
  }

  if (not isConnectedToWifi && timeSinceCodeRead >= 250)
  {
    if (tiny_code_reader_read(&tinyCodeReaderResults) &&
        tinyCodeReaderResults.content_length > 0)
    {
      String qrCodeContent(reinterpret_cast<char*>(tinyCodeReaderResults.content_bytes));

      Serial.print("Found '");
      Serial.print(qrCodeContent);
      Serial.println("'");

      String ssid;
      String password;

      extractWiFiCredentials(qrCodeContent, ssid, password);
      connectToWifi(ssid, password);

      configurateTime();
    }

    timeSinceCodeRead = 0;
  }

  if (timeSinceOledUpdate >= 25)
  {
    String timeString = getLocalTimeString();
    Serial.println(timeString);

    oled.clearBuffer();
    oled.setFont(u8g2_font_mystery_quest_32_tn);
    oled.drawStr(4, 48, timeString.c_str());
    oled.sendBuffer();

    timeSinceOledUpdate = 0;
  }
}
