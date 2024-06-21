#pragma once

#include "WinDef.h"
#include "Keyboard.h"

#include "assert_except.h"
using namespace util;

#include <source_location>
#include <sstream>


namespace dot3d
{
	class Window
	{
	public:
		class Exception : public ae::ExceptionBuffered
		{
		public:
			Exception(const char* file, int line, HRESULT hr) noexcept;
			const char* what() const noexcept override;
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorString() const noexcept;
			std::string GetOriginString() const noexcept;
			static std::string TranslateErrorCode(HRESULT hr);
		private:
			virtual std::string Name() const override { return "Window::Exception"; }
			HRESULT m_hres;
			std::string m_file;
			int m_line;
		};
	private:
		class WindowClass
		{
		public:
			static const wchar_t* GetName() noexcept;
			static HINSTANCE GetInstance() noexcept;
		private:
			WindowClass() noexcept;
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;
			static constexpr const wchar_t* s_windowClassName = L"dot3d window";
			static WindowClass s_windowClass;
			HINSTANCE m_hInst;
		};

	public:
		Window(unsigned int width, unsigned int height, const wchar_t* name);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

	public:
		Keyboard kbd;

	private:
		static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI HandleMsgAdapt(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);	// allows to use HandleMsg
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		int m_width = 100, m_height = 100;
		HWND m_hWnd;
	};
}

#define DOT_EXCEPT(hr) dot3d::Window::Exception(__FILE__, __LINE__, hr)
#define DOT_EXCEPT_LAST() dot3d::Window::Exception(__FILE__, __LINE__, GetLastError())