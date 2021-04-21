#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H


#include <gainput\gainput.h>
#include <memory>
#if defined _DEBUG | DEBUG
    #pragma comment(lib, "gainput\\x64\\Debug\\gainput-d.lib")
#else
    #pragma comment(lib, "gainput\\x64\\Release\\gainput.lib")
#endif




enum Button
{
	MouseL,
	MouseR,
	MouseX,
	MouseY
};
enum MapType
{
	B,
	F,
};

struct InputHandle
{
	std::unique_ptr<gainput::InputManager> Manager; // Manager Need In update loop for lister
	std::unique_ptr<gainput::InputMap> Map;
	gainput::DeviceId KeyBoardId = NULL;
	gainput::DeviceId MouseId = NULL;
	void Initialize(int windowWidth, int windowHeight, bool defaultSettings)
	{
		Manager = std::make_unique<gainput::InputManager>();
		Manager->SetDisplaySize(windowWidth, windowHeight);
		Map = std::make_unique<gainput::InputMap>(*Manager.get());
		KeyBoardId = Manager->CreateDevice<gainput::InputDeviceKeyboard>();
		MouseId = Manager->CreateDevice<gainput::InputDeviceMouse>();
		if (defaultSettings)
			BasicMap();
	}
	void AddMap(MapType mapType, Button button, gainput::DeviceId deviceid, gainput::MouseButton deviceButton)
	{
		if (mapType == MapType::B)
			Map->MapBool(button, deviceid, deviceButton);
		else
			Map->MapFloat(button, deviceid, deviceButton);
	}
private:
	void BasicMap()
	{
		AddMap(MapType::B, MouseL, MouseId, gainput::MouseButtonLeft);
		AddMap(MapType::B, MouseR, MouseId, gainput::MouseButtonRight);
		AddMap(MapType::F, MouseX, MouseId, gainput::MouseAxisX);
		AddMap(MapType::F, MouseY, MouseId, gainput::MouseAxisY);
	}
};

#endif // !INPUT_HANDLE_H

