#pragma once

#include <box2d/b2_common.h>

#pragma comment(lib, "box2d.lib")

class Box2dEngine
{
public:
	Box2dEngine();
	~Box2dEngine();
public:
	void Initialize();
	void Start();
	void Update();
	void Calculate();
	void Refresh();
	void Clean();


};

