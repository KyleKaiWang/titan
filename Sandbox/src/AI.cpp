#include "tpch.h"
#include "AI.h"
#include "SandboxGameObjectFactory.h"
#include "GameSystem/GameObjectManager.h"
#include "GameSystem/Transform.h"
#include "Bullet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLFW/include/GLFW/glfw3.h"

extern SandboxGameObjectFactory* gpSandboxGameObjectFactory;
extern GameObjectManager* gpGameObjectManager;

AI::AI()
{
	m_FireRate = 5.0f;
	m_NextFireTime = 0.0f;
}

AI::~AI()
{
}

void AI::Initialize()
{
	GameObject::Initialize();
	GameObjectManager::s_AICount++;
}

void AI::Update(float timeStep)
{
	GameObject::Update(timeStep);
	if(m_CanFire)
		Fire();
}

void AI::HandleObjectEvent(ObjectEventType eventType)
{
	if (eventType == ObjectEventType::COLLIDE)
	{
		if(AI_Category == 1)
			DoFire(FireType::Regular);
		else if(AI_Category == 2)
			DoFire(FireType::RegularBounce);
		else if (AI_Category == 3)
			DoFire(FireType::DualShot);
		else if (AI_Category == 4)
			DoFire(FireType::DualShotBounce);
		else if (AI_Category == 5)
			DoFire(FireType::TriShot);

		GameObjectManager::s_AICount--;
	}

	GameObject::HandleObjectEvent(eventType);
}

void AI::Serialize(FILE** fpp)
{
	GameObject::Serialize(fpp);

	char stringBuffer[256];
	memset(stringBuffer, 0, sizeof(char));

	while (!feof(*fpp))
	{
		fscanf_s(*fpp, "%255s", stringBuffer, sizeof(stringBuffer));

		//Check AICategory
		if (0 == strcmp(stringBuffer, "AICategory"))
		{
			fscanf_s(*fpp, "%d \n", &AI_Category);
			return;
		}
	}
}

void AI::Fire()
{
	float currentTime = (float)glfwGetTime();
	if (m_NextFireTime < currentTime)
	{
		DoFire(FireType::Regular);
	}
	m_NextFireTime = currentTime + m_FireRate;
}

void AI::DoFire(FireType type)
{
	Transform* pTransform = static_cast<Transform*>(GetComponent(CompoentType::TRANSFORM));
	if (type == FireType::TriShot)
	{
		Bullet* newBullet1 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		Bullet* newBullet2 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		Bullet* newBullet3 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		newBullet1->SetTag("AI");
		newBullet2->SetTag("AI");
		newBullet3->SetTag("AI");
		newBullet1->SetLifeSpan(10.0f);
		newBullet2->SetLifeSpan(10.0f);
		newBullet3->SetLifeSpan(10.0f);
		newBullet1->SetTexture("../Sandbox/assets/textures/Bullet.png");
		newBullet2->SetTexture("../Sandbox/assets/textures/Bullet.png");
		newBullet3->SetTexture("../Sandbox/assets/textures/Bullet.png");
		newBullet1->IsBounceBall = true;
		newBullet2->IsBounceBall = true;
		newBullet3->IsBounceBall = true;
		newBullet2->IsBounceBall = true;
		newBullet3->IsBounceBall = true;
		SetBulletTrajectory(newBullet1, glm::vec2(-1.0f, -1.0f), 3.0f);
		SetBulletTrajectory(newBullet2, glm::vec2(1.0f, -1.0f), 3.0f);
		SetBulletTrajectory(newBullet3, glm::vec2(0.1f, -1.0f), 3.0f);
	}
	else if (type == FireType::DualShot)
	{
		Bullet* newBullet1 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		Bullet* newBullet2 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		newBullet1->SetTag("AI");
		newBullet2->SetTag("AI");
		newBullet1->SetLifeSpan(8.0f);
		newBullet2->SetLifeSpan(8.0f);
		newBullet1->SetTexture("../Sandbox/assets/textures/Bullet3.png");
		newBullet2->SetTexture("../Sandbox/assets/textures/Bullet3.png");
		SetBulletTrajectory(newBullet1, glm::vec2(-1.0f, -1.0f), 3.0f);
		SetBulletTrajectory(newBullet2, glm::vec2(1.0f, -1.0f), 3.0f);
	}
	else if (type == FireType::DualShotBounce)
	{
		Bullet* newBullet1 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		Bullet* newBullet2 = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		newBullet1->SetTag("AI");
		newBullet2->SetTag("AI");
		newBullet1->SetLifeSpan(8.0f);
		newBullet2->SetLifeSpan(8.0f);
		newBullet1->IsBounceBall = true;
		newBullet2->IsBounceBall = true;
		newBullet1->SetTexture("../Sandbox/assets/textures/Bullet3.png");
		newBullet2->SetTexture("../Sandbox/assets/textures/Bullet3.png");
		SetBulletTrajectory(newBullet1, glm::vec2(-1.0f, -1.0f), 3.0f);
		SetBulletTrajectory(newBullet2, glm::vec2(1.0f, -1.0f), 3.0f);
	}
	else if (type == FireType::RegularBounce)
	{
		Bullet* newBullet = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		newBullet->SetTag("AI");
		newBullet->SetLifeSpan(8.0f);
		newBullet->IsBounceBall = true;
		newBullet->SetTexture("../Sandbox/assets/textures/Bullet2.png");
		SetBulletTrajectory(newBullet, glm::vec2(0.0f), 5.0f);
		
	}
	else
	{
		Bullet* newBullet = static_cast<Bullet*>(gpSandboxGameObjectFactory->CreateGameObject(GameObjectClass::Bullet));
		newBullet->SetTag("AI");
		newBullet->SetLifeSpan(3.0f);
		newBullet->SetTexture("../Sandbox/assets/textures/Bullet1.png");
		SetBulletTrajectory(newBullet, glm::vec2(0.0f), 5.0f);
	}
}

void AI::SetBulletTrajectory(GameObject* bullet, glm::vec2 dir, float speed)
{
	Transform* pTransform = static_cast<Transform*>(GetComponent(CompoentType::TRANSFORM));
	Transform* pBulletTransform = static_cast<Transform*>(bullet->GetComponent(CompoentType::TRANSFORM));
	if (pBulletTransform && pTransform && dir.x == 0.0f && dir.y == 0.0f)
	{
		pBulletTransform->m_PosX = pTransform->m_PosX;
		pBulletTransform->m_PosY = pTransform->m_PosY;
	
		GameObject* player = gpGameObjectManager->GetPlayer();
		if (player)
		{
			glm::vec2 playerLoc(player->GetPosX(), player->GetPosY());
			glm::vec2 targetVec = glm::normalize(playerLoc - glm::vec2(pTransform->m_PosX, pTransform->m_PosY));
			pBulletTransform->m_Velocity = glm::vec3(targetVec, 0.0f) * speed;
		}
	}
	else if (pBulletTransform && pTransform)
	{
		pBulletTransform->m_PosX = pTransform->m_PosX;
		pBulletTransform->m_PosY = pTransform->m_PosY;
		glm::vec2 targetVec = dir;
			
		pBulletTransform->m_Velocity = glm::vec3(targetVec, 0.0f) * speed;
	}
}