#include "tpch.h"
#include "Controller.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"

#include "Input.h"
#include "KeyCodes.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"
#include <GLFW/glfw3.h>

extern GameObjectFactory* gpGameObjectFactory;

Controller::Controller()
	: Component(CompoentType::CONTROLLER)
{
}

Controller::~Controller()
{
}

void Controller::Update(float timeStep)
{
	if (!m_Owner)
		return;

	Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(CompoentType::TRANSFORM));
	if (!pTransform)
		return;

	Sprite* pSprite = static_cast<Sprite*>(m_Owner->GetComponent(CompoentType::SPRITE));
	if (!pSprite)
		return;

	if (Titan::Input::IsKeyPressed(TITAN_KEY_UP) || Titan::Input::IsKeyPressed(TITAN_KEY_W))
	{
		pTransform->m_PosY += 10.0f * timeStep;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_DOWN) || Titan::Input::IsKeyPressed(TITAN_KEY_S))
	{
		pTransform->m_PosY -= 10.0f * timeStep;
	}
	else if(Titan::Input::IsKeyPressed(TITAN_KEY_RIGHT) || Titan::Input::IsKeyPressed(TITAN_KEY_D))
	{
		pTransform->m_PosX += 10.0f * timeStep;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_LEFT) || Titan::Input::IsKeyPressed(TITAN_KEY_A))
	{
		pTransform->m_PosX -= 10.0f * timeStep;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_Q))
	{
		pTransform->m_Rotate += 10.0f * timeStep;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_E))
	{
		pTransform->m_Rotate -= 10.0f * timeStep;
	}

	//Press C to show collision debug
	if (Titan::Input::IsKeyPressed(TITAN_KEY_C))
	{
		GameObjectManager::s_EnableCollisionDebug == true ? false : true;
	}
}

void Controller::HandleObjectEvent(ObjectEventType eventType)
{
	if (eventType == ObjectEventType::COLLIDE)
	{
		TITAN_INFO("Controller : Collide event!");
	}
}
