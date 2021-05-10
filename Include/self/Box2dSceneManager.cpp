#include "Box2dSceneManager.h"
#include <iostream>


Box2dSceneManager::Box2dSceneManager()
{
    
}
Box2dSceneManager::~Box2dSceneManager()
{

}

void Box2dSceneManager::Initialize(float gravity, float posX, float posY, float halfwidth, float halfHeight)
{
    InitializeEngine(gravity);
    InitializeGround(posX, posY, halfwidth, halfHeight);

}

void Box2dSceneManager::InitializeEngine(float gravity)
{
    m_Box2dEngine = std::make_unique<Box2dEngine>(gravity);
}

void Box2dSceneManager::InitializeGround(float posX, float posY, float halfwidth, float halfHeight)
{
    Box2dObject objects;
    objects.InitAsGround(posX, posY, halfwidth, halfHeight, GetEngine());
    m_AllObject.push_back(objects);
}

b2World* Box2dSceneManager::GetEngine()
{
    return m_Box2dEngine->GetEngine();
}

Box2dObject* Box2dSceneManager::GetSceneObject(const std::string& name)
{
    for (Box2dObject obj : m_AllObject)
    {
        /* code */
        if(obj.GetName() == name)
            return &obj;
    }
    
}

void Box2dSceneManager::AddBox2dObjects(std::string name,b2Vec2 points[], int counts, b2BodyType bodytype)
{
    Box2dObject obj(name, points, counts, GetEngine(), bodytype);
    m_AllObject.push_back(obj);
}


void Box2dSceneManager::Run()
{
    m_Box2dEngine->Start();
}


