#include "gJH24_def.h"
#include "gJH24_utility.h"
#include "gJH24_controller.h"
#include "gJH24_oled.h"
#include "gJH24_qrCode.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"
#include "gJH24_bitcoinInfo.h"

String timeString;

void setup()
{
  Serial.begin(9600);

  while (not Serial) {}

  Serial.println("setup started");
  
  oled.begin();
  Wire.begin();
  buttonA.begin();
  buttonB.begin();

  turnCodeReaderLedOff();
  
  Serial.println("setup finished");
}

void loop()
{
  handleApp(currentAppMode);

  /*if (not isConnectedToWifi)
  {
    String qrCodeContent = readCode();
    auto credentials = extractWiFiCredentials(qrCodeContent);

    if (credentials.m_isComplete)
    {
      connectToWifi(credentials);
      configurateTime();
    }
  }

  timeString = getLocalTimeString();*/

  updateOled(currentAppMode, "1,000,000");

  Serial.print("currentAppMode: ");
  Serial.println(static_cast<int16_t>(currentAppMode));
}
