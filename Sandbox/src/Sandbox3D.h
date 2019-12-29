#pragma once
#include "Titan.h"

class Sandbox3D : public Titan::Layer
{
public:
	Sandbox3D();
	virtual ~Sandbox3D() = default;

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

};