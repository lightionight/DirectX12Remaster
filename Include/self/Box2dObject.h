#include "IPhysicObjectInterface.h"
#include "../box2d/box2d.h"
#include "../box2d/b2_body.h"
#include <memory>
#include <string>

class Box2dObjects : public IPhysicObjectInterface
{
public:
    Box2dObjects();
    ~Box2dObjects();
public:
    void Update()override;
    const std::string& GetName() {return m_Name;}
private:
    b2Body m_Body;
    std::unique_ptr<b2Shape>  m_Shape; // this parameter may not be only one.
    std::string m_Name;
    

};