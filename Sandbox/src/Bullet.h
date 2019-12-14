#pragma once
#include "GameSystem/GameObject.h"
#include <glm/glm.hpp>

class Bullet : public GameObject
{
public:
	Bullet();
	~Bullet();
	
	virtual void Initialize();
	void Update();

	void ChangeDir(const char c);
	void SetTexture(const char* textureFilePath);

	bool IsBounceBall = false;
};