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

	std::shared_ptr<Titan::Shader> m_Shader;
	std::shared_ptr<Titan::VertexArray> m_VertexArray;
	std::shared_ptr<Titan::Shader> m_FlatColorShader;
	std::shared_ptr<Titan::VertexArray> m_SquareVA;
	std::shared_ptr<Titan::Shader> m_TextureShader;
	std::shared_ptr<Titan::Texture2D> m_Texture;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	Titan::Ref<Titan::Sprite> m_Sprite;

	Titan::ParticleSystem m_ParticleSystem;
	Titan::Particle m_Particle;
	uint32_t m_ParticlePoolSize = 1000;

	bool m_ParticleActive = true;

};