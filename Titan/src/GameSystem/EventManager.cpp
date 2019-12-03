#include "tpch.h"
#include "EventManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"

extern GameObjectManager* gpGameObjectManager;

ObjectEventManager::ObjectEventManager()
{
    
}

ObjectEventManager::~ObjectEventManager()
{
    
}

void ObjectEventManager::BroadCastEvent(ObjectEvent* objectEvent)
{
    for(GameObject* pObj : gpGameObjectManager->m_GameObjects)
    {
        pObj->HandleObjectEvent(objectEvent);
    }
}

void ObjectEventManager::BroadCastEventToSubscribers(ObjectEvent *objectEvent)
{
	auto& listOfSubscribers = m_Subscriptions[objectEvent->GetObjectEventType()];
    for(auto pRegisteredGO : listOfSubscribers)
    {
		pRegisteredGO->HandleObjectEvent(objectEvent);
    }
}

void ObjectEventManager::AddDelayEvent(ObjectEvent *objectEvent, float delayTime)
{
	objectEvent->SetDelayTimer(delayTime);
	m_Events.push_back(objectEvent);
}

void ObjectEventManager::Update(float frameTime)
{
    auto it = m_Events.begin();
    
    while(it != m_Events.end())
    {
        ObjectEvent *objectEvent = *it;
		float delayTime = objectEvent->GetDelayTimer() - frameTime;
		objectEvent->SetDelayTimer(delayTime);
        
        if(objectEvent->GetDelayTimer() < 0.0f)
        {
			BroadCastEvent(objectEvent);
            delete objectEvent;
            it = m_Events.erase(it);
        }
        else
			++it;
    }
}

void ObjectEventManager::Subscribe(OBJECT_EVENT_TYPE eventType, GameObject * gameObj)
{
    for(auto RegisteredGameObj : m_Subscriptions[eventType])
    {
        if(RegisteredGameObj == gameObj)
            return;
    }
    m_Subscriptions[eventType].push_back(gameObj);
}

void ObjectEventManager::UnSubscribe(OBJECT_EVENT_TYPE type, GameObject* gameObj)
{
	auto& listOfSubscribers = m_Subscriptions[type];
	auto it = std::find(listOfSubscribers.begin(), listOfSubscribers.end(), gameObj);
	if (it == listOfSubscribers.end()) 
	{
		return;
	}
	listOfSubscribers.erase(it);
}

ObjectEvent::ObjectEvent()
{
	m_ObjectEventType = OBJECT_EVENT_TYPE::NUM;
	m_DelayTimer = 0.0f;
}
