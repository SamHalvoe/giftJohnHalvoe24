#pragma once

#include <WiFi.h>

#include "gJH24_def.h"
#include "gJH24_utility.h"

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

bool connectToWifi(const WiFiCredentials& in_credentials)
{
  return WiFi.begin(in_credentials.m_ssid, in_credentials.m_password) == WL_CONNECTED;
}

bool isConnectedToWifi()
{
  return WiFi.status() == WL_CONNECTED;
}
