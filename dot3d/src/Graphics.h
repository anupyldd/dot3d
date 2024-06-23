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
		void ClearBuffer(float r, float g, float b) noexcept;
		void Present();

	private:
		ID3D11Device* m_device = nullptr;
		IDXGISwapChain* m_swapChain = nullptr;
		ID3D11DeviceContext* m_context = nullptr;
		ID3D11RenderTargetView* m_target = nullptr;
	};
}