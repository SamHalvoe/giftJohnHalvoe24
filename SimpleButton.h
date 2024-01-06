#pragma once

#include <elapsedMillis.h>

class SimpleButton
{
  private:
    uint8_t m_pin;
    uint8_t m_debounceTime;
    elapsedMillis m_timeSincePressed;
    int8_t m_pressedPinState;
    int8_t m_currentPinState;
    int8_t m_lastPinState;

  public:
    SimpleButton(uint8_t in_pin, uint8_t in_debounceTime = 40, bool in_isHighPressed = false) :
      m_pin(in_pin),
      m_debounceTime(in_debounceTime),
      m_timeSincePressed(in_debounceTime),
      m_pressedPinState(in_isHighPressed ? HIGH : LOW),
      m_currentPinState(in_isHighPressed ? LOW : HIGH),
      m_lastPinState(in_isHighPressed ? LOW : HIGH)
    {}

    void begin()
    {
      pinMode(m_pin, INPUT_PULLUP);
    }

    bool isPressed()
    {
      if (m_timeSincePressed < m_debounceTime) // if button was recently pressed
      {
        return false;
      }

      m_lastPinState = m_currentPinState;
      m_currentPinState = digitalRead(m_pin);

      if (m_currentPinState != m_lastPinState && m_currentPinState == m_pressedPinState) // if button is pressed
      {
        m_timeSincePressed = 0;

        return true;
      }

      return false;
    }
};
