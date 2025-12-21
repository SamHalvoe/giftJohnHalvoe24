#pragma once

#include <vector>
#include <memory>

struct WiFiCredentials
{
  public:
    bool m_isComplete = false;
    String m_ssid;
    String m_password;
};

using CredentialsList = std::vector<WiFiCredentials>;
using CredentialsListPtr = std::shared_ptr<CredentialsList>;

enum class AppMode : uint8_t
{
  config = 0,
  readWiFiQRCode,
  loadWiFiCredentials,
  connectToWiFi,
  connectToWiFiFailed,
  clock,
  bitcoin,
  blockHeight
};

AppMode currentAppMode = AppMode::config;

enum class Currency : uint8_t
{
  none = 0,
  euro,
  usDollar
};
