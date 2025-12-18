#pragma once

#include <SparkFun_CAP1203.h>

class TouchInput
{
  private:
    enum class TouchState : uint8_t
    {
      isReleased = 0,
      isPressed = 1,
      isTapped = 2
    };

  private:
    CAP1203 m_touchSensor;
    TouchState m_leftPadState = TouchState::isReleased;
    TouchState m_middlePadState = TouchState::isReleased;
    TouchState m_rightPadState = TouchState::isReleased;

  public:
    TouchInput() = default;

    bool begin()
    {
      if (not m_touchSensor.begin()) return false;
      m_touchSensor.setInterruptDisabled();
      m_touchSensor.setSensitivity(SENSITIVITY_32X);
      return true;
    }

    void reset()
    {
      m_leftPadState = TouchState::isReleased;
      m_middlePadState = TouchState::isReleased;
      m_rightPadState = TouchState::isReleased;
    }

    void update()
    {
      if (m_touchSensor.isLeftTouched())
      {
        m_leftPadState = TouchState::isPressed;
      }
      else if (m_leftPadState == TouchState::isPressed)
      {
        m_leftPadState = TouchState::isTapped;
      }
      else
      {
        m_leftPadState = TouchState::isReleased;
      }

      if (m_touchSensor.isMiddleTouched())
      {
        m_middlePadState = TouchState::isPressed;
      }
      else if (m_middlePadState == TouchState::isPressed)
      {
        m_middlePadState = TouchState::isTapped;
      }
      else
      {
        m_middlePadState = TouchState::isReleased;
      }

      if (m_touchSensor.isRightTouched())
      {
        m_rightPadState = TouchState::isPressed;
      }
      else if (m_rightPadState == TouchState::isPressed)
      {
        m_rightPadState = TouchState::isTapped;
      }
      else
      {
        m_rightPadState = TouchState::isReleased;
      }
    }

    bool isLeftTapped()
    {
      return m_leftPadState == TouchState::isTapped;
    }

    bool isMiddleTapped()
    {
      return m_middlePadState == TouchState::isTapped;
    }

    bool isRightTapped()
    {
      return m_rightPadState == TouchState::isTapped;
    }
};

TouchInput touchInput;
