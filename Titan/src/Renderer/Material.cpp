#include "tpch.h"
#include "Material.h"
#include "Texture.h"

namespace Titan {
	Material::Material()
	{
	}

	Material::~Material()
	{
	}

	void Material::Bind(const std::shared_ptr<Shader>& shader)
	{
	}

	PhongMaterial::PhongMaterial()
	{
		m_Texture = Texture2D::Create(1,1);
		m_Phong = std::make_shared<PhongElements>();
		m_Phong->Ambient = glm::vec3(0.4);
		m_Phong->Diffuse = glm::vec3(0.3);
		m_Phong->Specular = glm::vec3(0.05f);
		m_Phong->Shininess = 16.0;
	}

	PhongMaterial::~PhongMaterial()
	{
	}

	void PhongMaterial::Bind(const std::shared_ptr<Shader>& shader)
	{
		shader->Bind();
		shader->SetFloat3("u_Ambient", GetPhong()->Ambient);
		shader->SetFloat3("u_Diffuse", GetPhong()->Diffuse);
		shader->SetFloat3("u_Specular", GetPhong()->Specular);
		shader->SetFloat("u_Shininess", GetPhong()->Shininess);
		
		m_Texture->Bind();
		shader->SetInt("u_Texture", 0);
	}

	PBRMaterial::PBRMaterial()
	{
		m_PBR = std::make_shared<PBRTextures>();
	}

	PBRMaterial::~PBRMaterial()
	{
	}

	void PBRMaterial::Bind(const std::shared_ptr<Shader>& shader)
	{
		m_PBR->Albedo->Bind(0);
		m_PBR->Normal->Bind(1);
		m_PBR->Metallic->Bind(2);
		m_PBR->Roughness->Bind(3);
		shader->SetInt("u_Albedo", 0);
		shader->SetInt("u_Normal", 1);
		shader->SetInt("u_Metallic", 2);
		shader->SetInt("u_Roughness", 3);
	}
}