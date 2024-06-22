#include "WinDef.h"
#include "WindowsMessageMap.h"
#include "Window.h"
#include "App.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int       nCmdShow)
{
	try
	{
		return dot3d::App{}.Start();
	}
	catch (const util::ae::ExceptionBuffered& e)
	{
		MessageBoxA(nullptr, e.what(), e.Name().c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}