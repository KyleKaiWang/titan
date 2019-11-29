#include "tpch.h"
#include "Transform.h"

Transform::Transform() 
	:Component(COMPONENT_TYPE::TRANSFORM)
{
    m_PosX = 0;
    m_PosY = 0;
}

Transform::~Transform()
{

}

void Transform::Serialize(FILE** fpp)
{
	fscanf_s(*fpp, "%f %f\n", &m_PosX, &m_PosY);
}

void Transform::Update()
{

}
