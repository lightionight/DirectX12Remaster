#pragma once
// box2d Include
#include <box2d/b2_world.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_body.h>

#include <memory>                       // using for unique_ptr
#include <unordered_map>                // using for unorder_map

#include <self/Box2dEngine.h>
#include <self/Box2dObject.h>

#pragma comment(lib, "box2d.lib")



typedef enum SceneLayer
{
    AllOjbect = 0,
	DrawOjbect = 1
} SceneLayer;

/// Box2dSceneManager Focus on Origenaz the box2dEngine Data;

class Box2dSceneManager
{
public:
	Box2dSceneManager();
	~Box2dSceneManager();
public:
	void Initialize(float gravity, float posX, float posY, float halfwidth, float halfHeight);
	void Run();
	void AddBox2dObjects(std::string, b2Vec2[], int, b2BodyType);
	Box2dObject* GetSceneObject(const std::string& name);
private:
	void InitializeEngine(float gravity);
	void InitializeGround(float posX, float posY, float halfwidth, float halfHeight);
    b2World* GetEngine();
private:
	std::unique_ptr<Box2dEngine> m_Box2dEngine;
    std::unordered_map<SceneLayer, std::unique_ptr<Box2dObject>> m_Layer;
    std::vector<Box2dObject> m_AllObject;
};