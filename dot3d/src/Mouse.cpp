#include "WinDef.h"
#include "Mouse.h"

dot3d::MsPos dot3d::Mouse::GetPos() const noexcept
{
	return { m_x, m_y };
}

int dot3d::Mouse::GetX() const noexcept
{
	return m_x;
}

int dot3d::Mouse::GetY() const noexcept
{
	return m_y;
}

bool dot3d::Mouse::IsInsideWindow() const noexcept
{
	return m_insideClient;
}

bool dot3d::Mouse::LeftIsPressed() const noexcept
{
	return m_leftPressed;
}

bool dot3d::Mouse::RightIsPressed() const noexcept
{
	return m_rightPressed;
}

bool dot3d::Mouse::MiddleIsPressed() const noexcept
{
	return m_middleIsPressed;
}

bool dot3d::Mouse::IsEmpty() const noexcept
{
	return m_buf.empty();
}

std::optional<dot3d::Mouse::Event> dot3d::Mouse::Read() noexcept
{
	std::optional<Event> evt = m_buf.front();
	m_buf.pop();
	return evt;
}

void dot3d::Mouse::Clear() noexcept
{
	m_buf = std::queue<Event>();
}

void dot3d::Mouse::OnEvent(int x, int y, Event::TYPE type)
{
	m_x = x;
	m_y = y;
	m_buf.push(Event(type, *this));
	TrimBuffer();
}

void dot3d::Mouse::OnEnter()
{
	m_insideClient = true;
	m_buf.push(Event(Event::TYPE::ENTER, *this));
	TrimBuffer();
}

void dot3d::Mouse::OnLeave()
{
	m_insideClient = false;
	m_buf.push(Event(Event::TYPE::ENTER, *this));
	TrimBuffer();
}

void dot3d::Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	m_wheelDelta += delta;
	while (m_wheelDelta >= WHEEL_DELTA)
	{
		m_wheelDelta -= WHEEL_DELTA;
		OnEvent(x, y, Event::TYPE::WHEEL_UP);
	}
	while (m_wheelDelta <= -WHEEL_DELTA)
	{
		m_wheelDelta += WHEEL_DELTA;
		OnEvent(x, y, Event::TYPE::WHEEL_DOWN);
	}
}

void dot3d::Mouse::TrimBuffer() noexcept
{
	while (m_buf.size() > m_bufSize)
	{
		m_buf.pop();
	}
}
