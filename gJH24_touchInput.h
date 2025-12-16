#pragma once

#include <SparkFun_CAP1203.h>

class TouchInput
{
  public://private:
    CAP1203 m_touchSensor;

  public:
    TouchInput() = default;

    bool begin()
    {
      if (not m_touchSensor.begin()) return false;
      m_touchSensor.setInterruptDisabled();
      m_touchSensor.setSensitivity(SENSITIVITY_32X);
      return true;
    }
};

TouchInput touchInput;
