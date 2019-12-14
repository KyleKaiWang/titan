#pragma once
#include <vector>
#include "GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Update(float timeStep);
	void DestroyGameObject(int gameObjID);
	GameObject* GetPlayer();
	std::vector<GameObject*> m_GameObjects;

	static bool s_EnableCollisionDebug;
	static int s_NextGameID;
	static int s_AICount;

private:
	std::vector<GameObject*> m_DestroyPool;
};