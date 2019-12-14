#include "tpch.h"
#include "Transform.h"

Transform::Transform() 
	:Component(CompoentType::TRANSFORM)
{
    m_PosX = 0.0f;
    m_PosY = 0.0f;
	m_Rotate = 0.0f; 
	m_Velocity = glm::vec3(0.0f);
	m_Movement = false;
}

Transform::~Transform()
{

}

void Transform::Serialize(FILE** fpp)
{
	int movement = 0;
	fscanf_s(*fpp, "%f %f %f %i\n", &m_PosX, &m_PosY, &m_Rotate, &movement);
	m_Movement = movement;
}

void Transform::Update(float frameTime)
{
	if (m_Movement)
	{
		m_PosX += m_Velocity.x * frameTime;
		m_PosY += m_Velocity.y * frameTime;
	}
}
