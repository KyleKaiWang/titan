#pragma once
#include "GameObject.h"
#include "Component.h"

class GameObjectFactory
{
public:
	GameObjectFactory();
	virtual ~GameObjectFactory();

	virtual void LoadLevelFromFile(const char* pFileName);
	virtual GameObject* LoadObjectFromFile(const char* pFileName);
	virtual GameObject* CreateGameObject(const char* pFileName);
	virtual Component* ComponentTypeHandler(CompoentType componentType);
};
