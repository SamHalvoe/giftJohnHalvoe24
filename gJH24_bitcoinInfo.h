#pragma once

#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"
#include "gJH24_WiFi.h"

constexpr const char* infoDomain = "http://api.coinlayer.com/live";
String bitcoinPrice;

String getBitcoinPrice()
{
  if (not isConnectedToWifi)
  {
    return "BTCErrNoWiFi";
  }

  String serverPath;
  serverPath.concat(infoDomain);
  serverPath.concat("?access_key=");
  serverPath.concat(CRYPTO_API_KEY);
  serverPath.concat("&symbols=BTC");

  HTTPClient httpClient;
  httpClient.begin(serverPath.c_str());
  int httpResponseCode = httpClient.GET();
      
  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = httpClient.getString();
    Serial.println(payload);

    JsonDocument jsonDocument;
    deserializeJson(jsonDocument, payload);

    bitcoinPrice = jsonDocument["rates"]["BTC"].as<String>();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  httpClient.end();

  return bitcoinPrice;
}