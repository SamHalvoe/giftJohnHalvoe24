#pragma once

#include <WiFi.h>

#include "gJH24_utility.h"

bool isConnectedToWifi = false;

struct WiFiCredentials
{
  public:
    bool m_isComplete = false;
    String m_ssid;
    String m_password;
};

WiFiCredentials extractWiFiCredentials(const String& in_qrCodeContent)
{
  auto stringList = splitString(in_qrCodeContent, { ':', ';' });
  WiFiCredentials credentials;
  
  for (size_t index = 0; index < stringList.size(); ++index)
  {
    if (stringList[index] == "S")
    {
      ++index;

      credentials.m_ssid = stringList[index];
    }

    if (stringList[index] == "P")
    {
      ++index;

      credentials.m_password = stringList[index];
    }

    if (credentials.m_ssid.length() > 0 && credentials.m_password.length() > 0)
    {
      credentials.m_isComplete = true;

      return credentials;
    }
  }

  return credentials;
}

void connectToWifi(const WiFiCredentials& in_credentials)
{
  Serial.println("begin - connectToWifi");

  WiFi.begin(in_credentials.m_ssid, in_credentials.m_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  isConnectedToWifi = true;

  Serial.println();
  Serial.println("end - connectToWifi");
}
