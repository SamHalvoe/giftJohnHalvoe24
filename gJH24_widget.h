#pragma once

#include <vector>

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"
#include "gJH24_def.h"
#include "gJH24_WiFi.h"

const char* widgetDomain = "calibre.johanneskrug.de";
const char* widgetIdsPath = "/api/widgets/ids";
const char* widgetPath = "/api/widgets/";

WiFiClientSecure widgetClient;
std::vector<String> widgetIds;

void setupWidgetClient()
{
  widgetClient.setCACert(CERTIFICATE_WIDGET);

  if (not isConnectedToWifi())
  {
    Serial.println("Error: No WiFi!");
    return;
  }

  if (widgetClient.connected()) return; // we are already connected

  if (not widgetClient.connect(widgetDomain, 443))
  {
    Serial.println("Error: Could not connect to widgetDomain!");
  }
}

void getWidgetIds()
{
  widgetIds.clear();

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(widgetDomain);
  getRequest.concat(widgetIdsPath);
  getRequest.concat(" HTTP/1.1");

  widgetClient.println(getRequest);
  widgetClient.print("Host: ");
  widgetClient.println(widgetDomain);
  widgetClient.println();

  while (widgetClient.connected())
  {
    String line = widgetClient.readStringUntil('\n');
    if (line == "\r") break; // end of response header
  }
  
  // get payload (widget id list)
  String payload(widgetClient.readStringUntil(']'));
  payload.concat(']');
  
  JsonDocument jsonDocument;
  deserializeJson(jsonDocument, payload);
  JsonArray array = jsonDocument.as<JsonArray>();
  
  for (JsonVariant element : array)
  {
    widgetIds.emplace_back(element.as<String>());
  }
}

void getWidgetScreen(const String& in_id)
{
  currentWidgetScreen.fill(0);

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(widgetDomain);
  getRequest.concat(widgetPath);
  getRequest.concat(in_id);
  getRequest.concat(" HTTP/1.1");

  widgetClient.println(getRequest);
  widgetClient.print("Host: ");
  widgetClient.println(widgetDomain);
  widgetClient.println();

  while (widgetClient.connected())
  {
    String line = widgetClient.readStringUntil('\n');
    if (line == "\r") break; // end of response header
  }

  // get payload (widget pixel data)
  widgetClient.readBytes(currentWidgetScreen.data(), currentWidgetScreen.size());
}
