#pragma once
#include "Titan.h"

class SandboxDeferred : public Titan::Layer
{
public:
	SandboxDeferred();
	virtual ~SandboxDeferred() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Titan::Timestep ts) override;
	virtual void OnImGuiRender() override; 
	void OnEvent(Titan::Event& event) override;

private:
	Titan::CameraController m_CameraController;
	Titan::Ref<Titan::Texture2D> m_Texture;
	Titan::Ref<Titan::Shader> m_Shader;
	glm::vec3 m_Color = { 0.2f, 0.3f, 0.8f };
	Titan::Mesh* m_DrawMesh;
	Titan::Mesh* m_DrawMesh2;

	Titan::Ref<Titan::TextureCube> m_TextureCube;
	Titan::Ref<Titan::Shader> m_SkyboxShader;
	Titan::Mesh* m_DrawSkybox;

	std::shared_ptr<Titan::Scene> m_Scene;
	std::shared_ptr<Titan::PhongMaterial> m_PhongMaterial;

	glm::vec3 m_Ambient = glm::vec3(0.9f, 0.5f, 0.3f);
	glm::vec3 m_Diffuse = glm::vec3(0.9f, 0.5f, 0.3f);
	glm::vec3 m_Specular = glm::vec3(0.05f, 0.05f, 0.05f);
	float m_Shininess = 100.0f;

	//Titan::ParticleSystem m_ParticleSystem;
	//Titan::Particle m_Particle;
};