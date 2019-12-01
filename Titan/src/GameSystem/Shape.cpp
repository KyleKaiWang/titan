#include "tpch.h"
#include "Shape.h"
#include "Rigidbody.h"

Shape::Shape(SHAPE_TYPE Type)
{
	m_Type = Type;
	m_OwnerRigidbody = nullptr;
}

ShapeCircle::ShapeCircle() 
	: Shape(SHAPE_TYPE::CIRCLE)
{
	m_Radius = 0.0f;
}

ShapeCircle::~ShapeCircle()
{

}

bool ShapeCircle::TestPoint(float pointX, float pointY)
{
	/*float spDist;

	if(spDist > (mRadius * mRadius))
		return true;*/
	return false;
}

ShapeAABB::ShapeAABB() 
	: Shape(SHAPE_TYPE::AABB)
{
	m_Width = m_Height = 0.0f;
}

ShapeAABB::~ShapeAABB()
{

}

bool ShapeAABB::TestPoint(float x, float y)
{
	return false;
}