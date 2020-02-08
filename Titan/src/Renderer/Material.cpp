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

	PhongMaterial::PhongMaterial()
	{
		m_Shader = Shader::Create("shaders/PhongTexture.vs", "shaders/PhongTexture.fs");
		PhongTexture = Texture2D::Create("assets/textures/Brick.png");
	}

	PhongMaterial::~PhongMaterial()
	{
	}

	void PhongMaterial::BindPhongParam(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
	{
		PhongTexture->Bind();
		m_Shader->Bind();
		m_Shader->SetFloat3("Ka", ambient);
		m_Shader->SetFloat3("Kd", diffuse);
		m_Shader->SetFloat3("Ks", specular);
		m_Shader->SetFloat("u_Shininess", shininess);
		m_Shader->SetFloat4("u_Color", glm::vec4(1.0f));
		m_Shader->SetInt("u_Texture", 0);
	}
}