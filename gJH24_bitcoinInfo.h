#pragma once

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>

#include "arduino_secrets.h"
#include "gJH24_WiFi.h"
#include "gJH24_Time.h"

const char* infoDomain = "mempool.space";
const char* infoPathPrice = "/api/v1/prices";
const char* infoPathBlockHeight = "/api/v1/mining/blocks/timestamp";

String bitcoinPriceTimestamp = "PTsUnset";
String bitcoinPrice = "PriUnset";
String blockHeightTimestamp = "HTsUnset";
String blockHeight = "HeiUnset";

String timeStructToString(const tm* in_timeStruct, bool in_includesSeconds = false)
{
  String timeString;
  
  if (in_timeStruct->tm_hour < 10) timeString.concat('0');
  timeString.concat(in_timeStruct->tm_hour);

  timeString.concat(':');
  if (in_timeStruct->tm_min < 10) timeString.concat('0');
  timeString.concat(in_timeStruct->tm_min);

  if (in_includesSeconds)
  {
    timeString.concat(':');
    if (in_timeStruct->tm_sec < 10) timeString.concat('0');
    timeString.concat(in_timeStruct->tm_sec);
  }

  return timeString;
}

void updateBitcoinPrice()
{
  if (not isConnectedToWifi())
  {
    bitcoinPrice = "PriNoWiFi";
    return;
  }

  WiFiClientSecure client;
  client.setCACert(CERTIFICATE);

  if (not client.connect(infoDomain, 443))
  {
    bitcoinPrice = "PriErrCon";
    return;
  }

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(infoDomain);
  getRequest.concat(infoPathPrice);
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
  
  time_t priceTimestap = jsonDocument["time"].as<time_t>() + GMT_OFFSET_SECONDS; // + GMT_OFFSET_SECONDS => 1 hour offset from UTC (to Berlin time)
  bitcoinPriceTimestamp = timeStructToString(gmtime(&priceTimestap));

  switch (currentCurrency)
  {
    case Currency::euro:
      bitcoinPrice = jsonDocument["EUR"].as<String>();
      break;

    case Currency::usDollar:
      bitcoinPrice = jsonDocument["USD"].as<String>();
      break;

    default:
      bitcoinPrice = "PriErrCur";
  }
}

void updateBlockHeight()
{
  if (not isConnectedToWifi())
  {
    blockHeight = "HeiNoWiFi";
    return;
  }

  WiFiClientSecure client;
  client.setCACert(CERTIFICATE);

  if (not client.connect(infoDomain, 443))
  {
    blockHeight = "HeiErrCon";
    return;
  }

  String getRequest = "GET ";
  getRequest.concat("https://");
  getRequest.concat(infoDomain);
  getRequest.concat(infoPathBlockHeight);
  getRequest.concat("/");
  getRequest.concat(time(nullptr));
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

  String heightTimestap = jsonDocument["timestamp"].as<String>();
  blockHeightTimestamp = String(heightTimestap.substring(11, 13).toInt() + 1) + heightTimestap.substring(13, 16);
  blockHeight = jsonDocument["height"].as<String>();
}
