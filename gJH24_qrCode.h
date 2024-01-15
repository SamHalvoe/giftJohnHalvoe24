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

// turn on and off causes trouble: I could not read any QR code, see https://github.com/usefulsensors/tiny_code_reader_arduino/issues/1
void turnCodeReaderLedOn()
{
  // naming error, see https://github.com/usefulsensors/tiny_code_reader_arduino/issues/1
  //person_sensor_write_reg(TINY_CODE_READER_REG_LED_STATE, LED_REG_VALUE_ON);
}

// turn on and off causes trouble: I could not read any QR code, see https://github.com/usefulsensors/tiny_code_reader_arduino/issues/1
void turnCodeReaderLedOff()
{
  // naming error, see https://github.com/usefulsensors/tiny_code_reader_arduino/issues/1
  //person_sensor_write_reg(TINY_CODE_READER_REG_LED_STATE, LED_REG_VALUE_OFF);
}
