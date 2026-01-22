#pragma once

#include <vector>

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"
#include "gJH24_def.h"
#include "gJH24_WiFi.h"

const char* widgetDomain = "clock.johanneskrug.de";
const char* widgetIdsPath = "/api/widgets/ids";
const char* widgetPath = "/api/widgets/";

//WiFiClientSecure widgetClient;
//WiFiClient widgetClient;
std::vector<String> widgetIds;

void setupWidgetClient()
{
  //widgetClient.setInsecure();
  //widgetClient.setCACert(CERTIFICATE_WIDGET);
}

void getWidgetIds()
{
  widgetIds.clear();

  if (not isConnectedToWifi())
  {
    Serial.println("Error: No WiFi!");
    return;
  }

  WiFiClient widgetClient;

  if (not widgetClient.connect(widgetDomain, 80))
  //if (not widgetClient.connect(widgetDomain, 443))
  {
    Serial.println("Error: Could not connect to widgetDomain!");
    return;
  }

  String getRequest = "GET ";
  getRequest.concat("http://");
  //getRequest.concat("https://");
  getRequest.concat(widgetDomain);
  getRequest.concat(widgetIdsPath);
  getRequest.concat(" HTTP/1.1");
  //Serial.println(getRequest);
  
  widgetClient.println(getRequest);
  widgetClient.print("Host: ");
  widgetClient.println(widgetDomain);
  widgetClient.println("Connection: close");
  widgetClient.println();

  String responseHeader;
  while (widgetClient.connected())
  {
    String line = widgetClient.readStringUntil('\n');
    if (line == "\r") break; // end of response header
    responseHeader.concat(line);
    responseHeader.concat('\n');
  }
  Serial.println(responseHeader);
  Serial.println(widgetClient.available());

  String payload(widgetClient.readString());
  widgetClient.stop();
  //Serial.println(payload);

  JsonDocument jsonDocument;
  deserializeJson(jsonDocument, payload);
  JsonArray array = jsonDocument.as<JsonArray>();
  
  for (JsonVariant element : array)
  {
    widgetIds.emplace_back(element.as<String>());
  }

  Serial.println(widgetIds.size());
}

void getWidgetScreen(const String& in_id)
{
  currentWidgetScreen.fill(0);

  if (not isConnectedToWifi())
  {
    Serial.println("Error: No WiFi!");
    return;
  }
  
  WiFiClient widgetClient;

  if (not widgetClient.connect(widgetDomain, 80))
  //if (not widgetClient.connect(widgetDomain, 443))
  {
    Serial.println("Error: Could not connect to widgetDomain!");
    return;
  }

  String getRequest = "GET ";
  getRequest.concat("http://");
  //getRequest.concat("https://");
  getRequest.concat(widgetDomain);
  getRequest.concat(widgetPath);
  getRequest.concat(in_id);
  getRequest.concat(" HTTP/1.1");
  Serial.println(getRequest);

  widgetClient.println(getRequest);
  widgetClient.print("Host: ");
  widgetClient.println(widgetDomain);
  widgetClient.println("Connection: close");
  widgetClient.println();

  String responseHeader;
  while (widgetClient.connected())
  {
    String line = widgetClient.readStringUntil('\n');
    if (line == "\r") break; // end of response header
    responseHeader.concat(line);
    responseHeader.concat('\n');
  }
  Serial.println(responseHeader);
  Serial.println(widgetClient.available());

  // get payload (pixel data)
  widgetClient.readBytes(currentWidgetScreen.data(), currentWidgetScreen.size());
  widgetClient.stop();
}
