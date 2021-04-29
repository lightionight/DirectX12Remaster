#pragma once

#include <d2d1helper.h>
#include <d2d1.h>
using namespace D2D1;

struct D2dDrawData;

// Graphics Draw object interface.
class IDrawOjbectInterface
{
public:
	virtual void Draw();
	virtual void Update();
	virtual bool IsDirty();
};

typedef struct D2dDrawData
{
	

} D2dDrawData;