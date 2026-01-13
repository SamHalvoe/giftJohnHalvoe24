#pragma once

#include <WiFiClientSecure.h>

#include "arduino_secrets.h"
#include "gJH24_def.h"
#include "gJH24_WiFi.h"

const char* widgetDomain = "calibre.johanneskrug.de";
const char* widgetPath = "/";

void getWidgetScreen()
{
  if (not isConnectedToWifi())
  {
    Serial.println("Error: No WiFi!");
    return;
  }

  WiFiClientSecure client;
  client.setCACert(CERTIFICATE_WIDGET);

  if (not client.connect(widgetDomain, 443))
  {
    Serial.println("Error: Could not connect to widgetDomain!");
    return;
  }

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(widgetDomain);
  getRequest.concat(widgetPath);
  getRequest.concat(" HTTP/1.0");

  client.println(getRequest);
  client.print("Host: ");
  client.println(widgetDomain);
  client.println("Connection: close");
  client.println();

  while (client.connected())
  {
    String line = client.readStringUntil('\n');

    if (line == "\r") // end of response
    {
      break;
    }
  }

  // get payload (pixel data)
  currentWidgetScreen.fill(0);
  client.readBytes(currentWidgetScreen.data(), currentWidgetScreen.size());
  client.stop();
}
