#include "Dx2dData.h"
#include <iostream> // using for output ERROR
#include <memory>
#include <unordered_map>

inline void ThrowIfFailed(HRESULT hr, const std::string ErrorMessage)
{
	if (FAILED(hr))
	{
		std::cout << ErrorMessage << std::endl;
	}
}

D2dData::D2dData()
{

}

D2dData::~D2dData()
{
	if (m_D2DFactory.Get() != NULL)
	{
		m_D2DFactory->Release();
		m_D2DFactory = nullptr;
	}
	if (Brush.Get() != NULL)
	{
		Brush->Release();
		Brush = nullptr;
	}
}

D2dData::D2dData(HWND hwnd) :
	m_ScreenViewport(),
	m_SupportFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_D3DRenderTargetSize(),
	m_OutputSize(),
	m_logicalSize(),
	m_Dpi(-1.0f)
{
	GetClientRect(hwnd, &m_WindowRect);  //Set m_RenderRect;

	InitializeBase();
#ifdef _D2D_WITH_D3D_
	InitializeD2DWithD3D();
#else
	InitializeD2D();
#endif // _D2D_WITH_D3D_
	InitWindowOrResize();
}

void D2dData::InitializeBase()
{

	HRESULT hr = S_OK;
	// Create d2d1 Facotory
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
#if defined(DEBUG) || defined(_DEBUG)
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, &m_D2DFactory);
	if (FAILED(hr))
	{
		std::cout << "Init D2D1 Factory Error" << std::endl;
		return;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), &m_DWriteFactory);
	if (FAILED(hr))
	{
		std::cout << "Init DWrite facotry Error" << std::endl;
		return;
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WicFactory));
	if (FAILED(hr))
		std::cout << "Create WICFactory Error." << std::endl;

	AddColor("White", 1.0f, 1.0f, 1.0f, 1.0f);
	AddColor("Black", 0.0f, 0.0f, 0.0f, 1.0f);


}

void D2dData::InitializeD2D(HWND hwnd)
{
	HRESULT hr = S_OK;
	// Create Render Target;
	hr = m_D2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(m_WindowRect.right - m_WindowRect.left, m_WindowRect.bottom - m_WindowRect.top)),
		&m_D2D1DrawTarget);
	if (FAILED(hr))
	{
		std::cout << "Init D2d1 Draw Target Error." << std::endl;
		return;
	}

	// Create Brush
	hr = m_D2D1DrawTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&Brush);
	if (FAILED(hr))
	{
		std::cout << "Create Brush Error" << std::endl;
		return;
	}
}

void D2dData::InitializeD2DWithD3D()
{
	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined (_DEBUG) || defined (DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1 
	};

	//temp device and Context version
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&device,
		&m_SupportFeatureLevel,
		&context);
	if (FAILED(hr))
	{
		std::cout << "create d3d11 Device Error" << std::endl;
		return;
	}

	// Translate to current version d3d11Device and d3d11deviceContext Version
	hr = device.As(&m_D3DDevice);
	if (FAILED(hr))
		std::cout << "Convert D3dDevice version Error" << std::endl;
	hr = context.As(&m_D3DContext);
	if (FAILED(hr))
		std::cout << "Convert D3d11DeviceContext Version Error." << std::endl;

	// temp idxiDeivce
	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	hr = m_D3DDevice.As(&dxgiDevice);   // using d3ddevice as Dxgi Device
	if (FAILED(hr))
	{
		std::cout << "D3ddevice As DxgiDevice Error" << std::endl;
		return;
	}

	// Create D2DDeivce by using Dxgi Device
	hr = m_D2DFactory->CreateDevice(dxgiDevice.Get(), &m_D2DDevice);
	if (FAILED(hr))
	{
		std::cout << "Error to create D2ddevice." << std::endl;
		return;
	}

	// Create D2DContext
	hr = m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DContext);
	if (FAILED(hr))
	{
		std::cout << "Error to Create d2d1 Device Context." << std::endl;
		return;
	}
	// Create Brush
	hr = m_D2DContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &Brush);
	if (FAILED(hr))
	{
		std::cout << "Create Brush Error" << std::endl;
		return;
	}
}

