#include "Box2dSceneManager.h"
#if defined (DEBUG) | defined (_DEBUG)
    #include <iostream>
#endif // d (DEBUG) | defined (_DEBUG)

#include <algorithm>


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
    m_AllObject.push_back(std::move(objects));
}

b2World* Box2dSceneManager::GetEngine()
{
    return m_Box2dEngine->GetEngine();
}

Box2dObject* Box2dSceneManager::GetSceneObject(const std::string& name)
{
    Box2dObject* result = nullptr;
    for (Box2dObject obj : m_AllObject)
        if (obj.GetName() == name)
            result = &obj;

    return result;   
}

void Box2dSceneManager::AddBox2dObjects(std::string name,b2Vec2 points[], int counts, b2BodyType bodytype)
{
    Box2dObject obj(name, points, counts, GetEngine(), bodytype);
#if defined(DEBUG) | defined(_DEBUG)
    for (int i = 0; i < counts; i++)
        std::cout << "Points : " << points[i].x << "," << points[i].y << std::endl;
#endif

    m_AllObject.push_back(std::move(obj));
}


void Box2dSceneManager::Run()
{
    m_Box2dEngine->Start();
}


