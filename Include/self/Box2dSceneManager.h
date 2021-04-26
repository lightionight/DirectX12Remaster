#pragma once
// box2d Include
#include <box2d/b2_world.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_body.h>

#include <memory>                       // using for unique_ptr
#include <unordered_map>                // using for unorder_map


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

    ///Add b2ShapeBody to Scene
	/// @param world Current box2d engine world. after you create Box2dEngine ojbect, it should be set up and initialize with the gravity.
	/// @param bodyName name of b2BodyOjbect for using.
	/// @param position Where the b2body should be stay.
	/// @param lVelocity motion force of linear movement.
	/// @param aVelocity Angle froce of Angle rotate.
	/// @param angle This Should be set body init rotate angle by radian unit.
	/// @param gravityScale This is can scale the world gravity by multiphy it. 1 is as same as the world gravity settings
	/// @param bodyType This Deafult settting is static type
    void AddObjectBody(const std::string& bodyName, b2Vec2& position, b2Vec2& lVelocity, float aVelocity,float angle = 0.0f, float gravityScale = 1.0f, b2BodyType bodyType = b2BodyType::b2_staticBody);

    /// Add b2PolygonShape to Scene.
	/// @param shapeName  using for call.
	/// @param points  pointer of points array.
	/// @param count   count of points number.
	void AddOjbectShape(const std::string& shapeName, const b2Vec2* points, int32 count);

	void AddFixture(const std::string& fixtureName, const std::string& bodyName, const std::string& shapeName, float density, float friction);

    /// using for object Add fixture quick setting by give the name of object and fixtureName.
	/// must detecting the name of thoese exist or not.
	/// @param objectName the Name of element of mSceneObjects
	/// @param fixtureName the Name of elemenet of mSceneObjectsFixture
	void SetFixtureToOjbect(const std::string& ojbectName, const std::string& fixtureName);
public:
    const b2World* GetEngine();

	void CreateGround(float halfwidth, float halfheight);
private:
    std::unique_ptr<b2World> mWorld;
    std::unordered_map<SceneLayer::DrawOjbect, std::unique_ptr<>>

};