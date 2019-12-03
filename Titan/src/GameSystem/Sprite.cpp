#include "tpch.h"
#include <string>
#include "tpch.h"
#include "Sprite.h"
#include "Transform.h"
#include "GameObject.h"

#include <glm/fwd.hpp>
#include "Renderer/Renderer2D.h"

Sprite::Sprite() 
	:Component(COMPONENT_TYPE::SPRITE)
{
	m_Pos = { 0.0f, 0.0f };
	m_Size = { 0.0f, 0.0f };
	m_Texture = nullptr;
}

Sprite::~Sprite()
{

}

void Sprite::Update()
{	
	if (m_Owner)
	{
		Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));
		if (pTransform)
			m_Pos = glm::vec2(pTransform->m_PosX, pTransform->m_PosY);
	}
	if (m_Texture)
		Draw();
}

void Sprite::Draw()
{
	Titan::Renderer2D::DrawQuad(m_Pos, m_Size, m_Texture);
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
	m_Size = glm::vec2(scaleX, scaleY);
	if (m_Owner)
	{
		Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));
		if (pTransform)
			m_Pos = glm::vec2(pTransform->m_PosX, pTransform->m_PosY);
	}
	else
	{
		m_Pos = glm::vec2(posX, posY);
	}
}

void Sprite::HandleObjectEvent(ObjectEvent* pObjectEvent)
{
}

