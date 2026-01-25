#pragma once

#include <time.h>
#include <esp_sntp.h>

const char* NTP_SERVER_NAME = "de.pool.ntp.org";
const char* TIME_ZONE = "CET-1CEST,M3.5.0,M10.5.0/3";

bool isTimeConfigured = false;
tm timeInfo;
String timeString = "TmUnset";

bool updateTime()
{
  return isTimeConfigured && getLocalTime(&timeInfo);
}

bool configurateTime()
{
  configTzTime(TIME_ZONE, NTP_SERVER_NAME);
  isTimeConfigured = true;
  return updateTime();
}

void getTime()
{
  time_t timeT = time(nullptr);
  timeInfo = *localtime(&timeT);
}

void getTimeString(bool in_includesSeconds = false)
{
  if (not isTimeConfigured)
  {
    timeString = "TmErrCfg";
    return;
  }

  timeString = "";

  if (timeInfo.tm_hour < 10) timeString.concat('0');
  timeString.concat(timeInfo.tm_hour);

  timeString.concat(':');
  if (timeInfo.tm_min < 10) timeString.concat('0');
  timeString.concat(timeInfo.tm_min);

  if (in_includesSeconds)
  {
    timeString.concat(':');
    if (timeInfo.tm_sec < 10) timeString.concat('0');
    timeString.concat(timeInfo.tm_sec);
  }
}
