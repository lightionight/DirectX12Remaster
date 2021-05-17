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

// this function return reference may have some problem
Box2dObject* Box2dSceneManager::GetSceneObject(const std::string& name)
{
    for (size_t i = 0; i < m_AllObject.size(); i++)
        if (m_AllObject[i].GetName() == name)
            return &(m_AllObject[i]);
    return nullptr;
}

void Box2dSceneManager::AddBox2dObjects(std::string name,b2Vec2 points[], int counts, b2BodyType bodytype)
{
    Box2dObject obj(name, points, counts, GetEngine(), bodytype);
#if defined(DEBUG) | defined(_DEBUG)
    for (int i = 0; i < counts; i++)
        std::cout << "Points : " << i << ":" << points[i].x << "," << points[i].y << std::endl;
#endif
    m_AllObject.push_back(std::move(obj));
#if defined(DEBUG) | defined(_DEBUG)
    Box2dObject test = m_AllObject.back();
    b2PolygonShape* sp = test.GetShape();
    for (int i = 0; i < 8; i++)
        std::cout << "After Add: "<< i << ":" << sp->m_vertices[i].x << ", " << sp->m_vertices[i].y << std::endl;
#endif 
                  
}


void Box2dSceneManager::Run()
{
    m_Box2dEngine->Start();
}


