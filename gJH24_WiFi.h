#pragma once

#include <WiFi.h>

//WiFiClient wifiClient;
bool isConnectedToWifi = false;

void connectToWifi(const String& in_ssid, const String& in_password)
{
  Serial.println("begin - connectToWifi");

  WiFi.begin(in_ssid, in_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  isConnectedToWifi = true;

  Serial.println();
  Serial.println("end - connectToWifi");
}
