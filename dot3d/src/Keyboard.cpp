#include "Keyboard.h"

bool dot3d::Keyboard::KeyIsPressed(uint8_t keyCode) const noexcept
{
    return m_keyStates[keyCode];
}

bool dot3d::Keyboard::KeyIsEmpty(uint8_t keyCode) const noexcept
{
    return m_keyBuf.empty();
}

std::optional<dot3d::Keyboard::Event> dot3d::Keyboard::ReadKey() noexcept
{
    std::optional<Keyboard::Event> evt = m_keyBuf.front();
    m_keyBuf.pop();
    return evt;
}

void dot3d::Keyboard::ClearKey() noexcept
{
    m_keyBuf = std::queue<Event>();
}

bool dot3d::Keyboard::CharIsEmpty() noexcept
{
    return m_charBuf.empty();
}

std::optional<char> dot3d::Keyboard::ReadChar() noexcept
{
    std::optional<char> evt = m_charBuf.front();
    m_keyBuf.pop();
    return evt;
}

void dot3d::Keyboard::ClearChar() noexcept
{
    m_charBuf = std::queue<char>();
}

void dot3d::Keyboard::Clear() noexcept
{
    ClearKey();
    ClearChar();
}

void dot3d::Keyboard::SetAutorepeat(bool on) noexcept
{
    m_autorepeatOn = on;
}

bool dot3d::Keyboard::AutorepeatIsOn() const noexcept
{
    return m_autorepeatOn;
}

void dot3d::Keyboard::OnKeyPressed(uint8_t keyCode) noexcept
{
    m_keyStates[keyCode] = true;
    m_keyBuf.push(Event(Event::TYPE::PRESS, keyCode));
    TrimBuffer(m_keyBuf);
}

void dot3d::Keyboard::OnKeyReleased(uint8_t keyCode) noexcept
{
    m_keyStates[keyCode] = false;
    m_keyBuf.push(Event(Event::TYPE::RELEASE, keyCode));
    TrimBuffer(m_keyBuf);
}

void dot3d::Keyboard::OnChar(char ch) noexcept
{
    m_charBuf.push(ch);
    TrimBuffer(m_charBuf);
}

void dot3d::Keyboard::ClearState() noexcept
{
    m_keyStates.reset();
}

template<class T>
inline void dot3d::Keyboard::TrimBuffer(std::queue<T>& buf) noexcept
{
    while (buf.size() > m_bufSize)
    {
        buf.pop();
    }
}