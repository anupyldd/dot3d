#pragma once

#include "WinDef.h"
#include <d3d11.h>

namespace dot3d
{
	class Graphics
	{
	public:
		Graphics(HWND hWnd);
		~Graphics();

		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

	public:
		void Present();

	private:
		ID3D11Device* m_Device = nullptr;
		IDXGISwapChain* m_SwapChain = nullptr;
		ID3D11DeviceContext* m_Context = nullptr;
	};
}