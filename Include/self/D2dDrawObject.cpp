#include <self\D2dDrawObject.h>
#include <self/Dx2dData.h>


D2dDrawObject::D2dDrawObject(D2dData* d2dData, D2dDrawObjectType type)
{
    m_DrawType = type;
    m_PosW = D2D1::Point2(0.0f, 0.0f);
    m_Translate = D2D1::IdentityMatrix();
    m_rotate = D2D1::IdentityMatrix();
    m_scale = D2D1::IdentityMatrix();
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
    m_Translate._31 = position.x;
    m_Translate._32 = position.y;
}

bool D2dDrawObject::IsDirty()
{
    return true;
}

D2D1_POINT_2F* D2dDrawObject::Convertb2Vec2ToPoint2F(b2Vec2 vec[])
{ 
    int countNum = *(&vec + 1) - vec;

    std::vector<D2D1_POINT_2F> pointlist;
    for(int i = 0; i < countNum; ++i)
    {
        D2D1_POINT_2F point = D2D1::Point2(vec[i].x, vec[i].y);
        pointlist.push_back(point);
    }
    
    return pointlist.data();
}


void D2dDrawObject::Add(Box2dObject* box2dObjects)
{
    PrepareAdd();
    b2Vec2* b2vecList = box2dObjects->GetAllShapeVertexPosition();
    D2D1_POINT_2F* pointList = nullptr;
    pointList = Convertb2Vec2ToPoint2F(b2vecList);

    size_t CountPoints = sizeof(pointList) / sizeof(pointList[0]);
    m_GeoSink->SetFillMode(D2D1_FILL_MODE_WINDING);
    m_GeoSink->BeginFigure(pointList[0],D2D1_FIGURE_BEGIN_FILLED);
    for(size_t i = 1; i < CountPoints; ++i)
    {
       m_GeoSink->AddLine(pointList[i]);
    }
    m_GeoSink->EndFigure(D2D1_FIGURE_END_CLOSED);

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
    m_GeoSink->EndFigure(D2D1_FIGURE_END_CLOSED);
    m_GeoSink->Close();
}
