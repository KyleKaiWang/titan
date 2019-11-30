#include "tpch.h"
#include "GameObjectManager.h"
#include "GameObject.h"

GameObjectManager::GameObjectManager()
{
    
}

GameObjectManager::~GameObjectManager()
{
	for (auto go : m_GameObjects)
		delete go;

    m_GameObjects.clear();
}