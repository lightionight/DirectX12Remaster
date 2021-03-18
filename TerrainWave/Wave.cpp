#include "Wave.h"
#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>

using namespace DirectX;

Wave::Wave(int numRows, int numCols, float dx, float dt, float speed, float damping)
{
	mNumRows = numRows;
	mNumCols = numCols;
	mVertexCount = mNumRows * mNumCols;
	mTriangleCount = (mNumRows - 1) * (mNumCols - 1) * 2;
	mTimeStep = dt;
	mSpatialStep = dx;
	float d = damping * dt + 2.0f;//??????????????
	float e = (speed * speed) * (dt * dt) / (dx * dx);//?????????????????????
	mk1 = (damping * dt - 2.0f) / d;
	mk2 = (4.0f - 8.0f * e) / d;
	mk3 = (2.0f * e) / d;
	mPrevSolution.resize(mNumRows * mNumCols);
	mCurrentSolution.resize(mNumRows * mNumCols);
	mNormals.resize(mNumRows * mNumCols);
	mTangents.resize(mNumRows * mNumCols);
// using reGenerate vertices
	float halfWidth = (numCols - 1) * dx * 0.5f;
	float haifDepth = (numRows - 1) * dx * 0.5f;
	for (int i = 0; i < numRows; ++i)
	{
		float z = haifDepth - i * dx;
		for (int j = 0; j < numCols; ++j)
		{
			float x = -halfWidth + j * dx;

			mPrevSolution[i * numCols + j] = XMFLOAT3(x, 0.0f, z);
			mCurrentSolution[i * numCols + j] = XMFLOAT3(x, 0.0f, z);
			mNormals[i * numCols + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mTangents[i * numCols + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
// after this function all variable will be initialize.
		}
	}
}
Wave::~Wave() { }
int Wave::ColumnCount() const { return mNumCols; }

int Wave::RowCount()const { return mNumRows; }

int Wave::vertexCount()const { return mVertexCount; }

int Wave::TriangleCount()const { return mTriangleCount; }

float Wave::Width()const { return mNumCols * mSpatialStep; }

float Wave::Depth()const { return mNumRows * mSpatialStep; }

void Wave::Update(float dt)
{
	static float t = 0;
	t += dt;
	if (t >= mTimeStep) // if Big then TimeStep
	{
// Parallel computer
		concurrency::parallel_for(
			1, mNumRows - 1, [this](int i)
			// Instead for(int i = 1; i < mNumRows-1; ++i)
			{
				for (int j = 1; j < mNumCols - 1; ++j)
				{
					// This Sound like DNN neturl network filter
					mPrevSolution[i * mNumCols + j].y =
						mk1 * mPrevSolution[i * mNumCols + j].y +
						mk2 * mCurrentSolution[i * mNumCols + j].y +
						mk3 * (mCurrentSolution[(i + 1) * mNumCols + j].y +
							mCurrentSolution[(i - 1) * mNumCols + j].y +
							mCurrentSolution[i * mNumCols + j + 1].y +
							mCurrentSolution[i * mNumCols + j - 1].y
							);
				}
			}
		);
		std::swap(mPrevSolution, mCurrentSolution);
		
		//reset Time and computer normal
		t = 0.0f;
		concurrency::parallel_for(
			1, mNumRows - 1, [this](int i)
			{
				for (int j; j < mNumCols - 1; ++j)
				{
					//        - T - 
					//   -L- current - R -
					//        - B -
					float L = mCurrentSolution[i * mNumCols + j - 1].y;
					float R = mCurrentSolution[i * mNumCols + j + 1].y;
					float T = mCurrentSolution[(i - 1) * mNumCols + j].y;
					float B = mCurrentSolution[(i + 1) * mNumCols + j].y;
					mNormals[i * mNumCols + j].x = -R + L;
					mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
					mNormals[i * mNumCols + j].z = B - T;
					
					// DO normalize  normal and store them back. 
					XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
					XMStoreFloat3(&mCurrentSolution[i * mNumCols + j], n);
					// Calculate Tangent base on Normal
					mTangents[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, R - L, 0.0f);
					XMVECTOR Tan = XMVector3Normalize(XMLoadFloat3(&mTangents[i * mNumCols + j]));
					XMStoreFloat3(&mTangents[i * mNumCols + j], Tan);
				}
			}
		);

	}
}

void Wave::Disturb(int i, int j, float magnitude)
{
	// don't move border
	assert(i > 1 && i < mNumRows - 2);
	assert(j > 1 && j < mNumCols - 2);

	float halfMag = 0.5 * magnitude;
	mCurrentSolution[i * mNumCols + j].y += magnitude;
	mCurrentSolution[i * mNumCols + j + 1].y += halfMag;
	mCurrentSolution[i * mNumCols + j - 1].y += halfMag;
	mCurrentSolution[(i + 1) * mNumCols + j].y += halfMag;
	mCurrentSolution[(i - 1) * mNumCols + j].y += halfMag;

}
