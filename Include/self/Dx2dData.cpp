#include "Dx2dData.h"
#include <iostream> // using for output ERROR
#include <memory>
#include <unordered_map>
#include <DirectXMath.h>

inline void ThrowIfFailed(HRESULT hr, const std::string& ObjectName)
{
	if (FAILED(hr))
	{
		std::cout << "Create " << ObjectName << " Error" << std::endl;
		return;
	}
	else
	{
		std::cout << "Create " << ObjectName << " Success." << std::endl;
		return;
	}
}

D2dData::D2dData()
{

}

D2dData::~D2dData()
{

}

D2dData::D2dData(HWND hwnd) :
	m_ScreenViewport(),
	m_SupportFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_D3DRenderTargetSize(),
	m_OutputSize(),
	m_logicalSize(),
	m_Dpi(-1.0f),
	m_HWND(hwnd)
{
	GetClientRect(m_HWND, &m_ClientRect);  //Set m_RenderRect;
	GetWindowRect(m_HWND, &m_WindowRect);  
	m_Dpi = GetDpiForWindow(m_HWND);
	D2D1_MATRIX_3X2_F cameraScale = D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0, -1.0f), D2D1::Point2(0.0f, 0.0f));
	D2D1_MATRIX_3X2_F cameraTranslate = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, m_ClientRect.bottom - m_ClientRect.top));
	m_ViewCamera = D2D1::Matrix3x2F(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, (m_ClientRect.bottom - m_ClientRect.top));


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
	ThrowIfFailed(hr, "D2D_Factory");

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), &m_DWriteFactory);
	ThrowIfFailed(hr, "DDwriteFactory");

	hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WicFactory));
	ThrowIfFailed(hr, "WicFactory");

}

void D2dData::InitializeD2D(HWND hwnd)
{
	HRESULT hr = S_OK;
	// Create Render Target;
	hr = m_D2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top)),
		&m_D2D1DrawTarget);
	ThrowIfFailed(hr, "D2D1DrawTarget");

	// Create Brush
	hr = m_D2D1DrawTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&Brush);
	ThrowIfFailed(hr, "D2D1Brush");
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
	ThrowIfFailed(hr, "D3Ddevice & d3dDeviceContext");

	// Translate to current version d3d11Device and d3d11deviceContext Version
	hr = device.As(&m_D3DDevice);
	ThrowIfFailed(hr, "convert D3ddevice");
	hr = context.As(&m_D3DContext);
	ThrowIfFailed(hr, "Convert D3dDeviceContext");

	// temp idxiDeivce
	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	hr = m_D3DDevice.As(&dxgiDevice);   // using d3ddevice as Dxgi Device
	ThrowIfFailed(hr, "d3ddevice AS DxgiDevice");

	// Create D2DDeivce by using Dxgi Device
	hr = m_D2DFactory->CreateDevice(dxgiDevice.Get(), &m_D2DDevice);
	ThrowIfFailed(hr, "D2d1Device");

	// Create D2DContext
	hr = m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DContext);
	ThrowIfFailed(hr, "D2d1DeviceContext");

}

void D2dData::InitWindowOrResize()
{
	HRESULT hr = S_OK;
	ID3D11RenderTargetView* nullViews[] = {nullptr};

	m_D3DContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_D3DRenderTargetView = nullptr;
	m_D3DContext->Flush();

	m_D2DRenderTargetMap = nullptr;
	m_D2DContext->SetTarget(nullptr);
	
	GetWindowRect(m_HWND, &m_WindowRect);
	GetClientRect(m_HWND, &m_ClientRect);
	
	m_logicalSize.width = m_ClientRect.right - m_ClientRect.left;
	m_logicalSize.height = m_ClientRect.bottom - m_ClientRect.top;
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
		scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
		ThrowIfFailed(hr, "convert Device AS DxgiDeivce");

		ComPtr<IDXGIAdapter> dxgiAdpater;
		hr = dxgiDevice->GetAdapter(&dxgiAdpater);
		ThrowIfFailed(hr, "dxgiAdapater");

		ComPtr<IDXGIFactory3> dxgiFactory;

		hr = dxgiAdpater->GetParent(IID_PPV_ARGS(&dxgiFactory));
		ThrowIfFailed(hr, "dxgiFactory");
		
		hr = dxgiFactory->CreateSwapChainForHwnd(m_D3DDevice.Get(), m_HWND, &scDesc, NULL, NULL, &m_SwapChain);
		ThrowIfFailed(hr, "Swap Chain");

		hr = dxgiDevice->SetMaximumFrameLatency(1);
		ThrowIfFailed(hr, "MaxinumFrame");
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	ThrowIfFailed(hr, "ID3D11Texxture2D");

	hr = m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_D3DRenderTargetView);
	ThrowIfFailed(hr, "D3DRenderTargetView");

	m_ScreenViewport = CD3D11_VIEWPORT(0.0f, 0.0f, m_D3DRenderTargetSize.width, m_D3DRenderTargetSize.height);

	UINT pNumberViews = 1;
	m_D3DContext->RSGetViewports(&pNumberViews, &m_ScreenViewport);

	D2D1_BITMAP_PROPERTIES1 backBufferBitmapProperties = BitmapProperties1
	(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
		(float)m_Dpi, (float)m_Dpi
	);


	Microsoft::WRL::ComPtr<IDXGISurface2> dxgiBackBuffer;
	// backBuffer for direct2d
	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	ThrowIfFailed(hr, "DXGI Surface");
	
	DXGI_SURFACE_DESC dsc;
	dxgiBackBuffer->GetDesc(&dsc);
	std::cout << static_cast<UINT>(dsc.Format) << std::endl;
	std::cout << static_cast<UINT>(backBufferBitmapProperties.pixelFormat.format) << std::endl;

	// ERRORONTHIS
	hr = m_D2DContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &backBufferBitmapProperties, &m_D2DRenderTargetMap );
	ThrowIfFailed(hr,  "BitMap from DXGI Surface");

	// And Set Bitmap As RenderTarget
	m_D2DContext->SetTarget(m_D2DRenderTargetMap.Get());

	m_D2DContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	m_D2DContext->SetTransform(D2D1::Matrix3x2F::Identity());  // Set direct 2d draw coordinate to origin.
}

