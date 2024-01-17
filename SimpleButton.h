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
    elapsedMillis m_timeSinceRead;

  private:
    int8_t readPin()
    {
      m_timeSinceRead = 0;

      return digitalRead(m_pin);
    }

    void updatePinState()
    {
      m_lastPinState = m_currentPinState;
      m_currentPinState = readPin();
    }

  public:
    SimpleButton(uint8_t in_pin, uint8_t in_debounceTime = 40, bool in_isHighPressed = false) :
      m_pin(in_pin),
      m_debounceTime(in_debounceTime),
      m_timeSinceRead(in_debounceTime),
      m_pinStatePressed(in_isHighPressed ? HIGH : LOW),
      m_currentPinState(in_isHighPressed ? LOW : HIGH),
      m_lastPinState(in_isHighPressed ? LOW : HIGH)
    {}

    void begin()
    {
      pinMode(m_pin, INPUT_PULLUP);
    }

    bool isPressed()
    {
      updatePinState();

      return m_currentPinState == m_pinStatePressed;
    }

    bool isReleased()
    {
      updatePinState();

      return m_currentPinState != m_pinStatePressed;
    }

    bool isJustPressed()
    {
      if (m_timeSinceRead < m_debounceTime)
      {
        return false;
      }

      updatePinState();

      return (m_currentPinState != m_lastPinState && m_currentPinState == m_pinStatePressed); // if button is pressed
    }

    bool isJustReleased()
    {
      if (m_timeSinceRead < m_debounceTime)
      {
        return false;
      }

      updatePinState();

      return (m_currentPinState != m_lastPinState && m_currentPinState != m_pinStatePressed); // if button is just released
    }
};
