#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H


#include <gainput\gainput.h>
#include <memory>
#pragma comment(lib, "gainput\\gainput-d.lib")

enum Button
{
	MOUSE_L,
	MOUSE_R,
	MOUSE_MOVE
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
		AddMap(MapType::B, MOUSE_L, MouseId, gainput::MouseButtonLeft);
		AddMap(MapType::B, MOUSE_R, MouseId, gainput::MouseButtonRight);
		//AddMap(MapType::FLOAT, MOUSE_MOVE, MouseID, gainput::MouseAxisCount)
	}
};

#endif // !INPUT_HANDLE_H

