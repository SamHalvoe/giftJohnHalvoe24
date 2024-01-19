#pragma once

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"
#include "gJH24_WiFi.h"

constexpr const char* infoDomain = "pro-api.coinmarketcap.com";
constexpr const char* infoPath = "/v2/cryptocurrency/quotes/latest";
String bitcoinPrice = "BTCUnset";

void updateBitcoinPrice()
{
  if (not isConnectedToWifi())
  {
    bitcoinPrice = "BTCNoWiFi";

    return;
  }

  WiFiClientSecure client;
  client.setCACert(CERTIFICATE);

  if (not client.connect(infoDomain, 443))
  {
    bitcoinPrice = "BTCErrCon";

    return;
  }

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(infoDomain);
  getRequest.concat(infoPath);
  getRequest.concat("?CMC_PRO_API_KEY=");
  getRequest.concat(CRYPTO_API_KEY);
  getRequest.concat("&symbol=BTC");
  getRequest.concat("&convert=EUR");
  getRequest.concat(" HTTP/1.0");

  client.println(getRequest);
  client.print("Host: ");
  client.println(infoDomain);
  client.println("Connection: close");
  client.println();

  String response;
  
  while (client.connected())
  {
    String line = client.readStringUntil('\n');

    if (line == "\r") // end of response
    {
      break;
    }

    response.concat(line);
    response.concat('\n');
  }

  String payload;

  while (client.available())
  {
    payload.concat(static_cast<char>(client.read()));
  }

  client.stop();

  JsonDocument jsonDocument;
  deserializeJson(jsonDocument, payload);

  bitcoinPrice = jsonDocument["data"]["BTC"][0]["quote"]["EUR"]["price"].as<String>();
}