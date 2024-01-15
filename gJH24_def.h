#pragma once

enum class AppMode
{
  config = 0,
  readWiFiQRCode,
  loadWiFiData,
  clock,
  bitcoin
};

AppMode currentAppMode = AppMode::config;
