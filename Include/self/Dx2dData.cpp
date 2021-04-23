#include "Dx2dData.h"
#include <iostream> // using for output ERROR


D2dData::D2dData()
{

}

D2dData::~D2dData()
{
	if (D2D1Factory.Get() != NULL)
	{
		D2D1Factory->Release();
		D2D1Factory = nullptr;
	}
	if (D2D1DrawTarget.Get() != NULL)
	{
		D2D1DrawTarget->Release();
		D2D1DrawTarget = nullptr;
	}
	if (Brush.Get() != NULL)
	{
		Brush->Release();
		Brush = nullptr;
	}
}

#ifdef _WINDOWS_OLD_
void D2dData::Initialize(HWND hwnd, D2D1_FACTORY_TYPE factoryTypes)
{
	HRESULT hr = NULL;
	{
#if defined(DEBUG) || defined(_DEBUG)
		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		//hr = D2D1CreateFactory(factoryTypes, __uuidof(ID2D1Factory), options, IID_PPV_ARGS(&D2D1Factory));
		hr = D2D1CreateFactory(factoryTypes, IID_PPV_ARGS(&D2D1Factory));
#else
		hr = D2D1CreateFactory(factoryTypes, IID_PPV_ARGS(&D2D1Factory));
#endif
		if (FAILED(hr))
		{
			std::cout << "Init D2D1 Factory Error" << std::endl;
			return;
		}
	}

	// Create Render Target;
	GetClientRect(hwnd, &RenderRect);
	{
		hr = D2D1Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(RenderRect.right - RenderRect.left, RenderRect.bottom - RenderRect.top)),
			&D2D1DrawTarget
		);
		if (FAILED(hr))
		{
			std::cout << "Init D2d1 Draw Target Error." << std::endl;
			return;
		}
	}

	// Create Brush
	{
		hr = D2D1DrawTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&Brush
		);
		if (FAILED(hr))
		{
			std::cout << "Create Brush Error" << std::endl;
			return;
		}
	}
	// Create Text Format for draw TExt
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWriteFactory), reinterpret_cast<IUnknown**>(DWriteFactory.Get()));
		if (FAILED(hr))
		{
			std::cout << "DWriteFactory Create Error" << std::endl;
			return;
		}
		else
		{
			AddColor("White", 1.0f, 1.0f, 1.0f, 1.0f);
			AddColor("Black", 0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
	

}
#else
void D2dData::Initialize(HWND hwnd, D2D1_FACTORY_TYPE factoryTypes)
{
	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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

	D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&D3D11Device,
		SupportFeatureLevel,
		&D3D11Context
	);

    // using d3ddevice as Dxgi Device
	D3D11Device.As(&DXGIDevice);

#if defined(DEBUG) || defined(_DEBUG)
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	//hr = D2D1CreateFactory(factoryTypes, __uuidof(ID2D1Factory), options, IID_PPV_ARGS(&D2D1Factory));
	hr = D2D1CreateFactory(factoryTypes, IID_PPV_ARGS(&D2D1Factory));
#else
	hr = D2D1CreateFactory(factoryTypes, IID_PPV_ARGS(&D2D1Factory));
#endif
	if (FAILED(hr))
	{
		std::cout << "Init D2D1 Factory Error" << std::endl;
		return;
	}

	// Create D2DDeivce by using Dxgi Device
	D2D1Factory->CreateDevice(DXGIDevice.Get(),&D2D1Device);
	// Create D2DContext
	D2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &D2D1Context);

	DXGI_SWAP_CHAIN_DESC1 scDesc = {0};
	scDesc.Width = 0;
	scDesc.Height = 0;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = false;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
	scDesc.Flags = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC scDescFS = {0};
	scDescFS.RefreshRate.Denominator = 1;
	scDescFS.RefreshRate.Numerator = 60;
	scDescFS.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDescFS.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDescFS.Windowed = false;

    
	DXGIDevice->GetAdapter(&DXGIAdapter);

	DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));
	DXGIFactory->CreateSwapChainForHwnd(
		D3D11Device.Get(),
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&D2D1SwapChain
	);

	DXGIDevice->SetMaximumFrameLatency(1);

	D2D1SwapChain->GetBuffer(0, IID_PPV_ARGS(&D3DBackBuffer));

	D2D1_BITMAP_PROPERTIES1 backBufferBitmapProperties =
	    BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE)
		);
	
	// backBuffer for direct2d
    D2D1SwapChain->GetBuffer(0, IID_PPV_ARGS(&D2D1BackBuffer));

	D2D1Context->CreateBitmapFromDxgiSurface(D2D1BackBuffer.Get(),
	    &backBufferBitmapProperties,
		&D2D1RenderTargetMap
	);

	// And Set Bitmap As RenderTarget
	D2D1Context->SetTarget(D2D1RenderTargetMap.Get());

	// Create Brush
	{
		hr = D2D1Context->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&Brush
		);
		if (FAILED(hr))
		{
			std::cout << "Create Brush Error" << std::endl;
			return;
		}
	}
	// Create Text Format for draw TExt
	//{
	//	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWriteFactory), reinterpret_cast<IUnknown**>(DWriteFactory.Get()));
	//	if (FAILED(hr))
	//	{
	//		std::cout << "DWriteFactory Create Error" << std::endl;
	//		return;
	//	}
	//	else
	//	{
	//		AddColor("White", 1.0f, 1.0f, 1.0f, 1.0f);
	//		AddColor("Black", 0.0f, 0.0f, 0.0f, 1.0f);
	//	}
	//}

}

