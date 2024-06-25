#include "Graphics.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

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

	HRESULT hr;
	DOT_EXCEPT_GFX(D3D11CreateDeviceAndSwapChain(nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr, D3D11_CREATE_DEVICE_DEBUG, 
								nullptr, 0,
								D3D11_SDK_VERSION,
								&desc, &m_swapChain, &m_device, 
								nullptr, &m_context));

	wrl::ComPtr<ID3D11Resource> backBuffer = nullptr;
	DOT_EXCEPT_GFX(m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	DOT_EXCEPT_GFX(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_target));
}

void dot3d::Graphics::DrawTestTriangle()
{
	struct Vertex
	{
		float x, y;
	};
	const Vertex vert[3] =
	{
		{0.0f,0.5f},
		{0.5f,-0.5f},
		{-0.5f,-0.5f}
	};

	wrl::ComPtr<ID3D11Buffer> vBuffer;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(vert);
	desc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &vert;

	HRESULT hr;
	DOT_EXCEPT_GFX(m_device->CreateBuffer(&desc, &data, &vBuffer));
	
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, vBuffer.GetAddressOf(), &stride, &offset);

	wrl::ComPtr<ID3DBlob> blob;

	// bind pixel shader
	wrl::ComPtr<ID3D11PixelShader> pixShader;
	DOT_EXCEPT_GFX(D3DReadFileToBlob(L"PixelShader.cso", &blob));
	DOT_EXCEPT_GFX(m_device->CreatePixelShader(blob->GetBufferPointer(), 
		blob->GetBufferSize(), nullptr, &pixShader));

	m_context->PSSetShader(pixShader.Get(), 0, 0);

	// bind vertex shader
	wrl::ComPtr<ID3D11VertexShader> vertShader;
	DOT_EXCEPT_GFX(D3DReadFileToBlob(L"VertexShader.cso", &blob));
	DOT_EXCEPT_GFX(m_device->CreateVertexShader(blob->GetBufferPointer(),
		blob->GetBufferSize(), nullptr, &vertShader));

	m_context->VSSetShader(vertShader.Get(), 0, 0);

	// input layout
	wrl::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	DOT_EXCEPT_GFX(m_device->CreateInputLayout(ied, (UINT)std::size(ied),
		blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout));
	m_context->IASetInputLayout(inputLayout.Get());

	// bind render target
	m_context->OMSetRenderTargets(1, m_target.GetAddressOf(), nullptr);

	// set primitive topology (triangle list)
	m_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set viewport
	D3D11_VIEWPORT vp;
	vp.Width = 640;
	vp.Height = 480;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_context->RSSetViewports(1, &vp);

	m_context->Draw((UINT)std::size(vert), 0);
}

void dot3d::Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r,g,b,1.0f };
	m_context->ClearRenderTargetView(m_target.Get(), color);
}

void dot3d::Graphics::Present()
{
	HRESULT hr;
	if (FAILED(hr = m_swapChain->Present(1, 0)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw DOT_EXCEPT_DEVICE_REMOVED(m_device->GetDeviceRemovedReason());
		else
			DOT_EXCEPT_GFX(hr);
	}
}

dot3d::Graphics::HRException::HRException(const char* file, int line, HRESULT hr) noexcept
	: m_hres(hr), m_file(file), m_line(line)
{
}

const char* dot3d::Graphics::HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << Name() << " " << "[Error code]" << GetErrorCode() << '\n'
		<< "[Description]" << GetErrorString() << '\n'
		<< GetOriginString();
	m_buffer = oss.str();
	return m_buffer.c_str();
}

HRESULT dot3d::Graphics::HRException::GetErrorCode() const noexcept
{
	return m_hres;
}

std::string dot3d::Graphics::HRException::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hres);
}

std::string dot3d::Graphics::HRException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_file << '\n'
		<< "[Line] " << m_line;
	return oss.str();
}

std::string dot3d::Graphics::HRException::TranslateErrorCode(HRESULT hr)
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

dot3d::Graphics::DeviceRemovedException::DeviceRemovedException(const char* file, int line, HRESULT hr) noexcept
	: HRException(file, line, hr)
{
}
