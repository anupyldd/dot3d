#include "App.h"

namespace dot3d
{
	App::App()
		: wnd(640, 480, L"dot3d") { }

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

	}
}