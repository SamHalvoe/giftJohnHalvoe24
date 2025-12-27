#pragma once

#include <SparkFun_CAP1203.h>
#include <elapsedMillis.h>

class TouchInput
{
  private:
    enum class TouchState : uint8_t
    {
      isReleased = 0,
      isPressed,
      isTapped
    };

  private:
    CAP1203 m_touchSensor;
    TouchState m_leftPadState = TouchState::isReleased;
    TouchState m_middlePadState = TouchState::isReleased;
    TouchState m_rightPadState = TouchState::isReleased;
    elapsedMillis m_leftPadPressedFor;
    elapsedMillis m_middlePadPressedFor;
    elapsedMillis m_rightPadPressedFor;
    bool m_wasLeftPressedFor = false;
    bool m_wasMiddlePressedFor = false;
    bool m_wasRightPressedFor = false;

  private:
    void updateLeft()
    {
      if (m_touchSensor.isLeftTouched())
      {
        if (m_leftPadState != TouchState::isPressed)
        {
          m_leftPadPressedFor = 0;
        }

        m_leftPadState = TouchState::isPressed;
      }
      else if (m_leftPadState == TouchState::isPressed && not m_wasLeftPressedFor)
      {
        m_leftPadState = TouchState::isTapped;
      }
      else
      {
        m_leftPadState = TouchState::isReleased;

        if (m_wasLeftPressedFor)
        {
          m_wasLeftPressedFor = false;
        }
      }
    }

    void updateMiddle()
    {
      if (m_touchSensor.isMiddleTouched())
      {
        if (m_middlePadState != TouchState::isPressed)
        {
          m_middlePadPressedFor = 0;
        }

        m_middlePadState = TouchState::isPressed;
      }
      else if (m_middlePadState == TouchState::isPressed && not m_wasMiddlePressedFor)
      {
        m_middlePadState = TouchState::isTapped;
      }
      else
      {
        m_middlePadState = TouchState::isReleased;

        if (m_wasMiddlePressedFor)
        {
          m_wasMiddlePressedFor = false;
        }
      }
    }

    void updateRight()
    {
      if (m_touchSensor.isRightTouched())
      {
        if (m_rightPadState != TouchState::isPressed)
        {
          m_rightPadPressedFor = 0;
        }

        m_rightPadState = TouchState::isPressed;
      }
      else if (m_rightPadState == TouchState::isPressed && not m_wasRightPressedFor)
      {
        m_rightPadState = TouchState::isTapped;
      }
      else
      {
        m_rightPadState = TouchState::isReleased;

        if (m_wasRightPressedFor)
        {
          m_wasRightPressedFor = false;
        }
      }
    }

  public:
    TouchInput() = default;

    bool begin()
    {
      if (not m_touchSensor.begin()) return false;
      m_touchSensor.setInterruptDisabled();
      m_touchSensor.setSensitivity(SENSITIVITY_32X);
      return true;
    }

    void update()
    {
      updateLeft();
      updateMiddle();
      updateRight();
    }

    bool isLeftTapped() const
    {
      return m_leftPadState == TouchState::isTapped;
    }

    bool isMiddleTapped() const
    {
      return m_middlePadState == TouchState::isTapped;
    }

    bool isRightTapped() const
    {
      return m_rightPadState == TouchState::isTapped;
    }

    bool isLeftPressedFor(unsigned long in_duration)
    {
      if (m_leftPadState == TouchState::isPressed && m_leftPadPressedFor >= in_duration)
      {
        m_wasLeftPressedFor = true;
        m_leftPadPressedFor = 0;
        return true;
      }

      return false;
    }

    bool isMiddlePressedFor(unsigned long in_duration)
    {
      if (m_middlePadState == TouchState::isPressed && m_middlePadPressedFor >= in_duration)
      {
        m_wasMiddlePressedFor = true;
        m_middlePadPressedFor = 0;
        return true;
      }

      return false;
    }

    bool isRightPressedFor(unsigned long in_duration)
    {
      if (m_rightPadState == TouchState::isPressed && m_rightPadPressedFor >= in_duration)
      {
        m_wasRightPressedFor = true;
        m_rightPadPressedFor = 0;
        return true;
      }

      return false;
    }
};

TouchInput touchInput;
