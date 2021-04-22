// This is Base Helper For Direct2D Draw Graphics, Need Update And ReConsider function

#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>      // for DirectWrite draw text
#include <wrl.h>         // using for ComPtr
#include <unordered_map> // using for store Brush

#include <memory>  // using for smartPointer


#pragma comment(lib, "d2d1.lib")

using namespace D2D1;
using namespace Microsoft::WRL;

typedef struct D2dData {
public:
	ComPtr<ID2D1Factory> D2D1Factory = nullptr;
	ComPtr<ID2D1HwndRenderTarget> D2D1DrawTarget = nullptr;

	ComPtr<IDWriteFactory> DWriteFactory = nullptr;
	ComPtr<IDWriteTextLayout> DWriteTextLayout = nullptr;
	// Brush using for Draw 
private:
	ComPtr<ID2D1SolidColorBrush> Brush = nullptr;  // Brush Default Init Color is White. Please Careful!!!! ;
	RECT RenderRect;

	// Color using for brush add
	std::unordered_map<std::string, std::unique_ptr<D2D1_COLOR_F>> Colors; // Default add White and Black Color;
	std::unordered_map<std::string, ComPtr<IDWriteTextFormat*>> TextFormats;
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
	void DrawRect(float left, float right, float top, float bottom);

	void DrawWord(const WCHAR* text, const std::string& textformatname, D2D1_RECT_F* drawrect);
	
public:
	void AddColor(const std::string& colorName, float r, float g, float b, float a);
	ComPtr<D2D1_COLOR_F> UsingColor(const std::string& colorName);

	void AddTextFormat(const std::string& formatName, const WCHAR* fontName, float fontSize);




}D2dData;