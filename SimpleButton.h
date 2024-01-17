#pragma once

#include <elapsedMillis.h>

class SimpleButton
{
  private:
    uint8_t m_pin;
    int8_t m_pinStatePressed;
    int8_t m_currentPinState;
    int8_t m_lastPinState;
    uint8_t m_debounceTime;
    elapsedMillis m_timeSinceEvent;

  public:
    SimpleButton(uint8_t in_pin, uint8_t in_debounceTime = 40, bool in_isHighPressed = false) :
      m_pin(in_pin),
      m_pinStatePressed(in_isHighPressed ? HIGH : LOW),
      m_currentPinState(in_isHighPressed ? LOW : HIGH),
      m_lastPinState(in_isHighPressed ? LOW : HIGH),
      m_debounceTime(in_debounceTime),
      m_timeSinceEvent(in_debounceTime)
    {}

    void begin()
    {
      pinMode(m_pin, (m_pinStatePressed == HIGH ? INPUT : INPUT_PULLUP));
    }

    void update()
    {
      m_lastPinState = m_currentPinState;
      m_currentPinState = digitalRead(m_pin);
    }

    bool isPressed()
    {
      return m_currentPinState == m_pinStatePressed;
    }

    bool isReleased()
    {
      return m_currentPinState != m_pinStatePressed;
    }

    bool isJustPressed()
    {
      if (m_timeSinceEvent >= m_debounceTime &&
          m_currentPinState != m_lastPinState &&
          m_currentPinState == m_pinStatePressed)
      {
        m_timeSinceEvent = 0;

        return true;
      }

      return false;
    }

    bool isJustReleased()
    {
      if (m_timeSinceEvent >= m_debounceTime &&
          m_currentPinState != m_lastPinState &&
          m_currentPinState != m_pinStatePressed)
      {
        m_timeSinceEvent = 0;

        return true;
      }

      return false;
    }
};
