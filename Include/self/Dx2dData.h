// This is Base Helper For Direct2D Draw Graphics, Need Update And ReConsider function

#pragma once


// There need to Care fully windows version  win8 above and win8 later

/// DXGI AND D3D12 Include
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>

// D2D Include
#include <d2d1_1helper.h>
#include <D2DErr.h>

#include <d2d1effects_1.h>
#include <d2d1effects_2.h>
#include <d2d1effecthelpers.h>
//DDwrite
#include <dwrite.h>      // for DirectWrite draw text

#include <wrl.h>         // using for ComPtr
#include <unordered_map> // using for store Brush

#include <memory>  // using for smartPointer

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace D2D1;
using namespace Microsoft::WRL;

typedef struct D2dData {
public:
	D2dData();
	~D2dData();
public:
	void Initialize(HWND hwnd, D2D1_FACTORY_TYPE factoryType);
	void Resize();
	void InitBrushColor(const std::string& colorName);
	void PerpareDraw(const std::string& colorName);
	void AfterDraw();
	void ClearTarget();
	void Show();
	void DrawRect(float left, float right, float top, float bottom);
	void DrawWord(const WCHAR* text, const std::string& textformatname, D2D1_RECT_F* drawrect);
	
public:
	void AddColor(const std::string& colorName, float r, float g, float b, float a);
	ComPtr<D2D1_COLOR_F> UsingColor(const std::string& colorName);

	void AddTextFormat(const std::string& formatName, const WCHAR* fontName, float fontSize);
private:

public:
    // D2D
	ComPtr<ID2D1Factory1>         D2D1Factory = nullptr;
	ComPtr<ID2D1Device>           D2D1Device = nullptr;
	ComPtr<ID2D1DeviceContext>    D2D1Context = nullptr;
	ComPtr<IDXGISwapChain1>       D2D1SwapChain = nullptr;
	ComPtr<IDXGISurface2>         D2D1BackBuffer = nullptr;
	ComPtr<ID2D1Bitmap1>          D2D1RenderTargetMap = nullptr;
	ComPtr<ID2D1HwndRenderTarget> D2D1DrawTarget = nullptr; // using for tranditional Way

    // For DDwrite
	ComPtr<IDWriteFactory>        DWriteFactory = nullptr;
	ComPtr<IDWriteTextLayout>     DWriteTextLayout = nullptr;
private:
	// Brush using for Draw 
	ComPtr<ID2D1SolidColorBrush>  Brush = nullptr;  // Brush Default Init Color is White. Please Careful!!!! ;

	ComPtr<IDXGIFactory2>         DXGIFactory = nullptr;
	ComPtr<IDXGIAdapter>          DXGIAdapter = nullptr;
	ComPtr<IDXGIDevice1>          DXGIDevice = nullptr;

	ComPtr<ID3D11Device>          D3D11Device = nullptr;
	ComPtr<ID3D11DeviceContext>   D3D11Context = nullptr;
	ComPtr<ID3D11Texture2D>       D3DBackBuffer = nullptr;

	D3D_FEATURE_LEVEL* SupportFeatureLevel = nullptr; 

	RECT RenderRect;

	// Color using for brush add
	std::unordered_map<std::string, std::unique_ptr<D2D1_COLOR_F>> Colors; // Default add White and Black Color;
	std::unordered_map<std::string, ComPtr<IDWriteTextFormat>> TextFormats;




}D2dData;