#pragma once

#include <time.h>
#include <esp_sntp.h>

const char* NTP_SERVER_NAME = "de.pool.ntp.org";
const long GMT_OFFSET_SECONDS = 3600;
const int DAYLIGHT_OFFSET_SECONDS = 3600;

bool isTimeConfigured = false;
int lastCompleteHour = -1;
tm timeInfo;

String timeString = "TmUnset";

bool updateTime()
{
  return isTimeConfigured && getLocalTime(&timeInfo);
}

void configurateTime()
{
  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_NAME);
  isTimeConfigured = true;

  updateTime();
  lastCompleteHour = timeInfo.tm_hour;
}

void getLocalTimeString(bool in_includesSeconds = false)
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
