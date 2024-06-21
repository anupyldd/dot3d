#pragma once

#include <queue>
#include <bitset>
#include <optional>
#include <cstdint>

namespace dot3d
{
	class Keyboard
	{
	public:
		class Event
		{
		public:
			enum class TYPE
			{
				PRESS, RELEASE, INVALID
			};
		public:
			Event() = default;
			Event(TYPE type, uint8_t code)
				: m_type(type), m_code(code) { }

			bool IsPress() const noexcept { return m_type == TYPE::PRESS; }
			bool IsRelease() const noexcept { return m_type == TYPE::RELEASE; }
			bool IsValid() const noexcept { return m_type != TYPE::INVALID; }
			uint8_t GetCode() const noexcept { return m_code; }

		private:
			TYPE m_type = TYPE::INVALID;
			uint8_t m_code = 0;
		};

	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;

		bool KeyIsPressed(uint8_t keyCode) const noexcept;
		bool KeyIsEmpty(uint8_t keyCode) const noexcept;
		std::optional<Event> ReadKey() noexcept;
		void ClearKey() noexcept;

		bool CharIsEmpty() noexcept;
		std::optional<char> ReadChar() noexcept;
		void ClearChar() noexcept;

		void Clear() noexcept;

		void SetAutorepeat(bool on) noexcept;
		bool AutorepeatIsOn() const noexcept;

	private:
		void OnKeyPressed(uint8_t keyCode) noexcept;
		void OnKeyReleased(uint8_t keyCode) noexcept;
		void OnChar(char ch) noexcept;
		void ClearState() noexcept;
		template<class T>
		static void TrimBuffer(std::queue<T>& buf) noexcept;

	private:
		static constexpr size_t m_keysNum = 256;
		static constexpr size_t m_bufSize = 16;
		
		bool m_autorepeatOn = false;

		std::bitset<m_keysNum> m_keyStates;
		std::queue<Event> m_keyBuf;
		std::queue<char> m_charBuf;

	private:
		friend class Window;
	};
}


