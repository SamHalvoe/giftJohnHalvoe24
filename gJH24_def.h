#pragma once

enum class AppMode
{
  config = 0,
  readWiFiQRCode,
  loadWiFiData,
  connectToWiFi,
  connectToWiFiFailed,
  clock,
  bitcoin
};

AppMode currentAppMode = AppMode::config;
