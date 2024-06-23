#include "Window.h"

#include "../resource.h"

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
	wc.hIcon = static_cast<HICON>(LoadImage(m_hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(m_hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
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
	: m_width(width), m_height(height)
{
	RECT windRect;
	windRect.left = 100;
	windRect.right = windRect.left + width;
	windRect.top = 100;
	windRect.bottom = windRect.top + height;

	if (!AdjustWindowRect(&windRect, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, false))
		throw DOT_EXCEPT_LAST();

	m_hWnd = CreateWindow(WindowClass::GetName(), name,
						WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
						CW_USEDEFAULT, CW_USEDEFAULT, 
						windRect.right - windRect.left, windRect.bottom - windRect.top,
						nullptr, nullptr, WindowClass::GetInstance(), this);

	if (m_hWnd) ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	else throw DOT_EXCEPT_LAST();

	m_gfx = std::make_unique<Graphics>(m_hWnd);
}

dot3d::Window::~Window()
{
	DestroyWindow(m_hWnd);
}

void dot3d::Window::SetTitle(const std::wstring& title) const noexcept
{
	if (!SetWindowText(m_hWnd, title.c_str())) throw DOT_EXCEPT_LAST();
}

std::optional<int> dot3d::Window::ProcessMessage()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT) return msg.wParam;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

dot3d::Graphics& dot3d::Window::Gfx()
{
	return *m_gfx;
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
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KILLFOCUS:
	{
		kbd.ClearState();
		break;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{	// filter autorepeat
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsOn())
		{
			kbd.OnKeyPressed(static_cast<uint8_t>(wParam));
		}
		break;
	}

	case WM_CHAR:
	{
		kbd.OnChar(static_cast<uint8_t>(wParam));
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		kbd.OnKeyReleased(static_cast<uint8_t>(wParam));
		break;
	}

	case WM_MOUSEMOVE:
	{
		// capures mouse movements if outside client but a btn is still pressed
		const POINTS pts = MAKEPOINTS(lParam);
		if (pts.x >= 0 && pts.x < m_width && pts.y >= 0 && pts.y < m_height)
		{
			ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::MOVE);
			if (!ms.IsInsideWindow())
			{
				SetCapture(m_hWnd);
				ms.OnEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_E_NOTBOUND))
			{
				ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::MOVE);
			}
			else
			{
				ReleaseCapture();
				ms.OnLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::L_PRESS);
		break;
	}

	case WM_LBUTTONUP:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::L_RELEASE);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::R_PRESS);
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::R_RELEASE);
		break;
	}

	case WM_MBUTTONDOWN:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::MIDDLE_PRESS);
		break;
	}

	case WM_MBUTTONUP:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		ms.OnEvent(pts.x, pts.y, Mouse::Event::TYPE::MIDDLE_RELEASE);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		ms.OnWheelDelta(pts.x, pts.y, delta);
		break;
	}

	default:
	{
		break;
	}

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
