#include "Graphics.h"

#pragma comment(lib,"d3d11.lib")

dot3d::Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;
	desc.OutputWindow = hWnd;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;

	D3D11CreateDeviceAndSwapChain(nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr, 0, nullptr, 0,
								D3D11_SDK_VERSION,
								&desc, &m_SwapChain, &m_Device, 
								nullptr, &m_Context);
}

dot3d::Graphics::~Graphics()
{
	if (m_SwapChain) m_SwapChain->Release();
	if (m_Context) m_Context->Release();
	if (m_Device) m_Device->Release();
}

void dot3d::Graphics::Present()
{
	m_SwapChain->Present(1u, 0u);
}
