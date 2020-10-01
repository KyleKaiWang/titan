#pragma once
#include "Titan.h"

class SandboxDeferredRendering : public Titan::Layer
{
public:
	SandboxDeferredRendering();
	virtual ~SandboxDeferredRendering() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Titan::Timestep ts) override;
	virtual void OnImGuiRender() override; 
	void OnEvent(Titan::Event& event) override;

	void DrawSceneObjects(const std::shared_ptr<Titan::Shader>& shader);

private:
	Titan::CameraController m_CameraController;
	Titan::DirectionalLight m_DirLight;
	std::shared_ptr<Titan::Model> m_DrawModel;
	std::shared_ptr<Titan::Model> m_DrawModel2;
	std::shared_ptr<Titan::Mesh> m_DrawLight;
	std::shared_ptr<Titan::Mesh> m_DrawSphere;
	std::shared_ptr<Titan::Mesh> m_DrawCube;
	std::shared_ptr<Titan::Mesh> m_DrawPlane;
	std::shared_ptr<Titan::Mesh> m_DrawSkybox;

	std::vector<std::shared_ptr<Titan::PBRMaterial>> m_PBRMats;
	std::vector<Titan::PointLight> m_PointLights;

	std::shared_ptr<Titan::Framebuffer> m_FBO;

	Titan::ClusteredRenderer m_ClusteredRenderer;
};