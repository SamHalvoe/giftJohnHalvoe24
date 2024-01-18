#pragma once

#include <elapsedMillis.h>

class SimpleButton
{
  public:
    enum class PinState : int8_t
    {
      low = LOW,
      high = HIGH
    };

    enum class EnableState : int8_t
    {
      enabled = 0,
      disabled,
      disabledNext
    };

  private:
    uint8_t m_pin;
    PinState m_pinStatePressed;
    PinState m_currentPinState;
    PinState m_lastPinState;
    bool m_wasPressed;
    bool m_wasReleased;
    EnableState m_enableState;
    uint8_t m_debounceTime;
    elapsedMillis m_timeSinceStateChange;

  private:
    PinState invertPinState(PinState in_pinState) const
    {
      return (in_pinState == PinState::high ?
                PinState::low :
                PinState::high);
    }

    int getPinMode() const
    {
      return (m_pinStatePressed == PinState::high ?
                INPUT :
                INPUT_PULLUP);
    }

    bool handleDisabledNext()
    {
      if (hasStateChanged() && m_enableState == EnableState::disabledNext)
      {
        enable();

        return false;
      }

      return isEnabled();
    }
  
  public:
    SimpleButton(uint8_t in_pin, uint8_t in_debounceTime = 40, PinState in_pinStatePressed = PinState::low) :
      m_pin(in_pin),
      m_pinStatePressed(in_pinStatePressed),
      m_currentPinState(invertPinState(in_pinStatePressed)),
      m_lastPinState(invertPinState(in_pinStatePressed)),
      m_wasPressed(false),
      m_wasReleased(false),
      m_enableState(EnableState::enabled),
      m_debounceTime(in_debounceTime),
      m_timeSinceStateChange(in_debounceTime)
    {}

    void begin()
    {
      pinMode(m_pin, getPinMode());
    }

    void update()
    {
      m_lastPinState = m_currentPinState;
      m_currentPinState = static_cast<PinState>(digitalRead(m_pin));

      if (isEnabled() &&
          hasStateChanged() &&
          m_timeSinceStateChange >= m_debounceTime)
      {
        m_wasPressed = (m_currentPinState == m_pinStatePressed);
        m_wasReleased = not m_wasPressed;
        m_timeSinceStateChange = 0;
      }
      else
      {
        m_wasPressed = false;
        m_wasReleased = false;
      }
    }

    void enable()
    {
      m_enableState = EnableState::enabled;
    }

    void disable()
    {
      m_enableState = EnableState::disabled;;
    }

    void disableNext()
    {
      m_enableState = EnableState::disabledNext;
    }

    EnableState getEnableState() const
    {
      return m_enableState;
    }

    bool isEnabled() const
    {
      return m_enableState == EnableState::enabled;
    }

    bool hasStateChanged() const
    {
      return m_currentPinState != m_lastPinState;
    }

    uint32_t getTimeSinceStateChange() const
    {
      return m_timeSinceStateChange;
    }

    bool isPressed()
    {
      return handleDisabledNext() && m_currentPinState == m_pinStatePressed;
    }

    bool isReleased()
    {
      return handleDisabledNext() && m_currentPinState != m_pinStatePressed;
    }

    bool wasPressed()
    {
      return handleDisabledNext() && m_wasPressed;
    }

    bool wasReleased()
    {
      return handleDisabledNext() && m_wasReleased;
    }

    bool wasPressedFor(uint32_t in_milliseconds)
    {
      return isPressed() && m_timeSinceStateChange >= in_milliseconds;
    }

    bool wasReleasedFor(uint32_t in_milliseconds)
    {
      return isReleased() && m_timeSinceStateChange >= in_milliseconds;
    }
};
