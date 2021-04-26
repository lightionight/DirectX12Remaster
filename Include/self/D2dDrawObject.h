
#include <box2d/box2d.h>
#include <IDrawObjectInterface.h>

class D2dDrawObject: public IDrawOjbectInterface
{
public:
    D2dDrawObject();
    ~D2dDrawObject();
public:
    void Draw()override;
    void Update()override;
    bool IsDirty()override;
public:
    D2D1_COLOR_F* GetColor() { return &m_DrawColor;}
    D2D1_RECT_F* GetRect() { return &m_DrawArea; }
private:
    D2D1_COLOR_F* m_DrawColor;
    D2D1_RECT_F   m_DrawArea;
    
};

