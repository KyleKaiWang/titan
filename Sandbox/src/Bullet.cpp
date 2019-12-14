#include "tpch.h"
#include "Bullet.h"
#include "GameSystem/Rigidbody.h"
#include "GameSystem/Transform.h"
#include "GameSystem/Sprite.h"

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize()
{
	GameObject::Initialize();
	Rigidbody* pRigidbody = static_cast<Rigidbody*>(GetComponent(CompoentType::RIGIDBODY));
	if (pRigidbody)
	{
		pRigidbody->IgnoreSameTag = true;
	}
}

void Bullet::Update()
{
	
}

void Bullet::ChangeDir(const char c)
{
	Transform* pTransform = static_cast<Transform*>(GetComponent(CompoentType::TRANSFORM));

	if (c == 'x')
	{
		pTransform->m_Velocity.x *= (-1);
	}
	else
	{
		pTransform->m_Velocity.y *= (-1);
	}
}

void Bullet::SetTexture(const char* textureFilePath)
{
	Sprite* pSprite = static_cast<Sprite*>(GetComponent(CompoentType::SPRITE));
	if(pSprite)
		pSprite->SetSpriteTexture(textureFilePath);
}
