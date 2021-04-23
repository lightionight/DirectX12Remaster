#pragma once
/// box2d Head Include
#include <box2d/b2_common.h>
#include <box2d/b2_world.h>
#include <box2d/b2_math.h>  /// using for box2d data type
#include <box2d/b2_body.h>

#include "Box2dSceneManager.h"
#include <memory>  /// using for std::unique_ptr

#pragma comment(lib, "box2d.lib")


/// This Box2dEngine Class Focus on box2d Setings

class Box2dEngine
{
public:
	Box2dEngine();
	~Box2dEngine();
public:
	void Initialize(b2Vec2& gravity);
	void Start();
	void Update();
	void Calculate();
	void Refresh();
private:
	std::unique_ptr<Box2dSceneManager> mSceneManager;
	float mtimeStep = 1.0f / 60.0f; 
	int32 mVelocityIteration = 6;
	int32 mVelocityIteration = 2;

};

