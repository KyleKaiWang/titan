#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameSystem/GameObjectFactory.h"
#include "GameSystem/GameObjectManager.h"
#include "GameSystem/Physics.h"

GameObjectFactory* gpGameObjectFactory;
GameObjectManager* gpGameObjectManager;
Physics* gpPhysics;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f/720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = Titan::Texture2D::Create("assets/textures/checkerboard.png");

	//Game System
	gpGameObjectFactory = new GameObjectFactory();
	gpGameObjectManager = new GameObjectManager();
	gpPhysics = new Physics();

	//Start Level
	gpGameObjectFactory->LoadLevel("../Sandbox/assets/data/Level1.txt");
}

void Sandbox2D::OnDetach()
{
	//Delete Game System
	delete gpGameObjectFactory;
	delete gpGameObjectManager;
	delete gpPhysics;
}

void Sandbox2D::OnUpdate(Titan::Timestep ts)
{
	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	m_CameraController.OnUpdate(ts);
	gpPhysics->Update(ts);

	Titan::Renderer2D::BeginScene(m_CameraController.GetCamera());
	for (GameObject* pGo : gpGameObjectManager->m_GameObjects)
		pGo->Update();

	Titan::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	//ImGui::Begin("Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	//ImGui::End();
}

void Sandbox2D::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
