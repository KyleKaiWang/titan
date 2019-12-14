#pragma once
#include "Titan.h"
#include "imgui/imgui.h"

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

	inline Titan::OrthographicCameraController GetCameraController() const { return m_CameraController; }

private:
	Titan::OrthographicCameraController m_CameraController;
	Titan::Ref<Titan::Texture2D> m_CheckerboardTexture;
	ImFont* m_Font;
	std::chrono::time_point<std::chrono::steady_clock> startTimer;
	std::chrono::time_point<std::chrono::steady_clock> seconds_since_start;
	enum class GameState
	{
		Play = 0, Win = 1, GameOver = 2
	};

	GameState m_GameState = GameState::Play;
};