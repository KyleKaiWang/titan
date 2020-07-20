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
	std::shared_ptr<Titan::Model> m_DrawModel;
	std::shared_ptr<Titan::Mesh> m_DrawLight;
	std::shared_ptr<Titan::Mesh> m_DrawSphere;
	std::shared_ptr<Titan::Mesh> m_DrawCube;
	std::shared_ptr<Titan::Mesh> m_DrawPlane;
	std::shared_ptr<Titan::Mesh> m_DrawSkybox;

	std::vector<std::shared_ptr<Titan::PBRMaterial>> m_PBRMats;
	std::vector<Titan::PointLight> m_PointLights;

	std::shared_ptr<Titan::Framebuffer> m_FBO;
};