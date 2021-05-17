#pragma once

#include "IPhysicObjectInterface.h"
#include "../box2d/box2d.h"
#include "../box2d/b2_world.h"
#include "../box2d/b2_body.h"
#include "../box2d/b2_polygon_shape.h"
#include <memory>
#include <string>

class Box2dObject : public IPhysicObjectInterface
{
public:
    /// Add Objects by give the array of points and count of points , count must < maxpointsNumber.
    // @Param points 
    Box2dObject();
    Box2dObject(const std::string&, const b2Vec2[], int, b2World*, b2BodyType);
    Box2dObject(const std::string&);
    ~Box2dObject();
public:
    void Update()override;
public:
    void InitAsGround(float, float, float, float, b2World*);
   
public:
    const b2Vec2& GetCurrentPosition() { return m_Body->GetPosition();}
    const float& GetAngle() { return m_Body->GetAngle();}
    // this function only return array first element adress.
    b2Vec2* GetAllShapeVertexPosition() {return m_Shape->m_vertices;}
    int32 GetVertexCount(){ return m_Shape->m_count; }
    std::string GetName(){return m_Name;}
    void SetName(std::string name) { m_Name = name;}
    b2Body* GetBody() { return m_Body;}
    b2PolygonShape* GetShape(){ return m_Shape;}

private:
    std::string m_Name;
    b2Body* m_Body;
    b2PolygonShape*  m_Shape; // this parameter may not be only one.

    

};