#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Transform : public Component
{
public:
	Transform();
	~Transform();

	void Serialize(FILE** fpp);
    void Initialize() {}
	void Update(float frameTime);

	float m_PosX, m_PosY;
	float m_Rotate;
	glm::vec3 m_Velocity;
	bool m_Movement;
	
};
