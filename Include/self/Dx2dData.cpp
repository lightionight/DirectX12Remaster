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

void D2dData::Initialize(HWND hwnd, D2D1_FACTORY_TYPE factoryTypes)
{
	HRESULT hr = NULL;
	{
#if defined(DEBUG) | defined(_DEBUG)
		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		hr = D2D1CreateFactory(factoryTypes, options, IID_PPV_ARGS(&D2D1Factory));
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
	}
	AddColor("White", 1.0f, 1.0f, 1.0f, 1.0f);
	AddColor("Black", 0.0f, 0.0f, 0.0f, 1.0f);

}

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
		TextFormats[formatName].Get()
	);
	if (FAILED(hr))
		std::cout << "Create AddTextFormat is failed" << std::endl;

}

void D2dData::PerpareDraw(const std::string& colorName)
{
	D2D1DrawTarget->BeginDraw();
	InitBrushColor(colorName);
}

void D2dData::AfterDraw()
{
	HRESULT hr = D2D1DrawTarget->EndDraw();
	if (FAILED(hr))
		std::cout << "END Draw is Error" << std::endl;
	else
		std::cout << "Draw Done!" << std::endl;
}

void D2dData::ClearTarget()
{
	D2D1DrawTarget->Clear(Colors["White"].get());
}

void D2dData::Resize()
{
	D2D1DrawTarget->CheckWindowState();
}

void D2dData::DrawRect(float left, float right, float top, float bottom)
{
	const D2D1_RECT_F rc = D2D1::RectF(
		RenderRect.left + left,
		RenderRect.top + top,
		RenderRect.right - right,
		RenderRect.bottom - bottom
	);

	D2D1DrawTarget->DrawRectangle(&rc, Brush.Get());
}

void D2dData::DrawWord(const WCHAR* text, const std::string& textformatname, D2D1_RECT_F* drawrect)
{
	PerpareDraw("Black");
	//D2D1DrawTarget->DrawText(text, ARRAYSIZE(text) - 1, TextFormats[textformatname].Get(), drawrect, Brush);
}
