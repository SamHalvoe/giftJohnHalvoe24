#include "gJH24_utility.h"
#include "gJH24_oled.h"
#include "gJH24_qrCode.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"

#include "SimpleButton.h"

const uint8_t PIN_BUTTON = 12;
SimpleButton button(PIN_BUTTON);

String timeString;

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

  Serial.println("setup finished");
}

void loop()
{
  if (button.isPressed())
  {
    onPressed();
  }

  if (not isConnectedToWifi)
  {
    String qrCodeContent = readCode();
    auto credentials = extractWiFiCredentials(qrCodeContent);

    if (credentials.m_isComplete)
    {
      connectToWifi(credentials);
      configurateTime();
    }
  }

  timeString = getLocalTimeString();
  //Serial.println(timeString);

  updateOled(ScreenMode::bitcoin, "1,000,000");
}
