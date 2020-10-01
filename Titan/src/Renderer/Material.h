#pragma once
#include "Shader.h"

namespace Titan {

	class Texture2D;

	struct PBRTextures
	{
		std::vector<std::shared_ptr<Texture2D>> Albedo;
		std::vector<std::shared_ptr<Texture2D>> Normal;
		std::vector<std::shared_ptr<Texture2D>> Metallic;
		std::vector<std::shared_ptr<Texture2D>> Roughness;
	};

	struct PhongElements
	{
		glm::vec3 Ambient = glm::vec3(0.2);
		glm::vec3 Diffuse = glm::vec3(0.3);
		glm::vec3 Specular = glm::vec3(0.05f);
		float Shininess = 16.0;
	};

	class Material
	{
	public:
		Material();
		virtual ~Material();

		virtual void Bind(const std::shared_ptr<Shader>& shader);
	};

	class PBRMaterial : public Material
	{
	public:
		PBRMaterial();
		PBRMaterial(PBRTextures pbrTextures);
		~PBRMaterial();

		virtual void Bind(const std::shared_ptr<Shader>& shader) override;
		inline PBRTextures GetPBRTextures() { return m_PBR; }
		inline void SetPBRTextures(PBRTextures pbrTexs) { m_PBR = pbrTexs; }

	private:
		PBRTextures m_PBR;
	};

	class PhongMaterial : public Material
	{
	public:
		PhongMaterial();
		~PhongMaterial();

		virtual void Bind(const std::shared_ptr<Shader>& shader) override;
		inline std::shared_ptr<PhongElements>& GetPhong() { return m_Phong; }
		inline std::shared_ptr<Texture2D>& GetTexture() { return m_Texture; }

	private:
		std::shared_ptr<PhongElements> m_Phong;
		std::shared_ptr<Texture2D> m_Texture;
	};
}