#endif

void D2dData::InitBrushColor(const std::string& colorName)
{
	if (Colors[colorName].get() == nullptr)
	{
		std::cout << "Change Brush Color Error, There is no this Name Color" << std::endl;
	}
	Brush->SetColor(Colors[colorName].get());
}

void D2dData::AddColor(const std::string& colorName, float r, float g, float b, float a)
{
	std::unique_ptr<D2D1::ColorF> c = std::make_unique<D2D1::ColorF>(r, g, b, a);

	Colors[colorName] = std::move(c);
}

void D2dData::AddTextFormat(const std::string& formatName,const WCHAR* fontName, float fontSize)
{
	HRESULT hr = DWriteFactory->CreateTextFormat(
		fontName, 
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"",
		&(TextFormats[formatName])
	);
	if (FAILED(hr))
		std::cout << "Create AddTextFormat is failed" << std::endl;

}

void D2dData::PerpareDraw(const std::string& colorName)
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->BeginDraw();
#else
    D2D1Context->BeginDraw();
#endif
    InitBrushColor(colorName);
}

void D2dData::AfterDraw()
{
	HRESULT hr = S_OK;
#ifdef _WINDOWS_OLD_
	hr = D2D1DrawTarget->EndDraw();
#else
    hr = D2D1Context->EndDraw();
#endif

	if (FAILED(hr))
		std::cout << "END Draw is Error" << std::endl;
	else
		std::cout << "Draw Done!" << std::endl;
}

void D2dData::Show()
{
	D2D1SwapChain->Present(1, 0);
}

void D2dData::ClearTarget()
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->Clear(Colors["White"].get());
#else
    D2D1Context->Clear(Colors["White"].get());
#endif

}


void D2dData::Resize()
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->CheckWindowState();
#endif

}

// ---------------Draw Function---------------------------------//

void D2dData::DrawRect(float left, float right, float top, float bottom)
{

	const D2D1_RECT_F rc = D2D1::RectF(
		RenderRect.left + left,
		RenderRect.top + top,
		RenderRect.right - right,
		RenderRect.bottom - bottom
	);
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->DrawRectangle(&rc, Brush.Get());
#else
    D2D1Context->DrawRectangle(&rc, Brush.Get());
#endif
}

void D2dData::DrawWord(const WCHAR* text, const std::string& textformatname, D2D1_RECT_F* drawrect)
{
	PerpareDraw("Black");
	//D2D1DrawTarget->DrawText(text, ARRAYSIZE(text) - 1, TextFormats[textformatname].Get(), drawrect, Brush);
}
