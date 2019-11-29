#pragma once
#include "GameObject.h"

class GameObjectFactory
{
public:
	GameObjectFactory();
	~GameObjectFactory();
	GameObject* LoadObject(const char* pFileName);
	void LoadLevel(const char* pFileName);
};