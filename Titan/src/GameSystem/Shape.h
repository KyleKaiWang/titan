#pragma once
#include "Rigidbody.h"

class Shape
{
public:
	enum SHAPE_TYPE {
		CIRCLE,
		AABB,
		NUM
	};

	Shape(SHAPE_TYPE Type);
	virtual ~Shape() {}

	
	inline virtual glm::vec2 GetShapeSize() = 0;
	virtual bool TestPoint(float PointX, float PointY) = 0;
	Rigidbody* m_OwnerRigidbody;
	SHAPE_TYPE m_Type;

};

class ShapeCircle : public Shape
{
public:
	ShapeCircle();
	~ShapeCircle();
	
	//TODO:Fix it
	inline virtual glm::vec2 GetShapeSize() { return glm::vec2(m_Radius, m_Radius); }
	bool TestPoint(float pointX, float pointY);

	float m_Radius;
};

class ShapeAABB : public Shape
{
public:
	ShapeAABB();
	~ShapeAABB();

	inline virtual glm::vec2 GetShapeSize() { return glm::vec2(m_Width, m_Height); }
	virtual bool TestPoint(float x, float y);

	float m_Width;
	float m_Height;
};