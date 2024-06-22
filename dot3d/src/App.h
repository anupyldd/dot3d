#pragma once

#include "Window.h"
#include "WinDef.h"

namespace dot3d
{
	class App
	{
	public:
		App();
		int Start();

	private:
		void Update();

	private:
		Window wnd;
	};
}