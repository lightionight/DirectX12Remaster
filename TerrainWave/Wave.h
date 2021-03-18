#pragma once
// This Wave class focus animation terrain

#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class Wave
{
public:
	Wave(int m, int n, float dx, float dy, float speed, float damping);
	Wave(const Wave& rhs) = delete;
	Wave& operator=(const Wave& rhs) = delete;
	~Wave();
public:
	int RowCount()const;
	int ColumnCount()const;
	int vertexCount()const;
	int TriangleCount()const;
	float Width()const;
	float Depth()const;
	void Update(float dt);
	void Disturb(int i, int j, float magnitude);
// Parameter
public:
	const XMFLOAT3& Position(int i)const { return mCurrentSolution[i]; }
	const XMFLOAT3& Normal(int i)const { return mNormals[i]; }
	const XMFLOAT3& Tangle(int i)const { return mTangents[i]; }
private:
	int mNumRows = 0;
	int mNumCols = 0;
	int mVertexCount = 0;
	int mTriangleCount = 0;
	float mk1 = 0.0f;
	float mk2 = 0.0f;
	float mk3 = 0.0f;
	float mTimeStep = 0.0f;
	float mSpatialStep = 0.0f;
	std::vector<XMFLOAT3> mPrevSolution;
	std::vector<XMFLOAT3> mCurrentSolution;
	std::vector<XMFLOAT3> mNormals;
	std::vector<XMFLOAT3> mTangents;
};