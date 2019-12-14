#include "tpch.h"
#include "GameObject.h"
#include "Sprite.h"
#include "Transform.h"
#include "Controller.h"
#include "Rigidbody.h"

GameObject::GameObject()
{
	m_LifeSpan = -1.0f;
}
     
GameObject::~GameObject()
{
	for (auto comp: m_Components)
		delete comp;

	m_Components.clear();
}

void GameObject::Initialize()
{
    for(auto pComp : m_Components)
        pComp->Initialize();
}

void GameObject::Update(float timeStep)
{
	for (auto pComp : m_Components)
		pComp->Update(timeStep);
}

Component* GameObject::GetComponent(CompoentType Type)
{
    for (auto pComp : m_Components)
    {   
        if(pComp->GetType() == Type)
            return pComp;
    }
    return nullptr;
    
}

Component* GameObject::AddComponent(CompoentType Type)
{
	Component* pComponent = nullptr;
	switch (Type)
	{
	case CompoentType::TRANSFORM:
		pComponent = new Transform();
		break;

	case CompoentType::SPRITE:
		pComponent = new Sprite();
		break;

	case CompoentType::CONTROLLER:
		pComponent = new Controller();
		break;

	case CompoentType::RIGIDBODY:
        pComponent = new Rigidbody();
        break;

	default:
		break;
	}
	if (pComponent)
	{
		m_Components.push_back(pComponent);
		pComponent->m_Owner = this;
	}
	return pComponent;
}

void GameObject::AddComponent(Component* component)
{
	m_Components.push_back(component);
	component->m_Owner = this;
}

void GameObject::HandleObjectEvent(ObjectEventType eventType)
{
	//if (objectEvent->GetObjectEventType() == OBJECT_EVENT_TYPE::COLLIDE)
	//{
	//	FlagDestroyed();
	//}

	for (auto pComp : m_Components)
	{
		if(!Destroyed)
			pComp->HandleObjectEvent(eventType);
	}
}

float GameObject::GetPosX()
{
	Transform* pTransform = static_cast<Transform*>(GetComponent(CompoentType::TRANSFORM));
	if(pTransform)
		return pTransform->m_PosX;
	return 0.0f;
}

float GameObject::GetPosY()
{
	Transform* pTransform = static_cast<Transform*>(GetComponent(CompoentType::TRANSFORM));
	if (pTransform)
		return pTransform->m_PosY;
	return 0.0f;
}

void GameObject::FlagDestroyed()
{
	Destroyed = true;
}

