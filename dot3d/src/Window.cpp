#include "Window.h"

namespace dot3d { Window::WindowClass Window::WindowClass::s_windowClass;

Window::Exception::Exception(const char* file, int line, HRESULT hr) noexcept
	: m_hres(hr), m_file(file), m_line(line)
{
}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << Name() << " " << "[Error code]" << GetErrorCode() << '\n'
		<< "[Description]" << GetErrorString() << '\n'
		<< GetOriginString();
	m_buffer = oss.str();
	return m_buffer.c_str();
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return m_hres;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hres);
}

std::string Window::Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_file << '\n'
		<< "[Line] " << m_line;
	return oss.str();
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char* msgBuf = nullptr;
	DWORD msgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&msgBuf), 0, nullptr);
	
	if (msgLen == 0) return "Unidentified error code";

	std::string errorStr = msgBuf;
	LocalFree(msgBuf);
	return errorStr;
}

}

dot3d::Window::WindowClass::WindowClass() noexcept
	: m_hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hbrBackground = nullptr;
	wc.hCursor = nullptr;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();

	RegisterClassEx(&wc);
}

dot3d::Window::WindowClass::~WindowClass()
{
	UnregisterClass(s_windowClassName, GetInstance());
}

const wchar_t* dot3d::Window::WindowClass::GetName() noexcept
{
	return s_windowClassName;
}

HINSTANCE dot3d::Window::WindowClass::GetInstance() noexcept
{
	return s_windowClass.m_hInst;
}

dot3d::Window::Window(unsigned int width, unsigned int height, const wchar_t* name)
{
	RECT windRect;
	windRect.left = 100;
	windRect.right = windRect.left + width;
	windRect.top = 100;
	windRect.bottom = windRect.top + height;

	AdjustWindowRect(&windRect, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, false);

	m_hWnd = CreateWindow(WindowClass::GetName(), name,
						WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
						CW_USEDEFAULT, CW_USEDEFAULT, 
						windRect.right - windRect.left, windRect.bottom - windRect.top,
						nullptr, nullptr, WindowClass::GetInstance(), this);

	if (!m_hWnd) throw DOT_EXCEPT_LAST();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

dot3d::Window::~Window()
{
	DestroyWindow(m_hWnd);
}

LRESULT WINAPI dot3d::Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const create = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const wnd = static_cast<Window*>(create->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgAdapt));
		return wnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI dot3d::Window::HandleMsgAdapt(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return wnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT dot3d::Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;	// destruction done in destructor

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
