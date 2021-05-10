#pragma once
/// box2d Head Include
#include <box2d/b2_common.h>
#include <box2d/b2_world.h>
#include <box2d/b2_math.h>  /// using for box2d data type
#include <box2d/b2_body.h>
#include <box2d\b2_shape.h>
#include <box2d\b2_polygon_shape.h>
#include <box2d\b2_fixture.h>

//#include "Box2dSceneManager.h"
#include <memory>  /// using for std::unique_ptr

#pragma comment(lib, "box2d.lib")


/// This Box2dEngine Class Focus on box2d Setings

class Box2dEngine
{
public:
	Box2dEngine(float gravity);
	Box2dEngine();
	~Box2dEngine();
public:
	void Initialize();
	void Start();
	void Update();
	void Calculate();
	void Refresh();
public:
    b2World* GetEngine();
private:
	//std::unique_ptr<Box2dSceneManager> mSceneManager;

	std::unique_ptr<b2World> m_World;
	b2Vec2  m_Gravity;
	float   m_timeStep = 1.0f / 60.0f; 
	int32   m_VelocityIteration = 6;
	int32   m_PositionIteration = 2;
    // Engine Status
	bool    m_IsStart;
	bool    m_IsStop;
	bool    m_IsPause;


};

