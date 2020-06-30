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
	Titan::Light m_Light;
	std::shared_ptr<Titan::Mesh> m_DrawMesh;
	std::shared_ptr<Titan::Mesh> m_DrawMesh2;
	std::shared_ptr<Titan::Mesh> m_DrawMesh3;

	Titan::Ref<Titan::TextureCube> m_TextureCube;
	Titan::Ref<Titan::Shader> m_SkyboxShader;
	Titan::Mesh* m_DrawSkybox;

	std::shared_ptr<Titan::Scene> m_Scene;
	std::vector<Titan::PointLight> m_PointLights;

	//Titan::ParticleSystem m_ParticleSystem;
	//Titan::Particle m_Particle;
};