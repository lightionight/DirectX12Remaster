#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H


#include <gainput\gainput.h>
#include <memory>
#pragma comment(lib, "gainInput\gainput-d.lib")

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
	std::unique_ptr<gainput::InputDevice> Device = nullptr;
	std::unique_ptr<gainput::InputManager> Manager = nullptr; // Manager Need In update loop for lister
	std::unique_ptr<gainput::InputMap> Map = nullptr;
	gainput::DeviceId KeyBoardId = NULL;
	gainput::DeviceId MouseID = NULL;
	void Initialize(int windowWidth, int windowHeight, bool defaultSettings)
	{
		Device = std::make_unique<gainput::InputDevice>()
		Manager = std::make_unique<gainput::InputManager>();
		Manager->SetDisplaySize(windowWidth, windowHeight);
		Map = std::make_unique<gainput::InputMap>(*Manager.get());
		MouseID = Manager->CreateDevice<gainput::InputDevice>();
		KeyBoardId = Manager->CreateDevice<gainput::InputDevice>();
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
		AddMap(MapType::B, MOUSE_L, MouseID, gainput::MouseButtonLeft);
		AddMap(MapType::B, MOUSE_R, MouseID, gainput::MouseButtonRight);
		//AddMap(MapType::FLOAT, MOUSE_MOVE, MouseID, gainput::MouseAxisCount)
	}
};

#endif // !INPUT_HANDLE_H

