#include <self\D2dDrawObject.h>
#include <self/Dx2dData.h>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


// There need to make this class as templete

D2dDrawObject::D2dDrawObject(D2dData* d2dData, D2dDrawObjectType type, float offset)
{
    m_DrawType = type;
    m_PosW = D2D1::Point2(0.0f, 0.0f);
    m_Translate = D2D1::IdentityMatrix();
    m_rotate = D2D1::IdentityMatrix();
    m_scale = D2D1::IdentityMatrix();
    m_x_offset = offset;
    d2dData->CreateGeometry(&m_Geo);
        
    
}



D2dDrawObject::~D2dDrawObject()
{

}

void D2dDrawObject::Draw(D2dData* d2dData)
{
    d2dData->DrawGeometry(m_Geo.Get(), m_Translate);
}

void D2dDrawObject::Update(const b2Vec2& position)
{
    m_Translate._11 = 1.0f;
    m_Translate._21 = 0.0f;
    m_Translate._12 = 0.0f;
    m_Translate._22 = 1.0f;
    m_Translate._31 = position.x + m_x_offset;
    m_Translate._32 = -position.y;
}

bool D2dDrawObject::IsDirty()
{
    return true;
}

void D2dDrawObject::Convertb2Vec2ToPoint2F(b2Vec2* vec, int count)
{ 
    for(int i = 0; i < count; ++i)
    {
        D2D1_POINT_2F point = D2D1::Point2(vec[i].x, vec[i].y);
        m_PointList.push_back(std::move(point));
    }
}

void D2dDrawObject::Add(Box2dObject* box2dObjects)
{
    PrepareAdd();
    b2Vec2* b2vecList = box2dObjects->GetAllShapeVertexPosition();
    int count = box2dObjects->GetVertexCount();
    Convertb2Vec2ToPoint2F(b2vecList, count);

    size_t CountPoints = m_PointList.size();
    m_GeoSink->SetFillMode(D2D1_FILL_MODE_WINDING);
    m_GeoSink->BeginFigure(m_PointList[0],D2D1_FIGURE_BEGIN_FILLED);
    for(size_t i = 1; i < CountPoints; ++i)
    {
       m_GeoSink->AddLine(m_PointList[i]);
    }

    AfterAdd();
}

void D2dDrawObject::Add(D2D1_POINT_2F pointList[], int countPoints)
{
     PrepareAdd();
    m_GeoSink->BeginFigure(pointList[0],D2D1_FIGURE_BEGIN_FILLED);
    for(size_t i = 1; i < countPoints; ++i)
    {
       m_GeoSink->AddLine(pointList[i]);
    }
    AfterAdd();
}

void D2dDrawObject::PrepareAdd()
{
    m_Geo->Open(&m_GeoSink);
    m_GeoSink->SetFillMode(D2D1_FILL_MODE_WINDING);
}


void D2dDrawObject::AfterAdd()
{
#if defined(DEBUG) | defined(_DEBUG)
    std::cout << "GeoSink Was Closed." << std::endl;
#endif
    m_GeoSink->EndFigure(D2D1_FIGURE_END_CLOSED);
    m_GeoSink->Close();
}
