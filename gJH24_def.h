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

enum class AppMode
{
  config = 0,
  readWiFiQRCode,
  loadWiFiCredentials,
  connectToWiFi,
  connectToWiFiFailed,
  clock,
  bitcoin
};

AppMode currentAppMode = AppMode::config;
