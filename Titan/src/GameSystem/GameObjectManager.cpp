#include "tpch.h"
#include "GameObjectManager.h"
#include "GameObject.h"

int GameObjectManager::s_NextGameID = 0;
bool GameObjectManager::s_EnableCollisionDebug = false;
int GameObjectManager::s_AICount;

GameObjectManager::GameObjectManager()
{
	
}

GameObjectManager::~GameObjectManager()
{
	for (auto gameObj : m_GameObjects)
		delete gameObj;

    m_GameObjects.clear();
}

void GameObjectManager::Update(float timeStep)
{
	size_t size = m_GameObjects.size();
	for (int i = 0; i < size; ++i)
	{
		if (m_GameObjects[i] && m_GameObjects[i]->m_HasLifeSpan && !m_GameObjects[i]->Destroyed)
		{
			m_GameObjects[i]->m_LifeSpan -= timeStep;
			if (m_GameObjects[i]->m_LifeSpan <= 0.0f)
			{
				m_DestroyPool.push_back(m_GameObjects[i]);
				m_GameObjects[i]->FlagDestroyed();
				continue;
			}
		}
		m_GameObjects[i]->Update(timeStep);
	}
	std::vector<GameObject*>::iterator it;
	for (it = m_DestroyPool.begin(); it != m_DestroyPool.end();)
	{
		DestroyGameObject((*it)->GetGameID());
		m_DestroyPool.erase(it);
		if (m_DestroyPool.size() <= 2)
			break;
		else
			++it;
	}
}

void GameObjectManager::DestroyGameObject(int gameObjID)
{
	GameObject* destroyObject = nullptr;
	std::vector<GameObject*>::iterator it;
	for (it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		if ((*it)->GetGameID() == gameObjID)
		{
			destroyObject = (*it);
			m_GameObjects.erase(it);
			break;
		}
		else
		{
			++it;
		}
	}
	delete destroyObject;
}

GameObject* GameObjectManager::GetPlayer()
{
	size_t size = m_GameObjects.size();
	for (int i = 0; i < size; ++i)
	{
		if (m_GameObjects[i]->GetGameID() == 1)
			return m_GameObjects[i];
	}
	return nullptr;
}
