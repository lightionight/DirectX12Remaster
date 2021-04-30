
#include "IPhysicObjectInterface.h"
#include "../box2d/box2d.h"
#include "../box2d/b2_world.h"
#include "../box2d/b2_body.h"
#include "../box2d/b2_polygon_shape.h"
#include <memory>
#include <string>

class Box2dObjects : public IPhysicObjectInterface
{
public:
    /// Add Objects by give the array of points and count of points , count must < maxpointsNumber.
    // @Param points 
    Box2dObjects(const std::string& Name, const b2Vec2* points, int counts, b2World&  world, b2BodyType bodytype);
    ~Box2dObjects();
public:
    void Update()override;
    const std::string& GetName() {return m_Name;}
    const b2Vec2& GetCurrentPosition() { return m_Body->GetPosition();}
    const float& GetAngle() { return m_Body->GetAngle();}
    b2Vec2* GetAllShapeVertexPosition() {return m_Shape->m_vertices;}
private:
    std::string m_Name;
    b2Body* m_Body;
    b2PolygonShape*  m_Shape; // this parameter may not be only one.

    

};