void D2dData::InitWindowOrResize()
{
	HRESULT hr = S_OK;
	ID3D11RenderTargetView* nullViews[] = {nullptr};

	m_D3DContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_D3DRenderTargetView = nullptr;
	m_D2DContext->SetTarget(nullptr);
	m_D2DRenderTargetMap = nullptr;
	m_D3DContext->Flush();

	// Calculate necessary Render target size in pixel units
	m_OutputSize.width = m_logicalSize.width;
	m_OutputSize.height = m_logicalSize.height;

	// prevent zero size of directX content form being create
	m_OutputSize.width = (m_OutputSize.width == 0) ? 1 : m_OutputSize.width;
	m_OutputSize.height = (m_OutputSize.height == 0) ? 1 : m_OutputSize.height;

	// for Now Not Considering display rotate

	m_D3DRenderTargetSize.width = m_OutputSize.width;
	m_D3DRenderTargetSize.height = m_OutputSize.height;

	if (m_SwapChain != nullptr)
	{
		hr = m_SwapChain->ResizeBuffers(
			2, //double Buffer all need Resize
			lround(m_D3DRenderTargetSize.width),
			lround(m_D3DRenderTargetSize.height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			
		}
		else
		{

		}
	}
	else  // new Create SWAP chain using Swap Chain Desc
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc = { 0 };
		scDesc.Width = lround(m_D3DRenderTargetSize.width);
		scDesc.Height = lround(m_D3DRenderTargetSize.height);
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.Stereo = false;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = 2;
		scDesc.Scaling = DXGI_SCALING_NONE;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		scDesc.Flags = 0;
		scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		HRESULT hr;
		ComPtr<IDXGIDevice3> dxgiDevice;
		// Using D3d Device as DxgiDevice
		hr = m_D3DDevice.As(&dxgiDevice);
		if (FAILED(hr))
		{
			std::cout << "Convert Device as dxgiDevice Error." << std::endl;
			return;
		}
		
		ComPtr<IDXGIAdapter> dxgiAdpater;
		hr = dxgiDevice->GetAdapter(&dxgiAdpater);
		if (FAILED(hr))
		{
			std::cout << "Get dxgiAdpater Error from dxgi device " << std::endl;
			return;
		}

		ComPtr<IDXGIFactory3> dxgiFactory;

		hr = dxgiAdpater->GetParent(IID_PPV_ARGS(&dxgiFactory));
		if (FAILED(hr))
		{
			std::cout << "dxgiAdapter Get Parent Error" << std::endl;
			return;
		}
		
		hr = dxgiFactory->CreateSwapChainForHwnd(m_D3DDevice.Get(), m_HWND, &scDesc, nullptr, nullptr, &m_SwapChain);
		if (FAILED(hr))
			std::cout << "create Swap Chain Error" << std::endl;
		hr = dxgiDevice->SetMaximumFrameLatency(1);
		ThrowIfFailed(hr, "Set MaxinumFrame Error");
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	ThrowIfFailed(hr, "Get back buffer Error");

	hr = m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_D3DRenderTargetView);
	ThrowIfFailed(hr, "Create Render Target view Error");



	m_ScreenViewport = CD3D11_VIEWPORT(0.0f, 0.0f, m_D3DRenderTargetSize.width, m_D3DRenderTargetSize.height);

	UINT pNumberViews = 1;
	m_D3DContext->RSGetViewports(&pNumberViews, &m_ScreenViewport);

	D2D1_BITMAP_PROPERTIES1 backBufferBitmapProperties = BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
		m_Dpi, m_Dpi
	);

	Microsoft::WRL::ComPtr<IDXGISurface2> dxgiBackBuffer;
	// backBuffer for direct2d
	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	ThrowIfFailed(hr, "from swap Chain get dxgi surface is Error");

	hr = m_D2DContext->CreateBitmapFromDxgiSurface(
		dxgiBackBuffer.Get(),
		&backBufferBitmapProperties,
		&m_D2DRenderTargetMap
	);
	ThrowIfFailed(hr, "create d2d Render target map Error");

	// And Set Bitmap As RenderTarget
	m_D2DContext->SetTarget(m_D2DRenderTargetMap.Get());

	m_D2DContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

void D2dData::ReSize(HWND hwnd, float dpi)
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->CheckWindowState();
#else
	GetWindowRect(hwnd, &m_WindowRect);
	m_logicalSize = D2D1::SizeU(m_WindowRect.right - m_WindowRect.left, m_WindowRect.bottom - m_WindowRect.top);
	m_Dpi = dpi;
	m_D2DContext->SetDpi(m_Dpi, m_Dpi);
	InitWindowOrResize();
#endif
}

void D2dData::InitBrushColor(const std::string &colorName)
{
	if (Colors[colorName].get() == nullptr)
	{
		std::cout << "Change Brush Color Error, There is no this Name Color" << std::endl;
	}
	Brush->SetColor(Colors[colorName].get());
}

void D2dData::AddColor(const std::string &colorName, float r, float g, float b, float a)
{
	auto c = std::make_unique<D2D1::ColorF>(r, g, b, a);

	Colors[colorName] = std::move(c);
}

void D2dData::AddTextFormat(const std::string &formatName, const WCHAR *fontName, float fontSize)
{
	HRESULT hr = m_DWriteFactory->CreateTextFormat(
		fontName,
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"",
		&(TextFormats[formatName]));
	if (FAILED(hr))
		std::cout << "Create AddTextFormat is failed" << std::endl;
}

void D2dData::PerpareDraw(const std::string &colorName)
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->BeginDraw();
#else
	m_D2DContext->BeginDraw();
#endif
	InitBrushColor(colorName);
}

void D2dData::AfterDraw()
{
	HRESULT hr = S_OK;
#ifdef _WINDOWS_OLD_
	hr = D2D1DrawTarget->EndDraw();
#else
	hr = m_D2DContext->EndDraw();
#endif

	if (FAILED(hr))
		std::cout << "END Draw is Error" << std::endl;
	else
		std::cout << "Draw Done!" << std::endl;
}

void D2dData::Show()
{
	m_SwapChain->Present(1, 0);
}

void D2dData::ClearTarget()
{
#ifdef _WINDOWS_OLD_
	m_D2DDrawTarget->Clear(Colors["White"].get());
#else
	m_D2DContext->Clear(Colors["White"].get());
#endif
}

// ---------------Draw Function---------------------------------//

void D2dData::DrawRect(float left, float right, float top, float bottom)
{

	const D2D1_RECT_F rc = D2D1::RectF(
		m_WindowRect.left + left,
		m_WindowRect.top + top,
		m_WindowRect.right - right,
		m_WindowRect.bottom - bottom);
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->DrawRectangle(&rc, Brush.Get());
#else
	m_D2DContext->DrawRectangle(&rc, Brush.Get());
#endif
}

void D2dData::DrawWord(const WCHAR *text, const std::string &textformatname, D2D1_RECT_F *drawrect)
{
	PerpareDraw("Black");
	//D2D1DrawTarget->DrawText(text, ARRAYSIZE(text) - 1, TextFormats[textformatname].Get(), drawrect, Brush);
}
