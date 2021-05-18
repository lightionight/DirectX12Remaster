#include "Box2dObject.h"

Box2dObject::Box2dObject()
{

}
Box2dObject::Box2dObject(const std::string& name,const b2Vec2 points[], int counts, b2World* b2World, b2BodyType bodyType)
{
   m_Name = name; 
   b2BodyDef def;
   def.position.Set(500.0f, 500.0f);
   def.angle = 0;
   def.type = bodyType;
   m_Body = b2World->CreateBody(&def);
   
   b2PolygonShape shape;
   shape.Set(points, counts);

   b2FixtureDef fixDef;
   fixDef.shape = &shape;
   fixDef.density = 0.3f;
   fixDef.friction = 0.1f;
   m_Body->CreateFixture(&fixDef);

   m_Shape = (b2PolygonShape*)m_Body->GetFixtureList()[0].GetShape();
   

}
Box2dObject::Box2dObject(const std::string& name)
{
    m_Name = name;
}
Box2dObject::~Box2dObject()
{
    //if(m_Body != nullptr)

}
void Box2dObject::InitAsGround(float posX, float posY, float halfwidth, float halfHeight, b2World* world)
{
    m_Name = "Ground";
    b2BodyDef bDef;
    bDef.position.Set(posX, posY);

    m_Body = world->CreateBody(&bDef);

    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(halfwidth, halfHeight);

    m_Body->CreateFixture(&bodyShape, 0.0f);
}
void Box2dObject::Update()
{
    
}
