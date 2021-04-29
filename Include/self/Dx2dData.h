// This is Base Helper For Direct2D Draw Graphics, Need Update And ReConsider function

#pragma once

//_________TE�ӣԣߣߣߣߣߣߣͣ��ңãϣӣߣߣߣߣߣߣ�//
#define _TEST_DRAW_

// Need to Detect using Direct2d Old way or NEw Way by using #define _D2D_WITH_D3D_
#define _D2D_WITH_D3D_


// There need to Care fully windows version  win8 above and win8 later

/// DXGI AND D3D12 Include
#include <d3d11_3.h>

// D2D Include
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <d2d1effects_2.h>
#include <d2d1effectauthor_1.h>
#include <d2d1effecthelpers.h>

#include <D2DErr.h>
//DDwrite
#include <dwrite_2.h>      // for DirectWrite draw text

#include <wincodec.h>    // using for WICImageFactory
#include <wrl.h>         // using for ComPtr

#include <unordered_map> // using for store Brush
#include <memory>  // using for smartPointer

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace D2D1;
using namespace Microsoft::WRL;



class D2dData {
public:
	D2dData();
	D2dData(HWND hwnd);
	~D2dData();
	
public:
    void ReSize(HWND hwnd, float dpi);
public:
#ifdef _TEST_DRAW_
	void InitBrushColor(const D2D1::ColorF& color);
	void PerpareDraw(const D2D1::ColorF&);
	void AfterDraw();
	void ClearTarget();
	void Show();
	void FillRect(float left, float right, float top, float bottom);
	void DrawRect(float left, float right, float top, float bottom);
	void DrawLine(D2D_POINT_2F& fpoint, D2D_POINT_2F& lpoint, D2D1::Matrix3x2F& matrix = D2D1::Matrix3x2F::Identity());
	void DrawWord(const WCHAR* text, const std::string& textformatname, D2D1_RECT_F* drawrect);
#endif // _TEST_DRAW_
private:
	void InitializeBase();
	void InitializeD2D(HWND hwnd);
	void InitializeD2DWithD3D();
	void InitWindowOrResize();
	void SetTransformOrigin();
	
public:

private:
	// D2D
	ComPtr<ID2D1Factory3>         m_D2DFactory;
	ComPtr<ID2D1Device2>           m_D2DDevice;
	ComPtr<ID2D1DeviceContext2>    m_D2DContext;
	ComPtr<ID2D1Bitmap1>          m_D2DRenderTargetMap;

	ComPtr<ID2D1HwndRenderTarget> m_D2D1DrawTarget; // using for tranditional Way

	// For DDwrite
	ComPtr<IDWriteFactory2>        m_DWriteFactory;
	ComPtr<IWICImagingFactory>     m_WicFactory;

	// Brush using for Draw 
	ComPtr<ID2D1SolidColorBrush>  Brush;  // Brush Default Init Color is White. Please Careful!!!! ;

	// D3D Object
	ComPtr<ID3D11Device3>          m_D3DDevice;
	ComPtr<IDXGISwapChain1>        m_SwapChain;
	ComPtr<ID3D11DeviceContext4>   m_D3DContext;

	//D3D Render Object
	ComPtr<ID3D11RenderTargetView> m_D3DRenderTargetView;
	ComPtr<ID3D11Texture2D>        m_D3DBackBuffer;
	D3D11_VIEWPORT                 m_ScreenViewport;

	HWND m_HWND; // using for Windows WIn32

	// Custom Settings Parameters;
	D3D_FEATURE_LEVEL m_SupportFeatureLevel;
	UINT              m_Dpi;
	D2D1_SIZE_U       m_D3DRenderTargetSize;
	D2D1_SIZE_U       m_OutputSize;
	D2D1_SIZE_U       m_logicalSize;
	RECT              m_ClientRect;  // using for Windows Display and Draw Area Coodinate.
	RECT              m_WindowRect;  // using for Diplay Screen area Coordinate
};

