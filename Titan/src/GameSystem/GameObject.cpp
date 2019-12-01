#include "tpch.h"
#include "GameObject.h"
#include "Sprite.h"
#include "Transform.h"
#include "Controller.h"
#include "Rigidbody.h"
//#include "UpDown.h"

GameObject::GameObject()
{
}
     
GameObject::~GameObject()
{

}

void GameObject::Initialize()
{
    for(auto pComp : m_Components)
        pComp->Initialize();
}

void GameObject::Update()
{
	for (auto pComp : m_Components)
		pComp->Update();
}

Component* GameObject::GetComponent(COMPONENT_TYPE Type)
{
    for (auto pComp : m_Components)
    {   
        if(pComp->GetType() == Type)
            return pComp;
    }
    return nullptr;
    
}

Component* GameObject::AddComponent(COMPONENT_TYPE Type)
{
	Component* pComponent = nullptr;
	switch (Type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		pComponent = new Transform();
		break;

	case COMPONENT_TYPE::SPRITE:
		pComponent = new Sprite();
		break;

	case COMPONENT_TYPE::CONTROLLER:
		pComponent = new Controller();
		break;

	case COMPONENT_TYPE::RIGIDBODY:
        pComponent = new Rigidbody();
        break;

	/*case COMPONENT_TYPE::UP_DOWN:
		pComponent = new UpDown();
		break;
	*/
            

	default:
		//Warning
		break;
	}
	if (pComponent)
	{
		m_Components.push_back(pComponent);
		pComponent->m_Owner = this;
	}
	return pComponent;
}

void GameObject::HandleObjectEvent(ObjectEvent* pObjectEvent)
{
	for (auto pComp : m_Components)
	{
		pComp->HandleObjectEvent(pObjectEvent);
	}
}
