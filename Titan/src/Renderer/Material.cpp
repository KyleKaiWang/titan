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

	PBRMaterial::PBRMaterial(std::shared_ptr<PBRTextures> pbrTextures)
		: m_PBR(pbrTextures)
	{
	}

	PBRMaterial::~PBRMaterial()
	{
	}

	void PBRMaterial::Bind(const std::shared_ptr<Shader>& shader)
	{
		uint32_t i = 0;
		for (auto tex : m_PBR->Albedo) tex->Bind(i++);
		for (auto tex : m_PBR->Normal) tex->Bind(i++);
		for (auto tex : m_PBR->Metallic) tex->Bind(i++);
		for (auto tex : m_PBR->Roughness) tex->Bind(i++);

		// TODO : make this part support array
		shader->SetInt("g_Albedo", 0);
		shader->SetInt("g_Normal", 1);
		shader->SetInt("g_Metallic", 2);
		shader->SetInt("g_Roughness", 3);
	}
}