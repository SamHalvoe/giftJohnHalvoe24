#pragma once

#include <vector>
#include <array>
#include <memory>
#include <limits>

struct WiFiCredentials
{
  public:
    bool m_isComplete = false;
    String m_ssid;
    String m_password;
};

using CredentialsList = std::vector<WiFiCredentials>;
using CredentialsListPtr = std::shared_ptr<CredentialsList>;

std::size_t credentialsSelectionIndex = 0;
const std::size_t INVALID_INDEX = std::numeric_limits<std::size_t>::max();
std::size_t indexOfLastSelectedCredentials = INVALID_INDEX;
std::size_t brightnessLevelIndex = 4;
std::size_t fontIndex = 0;

std::array<unsigned char, 1024> currentWidgetScreen;

enum class AppMode : uint8_t
{
  config = 0,
  readWiFiQRCode,
  loadWiFiCredentials,
  loadLastWiFiCredentials,
  errorWiFiCredentials,
  connectToWiFi,
  connectToWiFiFailed,
  clock,
  bitcoin,
  blockHeight,
  widget
};

AppMode currentAppMode = AppMode::config;

enum class Currency : uint8_t
{
  none = 0,
  euro,
  usDollar
};

Currency currentCurrency = Currency::euro;
