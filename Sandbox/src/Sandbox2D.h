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
	Titan::Ref<Titan::Texture2D> m_CheckerboardTexture;

};