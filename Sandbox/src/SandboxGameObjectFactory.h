#pragma once
#include "GameSystem/GameObjectFactory.h"

enum class GameObjectClass
{
	None = 0, Bullet, AI
};

class SandboxGameObjectFactory : public GameObjectFactory
{
public:
	SandboxGameObjectFactory();
	~SandboxGameObjectFactory();

	GameObject* LoadObjectFromFile(const char* pFileName);
	GameObject* CreateGameObject(GameObjectClass type);
	Component* ComponentTypeHandler(CompoentType componentType);
};
