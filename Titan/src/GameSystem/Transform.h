#pragma once
#include "Component.h"
#include "Sprite.h"

class Transform : public Component
{
public:
	Transform();
	~Transform();

	void Serialize(FILE** fpp);
    void Initialize() {}
	void Update();

	float m_PosX, m_PosY;
};
