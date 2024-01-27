#pragma once

const uint8_t VBAT_PIN = A13;

float getBatteryVoltage()
{
  float voltage = analogReadMilliVolts(VBAT_PIN);
  voltage = voltage * 2;    // we divided by 2, so multiply back
  voltage = voltage / 1000; // convert to volts!

  //Serial.print("Battery voltage: ");
  //Serial.println(voltage);

  return voltage;
}
