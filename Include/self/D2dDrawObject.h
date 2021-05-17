
#pragma once

#include <box2d/box2d.h>
#include <self/IDrawObjectInterface.h>
#include <d2d1helper.h>
#include <vector>
#include <self/Box2dObject.h>

#include <d2d1_3helper.h>

class D2dDrawObject: public IDrawOjbectInterface
{
public:
    enum D2dDrawObjectType{
        Point     = 0,
        Line      = 1,
        Rectangle = 2,
        Geometry  = 3,
        Bitmap    = 4
    };
public:
    D2dDrawObject(D2dData* d2dData, D2dDrawObjectType type);
    ~D2dDrawObject();
public:
    void Draw(D2dData* d2dData)override;
    void Update(const b2Vec2& position)override;
    bool IsDirty()override;

    void Add(Box2dObject* box2dObject);
    void Add(D2D1_POINT_2F pointList[], int countPoints);  // Only Using For Test;
    /// <summary>
    /// 
    /// </summary>
    /// <param name="b2Vec"> Array Pointer</param>
    /// <param name="int">Count of array element</param>
    /// <returns></returns>
    void Convertb2Vec2ToPoint2F(b2Vec2*, int);

private:
    void PrepareAdd();
    void AfterAdd();

private:
    Microsoft::WRL::ComPtr<ID2D1PathGeometry>      m_Geo;
    Microsoft::WRL::ComPtr<ID2D1GeometrySink>      m_GeoSink;
    D2dDrawObjectType   m_DrawType;

    D2D1_COLOR_F*       m_DrawColor;
    D2D1_RECT_F         m_DrawArea;
    D2D1_POINT_2F       m_PosW;
    D2D1_MATRIX_3X2_F   m_Translate;
    D2D1_MATRIX_3X2_F   m_rotate;
    D2D1_MATRIX_3X2_F   m_scale;

    std::vector<D2D1_POINT_2F> m_PointList;
    
    
};

