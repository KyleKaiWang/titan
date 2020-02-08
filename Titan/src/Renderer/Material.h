#pragma once
#include "Shader.h"
#

namespace Titan {

	class Texture2D;

	class Material
	{
	public:
		Material();
		virtual ~Material();

		inline const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }
	protected:
		std::shared_ptr<Shader> m_Shader;
	};

	class PhongMaterial : public Material
	{
	public:
		PhongMaterial();
		~PhongMaterial();

		void BindPhongParam(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

		glm::vec3 Ambient = glm::vec3(0.9f, 0.5f, 0.3f);
		glm::vec3 Diffuse =	glm::vec3(0.9f, 0.5f, 0.3f);
		glm::vec3 Specular = glm::vec3(0.05f, 0.05f, 0.05f);
		float Shininess = 100.0f;
		Titan::Ref<Texture2D> PhongTexture;
	};
}