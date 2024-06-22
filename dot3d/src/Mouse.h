#pragma once

#include <queue>
#include <bitset>
#include <optional>
#include <cstdint>
#include <utility>

namespace dot3d
{
	using MsPos = std::pair<int, int>;

	class Mouse
	{
	public:
		class Event
		{
		public:
			enum class TYPE
			{
				L_PRESS, L_RELEASE,
				R_PRESS, R_RELEASE,
				MIDDLE_PRESS, MIDDLE_RELEASE,
				WHEEL_UP, WHEEL_DOWN,
				MOVE, LEAVE, ENTER,
				INVALID
			};
		public:
			Event() = default;
			Event(TYPE type, const Mouse& parentMs) noexcept
				: m_type(type), m_x(parentMs.m_x), m_y(parentMs.m_y),
					m_insideClient(parentMs.m_insideClient),
					m_leftPressed(parentMs.m_leftPressed), 
					m_rightPressed(parentMs.m_rightPressed),
					m_middleIsPressed(parentMs.m_middleIsPressed) { }

			bool IsValid() const noexcept { return m_type != TYPE::INVALID; }
			TYPE GetType() const noexcept { return m_type; }
			MsPos GetPos() { return { m_x,m_y }; }
			int GetX() const noexcept { return m_x; }
			int GetY() const noexcept { return m_y; }
			bool IsInsideWindow() const noexcept { return m_insideClient; }
			bool LeftIsPressed() const noexcept { return m_leftPressed; }
			bool RightIsPressed() const noexcept { return m_rightPressed; }
			bool MiddleIsPressed() const noexcept { return m_middleIsPressed; }

		private:
			TYPE m_type = TYPE::INVALID;
			int m_x = 0, m_y = 0;
			bool m_insideClient = false;
			bool m_leftPressed = false;
			bool m_rightPressed = false;
			bool m_middleIsPressed = false;
		};

	public:
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;

		MsPos GetPos() const noexcept;
		int GetX() const noexcept;
		int GetY() const noexcept;
		bool IsInsideWindow() const noexcept;
		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		bool MiddleIsPressed() const noexcept;

		bool IsEmpty() const noexcept;
		std::optional<Event> Read() noexcept;
		void Clear() noexcept;

	private:
		void OnEvent(int x, int y, Event::TYPE type);
		void OnEnter();
		void OnLeave();
		void TrimBuffer() noexcept;

	private:
		static constexpr uint8_t m_bufSize = 16;

		std::queue<Event> m_buf;
		int  m_x = 0, m_y = 0;
		int  m_wheelDelta = 0;
		bool m_insideClient = false;
		bool m_leftPressed = false;
		bool m_rightPressed = false;
		bool m_middleIsPressed = false;

	private:
		friend class Window;
	};
}
