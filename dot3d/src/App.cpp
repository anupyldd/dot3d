#include "App.h"

namespace dot3d
{
	App::App()
		: wnd(640, 480, L"dot3d") { }

	int App::Start()
	{
		MSG msg;
		BOOL result;
		while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			Update();
		}
		if (result == -1) throw DOT_EXCEPT_LAST();
		else return msg.wParam;
	}

	void App::Update()
	{

	}
}