#include "Box2dObject.h"

Box2dObjects::Box2dObjects(const std::string& name,const b2Vec2* points, int counts, b2World& b2World, b2BodyType bodyType)
{
    m_Name = name; 
   b2BodyDef def;
   def.position.Set(0.0f, 1.0f);
   def.angle = 0;
   def.type = bodyType;
   m_Body = b2World.CreateBody(&def);
   
   b2PolygonShape shape = b2PolygonShape();
   for(int i = 0; i < counts; ++i)
   {
       shape.m_vertices[i].Set(points[i].x, points[i].y);
   }
   m_Shape = std::move(&shape);

   b2FixtureDef fixDef;
   fixDef.shape = m_Shape;
   fixDef.density = 0.3f;
   fixDef.friction = 0.1f;
   m_Body->CreateFixture(&fixDef);

}

Box2dObjects::~Box2dObjects()
{
    if(m_Body != nullptr)

}

void Box2dObjects::Update()
{
    
}