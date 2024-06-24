#pragma once

#include "WinDef.h"
#include <d3d11.h>

#include <wrl.h>
namespace dot3d { namespace wrl = Microsoft::WRL; }

#include "AssertExcept.h"
using namespace util;

namespace dot3d
{
	class Graphics
	{
	public:
		class HRException : public ae::ExceptionBuffered
		{
		public:
			HRException(const char* file, int line, HRESULT hr) noexcept;
			const char* what() const noexcept override;
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorString() const noexcept;
			std::string GetOriginString() const noexcept;
			static std::string TranslateErrorCode(HRESULT hr);
		private:
			virtual std::string Name() const override { return "Graphics::Exception"; }
			HRESULT m_hres;
			std::string m_file;
			int m_line;
		};
		class DeviceRemovedException : public HRException
		{
		public:
			DeviceRemovedException(const char* file, int line, HRESULT hr) noexcept;
		};

	public:
		Graphics(HWND hWnd);

		~Graphics() = default;
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

	public:
		void ClearBuffer(float r, float g, float b) noexcept;
		void Present();

	private:
		wrl::ComPtr<ID3D11Device> m_device = nullptr;
		wrl::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
		wrl::ComPtr<ID3D11DeviceContext> m_context = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> m_target = nullptr;
	};
}

// checks the hrc expression before throwing
// HRESULT hr has to be declared in local scope before calling DOT_EXCEPT_GFX
#define DOT_EXCEPT_GFX(hrc) if(FAILED(hr = (hrc))) throw Graphics::HRException(__FILE__, __LINE__, hr)
#define DOT_EXCEPT_DEVICE_REMOVED(hr) Graphics::DeviceRemovedException(__FILE__, __LINE__, hr)