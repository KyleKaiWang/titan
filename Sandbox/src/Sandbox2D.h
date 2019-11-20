#pragma once
#include "Titan.h"

class Sandbox2D : public Titan::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void OnUpdate(Titan::Timestep ts) override;
	virtual void OnImGuiRender() override; 
	void OnEvent(Titan::Event& event) override;
private:
	Titan::OrthographicCameraController m_CameraController;

	Titan::Ref<Titan::Shader> m_Shader;
	Titan::Ref<Titan::VertexArray> m_VertexArray;

	Titan::Ref<Titan::Shader> m_FlatColorShader;
	Titan::Ref<Titan::VertexArray> m_SquareVA;

	Titan::Ref<Titan::Shader> m_TextureShader;
	Titan::Ref<Titan::Texture2D> m_Texture;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

};