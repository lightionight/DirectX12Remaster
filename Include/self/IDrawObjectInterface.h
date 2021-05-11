#pragma once

#include <d2d1helper.h>
#include <d2d1.h>
#include <self\Dx2dData.h>
#include <self\Box2dObject.h>
using namespace D2D1;

struct D2dDrawData;

// Graphics Draw object interface.
class IDrawOjbectInterface
{
public:
	virtual void Draw(D2dData*) = 0;
	virtual void Update(const b2Vec2&) = 0;
	virtual bool IsDirty() = 0;
};

typedef struct D2dDrawData
{
	

} D2dDrawData;