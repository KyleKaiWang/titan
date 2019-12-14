#include "tpch.h"
#include <string>
#include "tpch.h"
#include "Sprite.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "Shape.h"

#include <glm/fwd.hpp>
#include "Renderer/Renderer2D.h"
#include "Platform/OpenGL/OpenGLDebugDraw.h"

Sprite::Sprite() 
	:Component(CompoentType::SPRITE)
{
	m_Pos = { 0.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f };
	m_Rotate = 0.0f;
	m_Texture = nullptr;
}

Sprite::~Sprite()
{

}

void Sprite::Update(float frameTime)
{	
	if (m_Owner)
	{
		Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(CompoentType::TRANSFORM));
		if (pTransform)
		{
			m_Pos = glm::vec2(pTransform->m_PosX, pTransform->m_PosY);
			m_Rotate = pTransform->m_Rotate;
		}
	}
	if (IsVisiable && m_Texture)
		DrawSprite();
}

void Sprite::DrawSprite()
{
	Titan::Renderer2D::DrawQuad(m_Pos, m_Scale, m_Rotate, m_Texture);
	if (GameObjectManager::s_EnableCollisionDebug)
	{
		Rigidbody* pRigidbody = static_cast<Rigidbody*>(m_Owner->GetComponent(CompoentType::RIGIDBODY));
		if (pRigidbody)
		{
			Titan::OpenGLDebugDraw::AddAABB(glm::vec3(m_Pos, 1.0f), pRigidbody->m_Shape->GetShapeSize(), m_Rotate, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f, false);
		}
	}
}

void Sprite::SetSpriteTexture(const std::string& textureFilePath)
{
	m_Texture = Titan::Texture2D::Create(textureFilePath);
}

void Sprite::Serialize(FILE** fpp)
{
    char textureName[256];
    memset(textureName, 0, 256 * sizeof(char));
    
	float scaleX, scaleY;
	float posX, posY;

	std::string fullPath = "../Sandbox/assets/textures/";
    fscanf_s(*fpp, "%255s %f %f %f %f\n", textureName, sizeof(textureName), &posX, &posY, &scaleX, &scaleY);
	fullPath += textureName;

	m_Texture = Titan::Texture2D::Create(fullPath.c_str());
	m_Scale = glm::vec2(scaleX, scaleY);
	if (m_Owner)
	{
		Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(CompoentType::TRANSFORM));
		if (pTransform)
			m_Pos = glm::vec2(pTransform->m_PosX, pTransform->m_PosY);
	}
	else
	{
		m_Pos = glm::vec2(posX, posY);
	}
}

void Sprite::HandleObjectEvent(ObjectEventType eventType)
{
}

