#include "PlayerController.h"
#include "Bullet.h"

#include "Input.h"
#include "KeyCodes.h"
#include <Log.h>

#include "GameSystem/GameObjectManager.h"
#include "GameSystem/Transform.h"
#include "GLFW/include/GLFW/glfw3.h"
#include "SandboxGameObjectFactory.h"


extern SandboxGameObjectFactory* gpSandboxGameObjectFactory;

PlayerController::PlayerController()
{
	m_NextShootTime = 0.0f;
	m_FireRate = 0.1f;
}

PlayerController::~PlayerController()
{
}

void PlayerController::Update(float timeStep)
{
	Controller::Update(timeStep);

	//Press F to fire bullet
	if (Titan::Input::IsKeyPressed(TITAN_KEY_F))
	{
		Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(CompoentType::TRANSFORM));
		if (!pTransform)
			return;

		float currentTime = (float)glfwGetTime();
		if (m_NextShootTime < currentTime)
		{
			Bullet* bullet = dynamic_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
			Transform* pBulletTransform = static_cast<Transform*>(bullet->GetComponent(CompoentType::TRANSFORM));
			if (m_BounceBullet)
			{
				if (pBulletTransform)
				{
					pBulletTransform->m_PosX = pTransform->m_PosX;
					pBulletTransform->m_PosY = pTransform->m_PosY;
					pBulletTransform->m_Velocity = glm::vec3(-sin(pTransform->m_Rotate), cos(pTransform->m_Rotate), 0.0f) * 5.0f;
					bullet->IsBounceBall = true;
					bullet->SetLifeSpan(5.0f);
					bullet->SetTexture("../Sandbox/assets/textures/Bullet.png");
				}
			}
			else
			{
				if (pBulletTransform)
				{
					pBulletTransform->m_PosX = pTransform->m_PosX;
					pBulletTransform->m_PosY = pTransform->m_PosY;
					pBulletTransform->m_Velocity = glm::vec3(-sin(pTransform->m_Rotate), cos(pTransform->m_Rotate), 0.0f) * 10.0f;
					bullet->SetLifeSpan(3.0f);
				}
			}
		m_NextShootTime = currentTime + m_FireRate;
		}
	}

	if (Titan::Input::IsKeyPressed(TITAN_KEY_R))
	{
		if (m_BounceBullet)
			m_BounceBullet = false;
		else
			m_BounceBullet = true;
	}

	//Press C to show collision debug
	if (Titan::Input::IsKeyPressed(TITAN_KEY_C))
	{
		if (GameObjectManager::s_EnableCollisionDebug == true)
			GameObjectManager::s_EnableCollisionDebug = false;
		else
			GameObjectManager::s_EnableCollisionDebug = true;
	}

}

void PlayerController::HandleObjectEvent(ObjectEventType eventType)
{
	if (eventType == ObjectEventType::COLLIDE)
	{
		TITAN_INFO("Controller : Collide event!");
	}
}
