#include "Box2dEngine.h"

#if defined(DEBUG) || defined (_DEBUG)
    #include <iostream> // Output debug information
#endif // DEBUG  _DEBUG


Box2dEngine::Box2dEngine()
{

}

Box2dEngine::Box2dEngine(float gravity)
{
    m_Gravity = {0.0f, gravity};
    m_World = std::move(std::make_unique<b2World>(m_Gravity));
    // Game Engine World Create
}

Box2dEngine::~Box2dEngine()
{

}

void Box2dEngine::Initialize()
{
    // Create Ground
    b2BodyDef groundDef;
    groundDef.position.Set(0.0f, -10.0f);
    b2Body* groundBody = m_World->CreateBody(&groundDef);

    // Ground polygon Shape
    b2PolygonShape groundbox;
    groundbox.SetAsBox(50.0f, 10.0f);
    
    //set Ground static
    groundBody->CreateFixture(&groundbox, 0.0f);
    
    b2BodyDef boxdef;
    boxdef.type = b2_dynamicBody;
    boxdef.position.Set(0.0f, 4.0f);
    
    // Create box body
    b2Body* boxBody = m_World->CreateBody(&boxdef);

    // Create box Shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox(1.0f, 1.0f);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1.0f;
    boxFixtureDef.friction = 0.3f;

    boxBody->CreateFixture(&boxFixtureDef);

    for(int32 i = 0; i < 60; ++i)
    {
        m_World->Step(m_timeStep, m_VelocityIteration, m_PositionIteration);
        b2Vec2 b2Position = boxBody->GetPosition();
        float angle = boxBody->GetAngle();
#if defined (_DEBUG) || defined (DEBUG)
        std::cout << "POSITION : " << b2Position.x << " " << b2Position.y << "ANGLE: " << angle << std::endl;
#endif
    }
 
}

void Box2dEngine::Start()
{
    if(!m_IsStart)
        m_IsStart = true;
        m_IsPause = false;
        m_IsStop  = false;
    m_World->Step(m_timeStep, m_VelocityIteration, m_PositionIteration);
#if defined (DEBUG) | defined (_DEBUG)
    b2Body* test = m_World->GetBodyList();
    if(test != nullptr)
        std::cout << "Position is :" << test->GetPosition().x << " " << test->GetPosition().y << std::endl;
#endif
}

void Box2dEngine::Update()
{
    

}

void Box2dEngine::Calculate()
{

}

void Box2dEngine::Refresh()
{

}

b2World* Box2dEngine::GetEngine()
{
    return m_World.get();
}