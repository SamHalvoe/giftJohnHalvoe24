#pragma once

#include "time.h"
#include "esp_sntp.h"

constexpr const char* NTP_SERVER_NAME = "de.pool.ntp.org";
constexpr const long GMT_OFFSET_SECONDS = 3600;
constexpr const int DAYLIGHT_OFFSET_SECONDS = 3600;

bool isTimeConfigured = false;
bool gotIsCompleteHour = false;
int lastCompleteHour = -1;

String timeString = "TmUnset";

void configurateTime()
{
  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_NAME);
  isTimeConfigured = true;
}

void updateLocalTimeString()
{
  if (not isTimeConfigured)
  {
    timeString = "TmErrCfg";
  }

  tm timeInfo;

  if (not getLocalTime(&timeInfo))
  {
    timeString = "TmErrGet";
  }

  timeString = "";

  if (timeInfo.tm_hour < 10) timeString.concat('0');
  timeString.concat(timeInfo.tm_hour);
  timeString.concat(':');
  
  if (timeInfo.tm_min < 10) timeString.concat('0');
  timeString.concat(timeInfo.tm_min);
  timeString.concat(':');

  if (timeInfo.tm_sec < 10) timeString.concat('0');
  timeString.concat(timeInfo.tm_sec);
}

bool isCompleteHour()
{
  if (not isTimeConfigured)
  {
    return false;
  }

  tm timeInfo;

  if (getLocalTime(&timeInfo))
  {
    if (timeInfo.tm_min == 0 && not gotIsCompleteHour)
    {
      gotIsCompleteHour = true;

      return true;
    }
  }

  if (lastCompleteHour == -1)
  {
    lastCompleteHour = timeInfo.tm_hour;
  }

  if (lastCompleteHour != timeInfo.tm_hour)
  {
    gotIsCompleteHour = false;
  }

  return false;
}
