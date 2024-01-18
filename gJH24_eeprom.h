#pragma once

#include <SparkFun_External_EEPROM.h>

#include "gJH24_def.h"

ExternalEEPROM eeprom;
bool isEepromInitialised = false;
const String EEPROM_IS_INITIALIZED_MARK = "EEPROM_IS_INITIALIZED_MARK";
const uint32_t EEPROM_FIELD_WIDTH = 64;
const uint32_t EEPROM_CREDENTIAL_WIDTH = 2 * EEPROM_FIELD_WIDTH;
uint32_t EEPROM_CREDENTIAL_COUNT_ADDRESS = 0;
uint32_t EEPROM_FIRST_CREDENTIAL_ADDRESS = 0;
uint32_t credentialCount = 0;

String readMark()
{
  String mark;

  for (size_t index = 0; index < EEPROM_IS_INITIALIZED_MARK.length(); ++index)
  {
    mark.concat(static_cast<char>(eeprom.read(index)));
  }

  return mark;
}

bool beginEeprom()
{
  eeprom.setMemoryType(512);

  if (not eeprom.begin())
  {
    return false;
  }

  EEPROM_CREDENTIAL_COUNT_ADDRESS = EEPROM_IS_INITIALIZED_MARK.length() + 1;

  if (readMark() == EEPROM_IS_INITIALIZED_MARK)
  {
    eeprom.get(EEPROM_CREDENTIAL_COUNT_ADDRESS, credentialCount);
  }
  else
  {
    String nonConstCopyOfMark = EEPROM_IS_INITIALIZED_MARK;
    eeprom.putString(0, nonConstCopyOfMark);
    eeprom.put(EEPROM_CREDENTIAL_COUNT_ADDRESS, credentialCount);
  }

  EEPROM_FIRST_CREDENTIAL_ADDRESS = EEPROM_CREDENTIAL_COUNT_ADDRESS + sizeof(credentialCount);
  isEepromInitialised = true;

  return true;
}

bool writeCredentials(uint32_t in_credentialIndex, const WiFiCredentials& in_credentials)
{
  if (not isEepromInitialised ||
      in_credentialIndex > credentialCount ||
      not in_credentials.m_isComplete ||
      in_credentials.m_ssid.length() > EEPROM_FIELD_WIDTH - 1 ||
      in_credentials.m_password.length() > EEPROM_FIELD_WIDTH - 1)
  {
    return false;
  }

  WiFiCredentials nonConstCopyOfCredentials = in_credentials;
  uint32_t address = EEPROM_FIRST_CREDENTIAL_ADDRESS + (in_credentialIndex * EEPROM_CREDENTIAL_WIDTH);
  eeprom.putString(address, nonConstCopyOfCredentials.m_ssid);
  address = address + EEPROM_FIELD_WIDTH;
  eeprom.putString(address, nonConstCopyOfCredentials.m_password);

  ++credentialCount;  
  eeprom.put(EEPROM_CREDENTIAL_COUNT_ADDRESS, credentialCount);

  return true;
}

bool appendCredentials(const WiFiCredentials& in_credentials)
{
  return writeCredentials(credentialCount, in_credentials);
}

WiFiCredentials readCredentials(uint32_t in_credentialIndex)
{
  if (not isEepromInitialised ||
      in_credentialIndex >= credentialCount)
  {
    return WiFiCredentials(); // with "m_isComplete = false" as mark for failure
  }

  WiFiCredentials credentials;

  uint32_t address = EEPROM_FIRST_CREDENTIAL_ADDRESS + (in_credentialIndex * EEPROM_CREDENTIAL_WIDTH);
  eeprom.getString(address, credentials.m_ssid);
  address = address + EEPROM_FIELD_WIDTH;
  eeprom.getString(address, credentials.m_password);
  credentials.m_isComplete = true;

  return credentials;
}

CredentialsListPtr readCredentialsList()
{
  if (not isEepromInitialised ||
      credentialCount == 0)
  {
    return nullptr;
  }

  CredentialsListPtr credentialsListPtr = std::make_shared<CredentialsList>();

  for (uint32_t index = 0; index < credentialCount; ++index)
  {
    credentialsListPtr->emplace_back(readCredentials(index));
  }

  return credentialsListPtr;
}

bool saveCredentials(const WiFiCredentials& in_credentials)
{
  CredentialsListPtr credentialsListPtr = readCredentialsList();

  if (credentialsListPtr)
  {
    for (size_t index = 0; index < credentialsListPtr->size(); ++index)
    {
      if (credentialsListPtr->at(index).m_ssid == in_credentials.m_ssid)
      {
        return writeCredentials(index, in_credentials);
      }
    }
  }

  return appendCredentials(in_credentials);
}
