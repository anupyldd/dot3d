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
								&desc, &m_swapChain, &m_device, 
								nullptr, &m_context);

	ID3D11Resource* backBuffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer));
	if (backBuffer)
	{
		m_device->CreateRenderTargetView(backBuffer, nullptr, &m_target);
		backBuffer->Release();
	}
}

dot3d::Graphics::~Graphics()
{
	if (m_swapChain) m_swapChain->Release();
	if (m_context) m_context->Release();
	if (m_target) m_target->Release();
	if (m_device) m_device->Release();
}

void dot3d::Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r,g,b,1.0f };
	m_context->ClearRenderTargetView(m_target, color);
}

void dot3d::Graphics::Present()
{
	m_swapChain->Present(1u, 0u);
}
