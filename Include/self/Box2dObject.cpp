#include "Box2dObject.h"

Box2dObject::Box2dObject()
{

}

Box2dObject::Box2dObject(const std::string& name,const b2Vec2 points[], int counts, b2World* b2World, b2BodyType bodyType)
{
    m_Name = name; 
   b2BodyDef def;
   def.position.Set(0.0f, 1.0f);
   def.angle = 0;
   def.type = bodyType;
   m_Body = b2World->CreateBody(&def);
   
   b2PolygonShape shape;
   shape.m_count = counts;
   for(int i = 0; i < counts; ++i)
   {
       shape.m_vertices[i].Set(points[i].x, points[i].y);
   }
   m_Shape = &shape;

   b2FixtureDef fixDef;
   fixDef.shape = m_Shape;
   fixDef.density = 0.3f;
   fixDef.friction = 0.1f;
   m_Body->CreateFixture(&fixDef);

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