void D2dData::ReSize(HWND hwnd, float dpi)
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->CheckWindowState();
#else
	GetWindowRect(hwnd, &m_ClientRect);
	m_logicalSize = D2D1::SizeU(m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top);
	m_Dpi = dpi;
	m_D2DContext->SetDpi((FLOAT)m_Dpi, (FLOAT)m_Dpi);
	InitWindowOrResize();
#endif

}
#ifdef _TEST_DRAW_



void D2dData::PerpareDraw(const D2D1::ColorF& drawColor)
{
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->BeginDraw();
#else
	m_D2DContext->BeginDraw();
#endif
	// Create Brush
	HRESULT hr = m_D2DContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &Brush);
	ThrowIfFailed(hr, "d2d1Brush");
	InitBrushColor(drawColor);
    ClearTarget();
}

/// <summary>
/// using for Reset Brush Color
/// </summary>
/// <param name="color">D2D1::ColorF</param>
void D2dData::InitBrushColor(const D2D1::ColorF& color)
{
	Brush->SetColor(color);
}

void D2dData::AfterDraw()
{
	HRESULT hr = S_OK;
#ifdef _WINDOWS_OLD_
	hr = D2D1DrawTarget->EndDraw();
#else
	hr = m_D2DContext->EndDraw();
#endif

	ThrowIfFailed(hr, "END DRAW");
	Show();
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
	m_D2DContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f));
#endif
}


void D2dData::SetTransformOrigin()
{
	m_D2DContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2dData::CameraMove(float offsetx, float offsety)
{
	m_ViewCamera.dx += offsetx;
	m_ViewCamera.dy += offsety;
}

//---------------------Resource Create Function------------------//


void D2dData::CreateGeometry(ID2D1PathGeometry** pathGeometry)
{
	m_D2DFactory->CreatePathGeometry(pathGeometry);
}






// ---------------Draw Function---------------------------------//

void D2dData::DrawRect(float left, float right, float top, float bottom)
{

	const D2D1_RECT_F rc = D2D1::RectF(
		left,
		bottom,
		right,
		top);
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->DrawRectangle(&rc, Brush.Get());
#else
	m_D2DContext->DrawRectangle(&rc, Brush.Get());
#endif
}

// One Object or Draw Object Only can using one Matrix.
/// the Rotate using 
void D2dData::FillRect(float left, float right, float top, float bottom, D2D1_MATRIX_3X2_F matrix)
{
	m_D2DContext->SetTransform(m_ViewCamera * matrix);
	const D2D1_RECT_F rc = D2D1::RectF(
		m_ClientRect.left + left,
		m_ClientRect.top + top,
		m_ClientRect.right - right,
		m_ClientRect.bottom - bottom);
	//m_D2DContext->SetTransform(
		//D2D1::Matrix3x2F::Rotation(45.0f, D2D1::Point2F(640.0f, 360.0f)));
#ifdef _WINDOWS_OLD_
	D2D1DrawTarget->DrawRectangle(&rc, Brush.Get());
#else
	m_D2DContext->FillRectangle(&rc, Brush.Get());
	SetTransformOrigin();
#endif
}

void D2dData::DrawLine(D2D_POINT_2F& fpoint, D2D_POINT_2F& lpoint)
{
	// this is transform matrix.
	m_D2DContext->SetTransform(m_ViewCamera);

	m_D2DContext->DrawLine(fpoint, lpoint, Brush.Get());
	SetTransformOrigin();
}

void D2dData::DrawGeometry(ID2D1Geometry* geo, D2D1_MATRIX_3X2_F matrix)
{
	m_D2DContext->SetTransform(m_ViewCamera * matrix);
	m_D2DContext->DrawGeometry(geo, Brush.Get());
}



void D2dData::DrawWord(const WCHAR *text, const std::string &textformatname, D2D1_RECT_F *drawrect)
{
	//PerpareDraw("Black");
	//D2D1DrawTarget->DrawText(text, ARRAYSIZE(text) - 1, TextFormats[textformatname].Get(), drawrect, Brush);
}
#endif  //_TEST_DRAW_