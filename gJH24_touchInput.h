#pragma once

#include <SparkFun_CAP1203.h>
#include <elapsedMillis.h>

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
    elapsedMillis m_leftPadPressedFor;
    elapsedMillis m_middlePadPressedFor;
    elapsedMillis m_rightPadPressedFor;
    const unsigned long m_maxTapTime = 1000; // ms

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
      else if (m_leftPadState == TouchState::isPressed && getLeftPressedFor() <= m_maxTapTime)
      {
        m_leftPadState = TouchState::isTapped;
      }
      else
      {
        m_leftPadState = TouchState::isReleased;
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
      else if (m_middlePadState == TouchState::isPressed && getMiddlePressedFor() <= m_maxTapTime)
      {
        m_middlePadState = TouchState::isTapped;
      }
      else
      {
        m_middlePadState = TouchState::isReleased;
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
      else if (m_rightPadState == TouchState::isPressed && getRightPressedFor() <= m_maxTapTime)
      {
        m_rightPadState = TouchState::isTapped;
      }
      else
      {
        m_rightPadState = TouchState::isReleased;
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

    void resetStates()
    {
      m_leftPadState = TouchState::isReleased;
      m_middlePadState = TouchState::isReleased;
      m_rightPadState = TouchState::isReleased;
    }

    void resetPressedFor()
    {
      m_leftPadPressedFor = 0;
      m_middlePadPressedFor = 0;
      m_rightPadPressedFor = 0;
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

    unsigned long getLeftPressedFor() const
    {
      if (m_leftPadState == TouchState::isPressed)
      {
        return m_leftPadPressedFor;
      }

      return 0;
    }

    unsigned long getMiddlePressedFor() const
    {
      if (m_middlePadState == TouchState::isPressed)
      {
        return m_middlePadPressedFor;
      }

      return 0;
    }

    unsigned long getRightPressedFor() const
    {
      if (m_rightPadState == TouchState::isPressed)
      {
        return m_rightPadPressedFor;
      }

      return 0;
    }
};

TouchInput touchInput;
