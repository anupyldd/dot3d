#include "App.h"

namespace dot3d
{
	App::App()
		: m_wnd(640, 480, L"dot3d") { }

	int App::Start()
	{
		MSG msg;
		BOOL result;
		while (true)
		{
			if (const auto code = Window::ProcessMessage()) return *code;
			Update();
		}
	}

	void App::Update()
	{
		m_wnd.Gfx().ClearBuffer(0.0f, 0.0f, 0.0f);
		m_wnd.Gfx().Present();
	}
}