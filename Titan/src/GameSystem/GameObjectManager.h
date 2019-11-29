#pragma once
#include <vector>
#include "GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	std::vector<GameObject*> m_GameObjects;
};