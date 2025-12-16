#pragma once

#include <elapsedMillis.h>
#include <tiny_code_reader.h>

const uint16_t CODE_READ_INTERVAL = 100; // ms
const uint8_t LED_REG_VALUE_ON = 0x01;
const uint8_t LED_REG_VALUE_OFF = 0x00;

String readQRCode()
{
  static elapsedMillis timeSinceCodeRead = CODE_READ_INTERVAL;

  if (timeSinceCodeRead >= CODE_READ_INTERVAL)
  {
    tiny_code_reader_results_t tinyCodeReaderResults = {};

    if (tiny_code_reader_read(&tinyCodeReaderResults) &&
        tinyCodeReaderResults.content_length > 0)
    {
      return String(reinterpret_cast<char*>(tinyCodeReaderResults.content_bytes));
    }

    timeSinceCodeRead = 0;
  }

  return String();
}
