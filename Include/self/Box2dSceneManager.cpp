#include "Box2dSceneManager.h"
#include <iostream>
void Box2dSceneManager::AddObjectBody(
        const std::string& bodyName,
        b2Vec2& position, 
        b2Vec2& lVelocity, 
        float aVelocity,
        float angle, 
        float gravityScale,
        b2BodyType b2BodyType)
{
    b2BodyDef bDef;
    bDef.type = b2BodyType;
    bDef.position.Set(position.x, position.y);
    bDef.linearVelocity.Set(lVelocity.x, lVelocity.y);
    bDef.angularVelocity = aVelocity;
    bDef.angle = angle;
    bDef.gravityScale = gravityScale;

    b2Body* newBody = mWorld->CreateBody(&bDef);
    mSceneObjectsBody[bodyName] = std::make_unique<b2Body>(&newBody);
}

void Box2dSceneManager::AddOjbectShape(const std::string& shapeName, const b2Vec2* points, int32 count)
{
    b2PolygonShape shape;
    shape.Set(points, count);
    mSceneOjbectsShape[shapeName] = std::make_unique<b2PolygonShape>(shape);
}

void Box2dSceneManager::AddFixture(const std::string& fixtureName, const std::string& bodyName, const std::string& shapeName, float density, float friction)
{
    b2FixtureDef fixtureDef;
    if(mSceneOjbectsShape[shapeName].get() != nullptr)
    {
        fixtureDef.shape = mSceneOjbectsShape[shapeName].get();
        fixtureDef.density = density;
        fixtureDef.friction = friction;
        if(mSceneObjectsBody[bodyName].get() != nullptr)
        {
            mSceneObjectsBody[bodyName]->CreateFixture(&fixtureDef);
            mSceneOjbectsFixture[fixtureName] = std::make_unique<b2FixtureDef>(fixtureDef);
        }
        else
        {
            std::cout << "Error for Create fixture, Can't find body" << std::endl;

        }
    }
    else
    {
        std::cout << "Error for create fixture, can't find shape" << std::endl; 
    }

}

void Box2dSceneManager::SetFixtureToOjbect(const std::string& OjbectsName, const std::string& fixtureName)
{
    b2FixtureDef& fixture = *(mSceneOjbectsFixture[fixtureName].get());
    b2Body& object = *(mSceneObjectsBody[OjbectsName].get());
    object.CreateFixture(&fixture);
}


void Box2dSceneManager::CreateGround(float halfwidth, float halfHeight)
{
    b2Vec2 groundPosition(0.0f, -10.0f);
    b2Vec2 lVelocity(0.0f, 0.0f);
    AddObjectBody("Ground", groundPosition, lVelocity, 0.0f, 0.0f, 1.0f);
    
    b2PolygonShape groundBox;
    groundBox.SetAsBox(halfwidth, halfHeight);
    mSceneOjbectsShape["Ground"] = std::make_unique<b2PolygonShape>(groundBox);
    mSceneObjectsBody["Ground"]->CreateFixture(mSceneOjbectsShape["Ground"].get(), 0.0f); // This using for Create static Objects;

}

const b2World* Box2dSceneManager::GetEngine()
{
    return mWorld.get();
}