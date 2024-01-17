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

  private:
    uint8_t m_pin;
    PinState m_pinStatePressed;
    PinState m_currentPinState;
    PinState m_lastPinState;
    bool m_wasPressed;
    bool m_wasReleased;
    uint16_t m_debounceTime;
    elapsedMillis m_timeSinceEvent;

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

  public:
    SimpleButton(uint8_t in_pin, uint16_t in_debounceTime = 40, PinState in_pinStatePressed = PinState::low) :
      m_pin(in_pin),
      m_pinStatePressed(in_pinStatePressed),
      m_currentPinState(invertPinState(in_pinStatePressed)),
      m_lastPinState(invertPinState(in_pinStatePressed)),
      m_wasPressed(false),
      m_wasReleased(false),
      m_debounceTime(in_debounceTime),
      m_timeSinceEvent(in_debounceTime)
    {}

    void begin()
    {
      pinMode(m_pin, getPinMode());
    }

    void update()
    {
      m_lastPinState = m_currentPinState;
      m_currentPinState = static_cast<PinState>(digitalRead(m_pin));

      if (m_timeSinceEvent >= m_debounceTime && hasStateChanged())
      {
        m_wasPressed = isPressed();
        m_wasReleased = isReleased();
        m_timeSinceEvent = 0;
      }
      else
      {
        m_wasPressed = false;
        m_wasReleased = false;
      }
    }

    bool hasStateChanged() const
    {
      return m_currentPinState != m_lastPinState;
    }

    bool isPressed() const
    {
      return m_currentPinState == m_pinStatePressed;
    }

    bool isReleased() const
    {
      return m_currentPinState != m_pinStatePressed;
    }

    bool wasPressed() const
    {
      return m_wasPressed;
    }

    bool wasReleased() const
    {
      return m_wasReleased;
    }
};
