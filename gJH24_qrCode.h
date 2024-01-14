#pragma once

#include <elapsedMillis.h>
#include <tiny_code_reader.h>

tiny_code_reader_results_t tinyCodeReaderResults = {};
const uint16_t CODE_READ_INTERVAL = 250; // ms

String readCode()
{
  static elapsedMillis timeSinceCodeRead = CODE_READ_INTERVAL;

  if (timeSinceCodeRead >= CODE_READ_INTERVAL)
  {
    if (tiny_code_reader_read(&tinyCodeReaderResults) &&
        tinyCodeReaderResults.content_length > 0)
    {
      return String(reinterpret_cast<char*>(tinyCodeReaderResults.content_bytes));
    }

    timeSinceCodeRead = 0;
  }

  return String();
}