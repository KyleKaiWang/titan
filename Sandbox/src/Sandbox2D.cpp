#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#include "GameSystem/GameObjectManager.h"
#include "GameSystem/PhysicsManager.h"
#include "Bullet.h"
#include "SandboxGameObjectFactory.h"
#include "SandboxGameObjectFactory.h"
#include "Platform/OpenGL/OpenGLDebugDraw.h"

SandboxGameObjectFactory* gpSandboxGameObjectFactory;
GameObjectManager* gpGameObjectManager;
CollisionManager* gpCollisionManager;
PhysicsManager* gpPhysicsManager;
Titan::OpenGLDebugDraw* gpOpenGLDebugDraw;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(720.0f/1280.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = Titan::Texture2D::Create("assets/textures/checkerboard.png");

	//Game System
	gpSandboxGameObjectFactory = new SandboxGameObjectFactory();
	gpGameObjectManager = new GameObjectManager();
	gpCollisionManager = new CollisionManager();
	gpPhysicsManager = new PhysicsManager();
	gpOpenGLDebugDraw = new Titan::OpenGLDebugDraw();
	gpOpenGLDebugDraw->Init();
	//Start Level
	gpSandboxGameObjectFactory->LoadLevelFromFile("../Sandbox/assets/data/Level1.txt");

	ImGuiIO io = ImGui::GetIO();
	m_Font = io.Fonts->AddFontFromFileTTF("../Sandbox/assets/font/OpenSans-Regular.ttf", 120.0f);
	startTimer = std::chrono::high_resolution_clock::now();
}

void Sandbox2D::OnDetach()
{
	//Delete Game System
	delete gpSandboxGameObjectFactory;
	delete gpGameObjectManager;
	delete gpCollisionManager;
	delete gpPhysicsManager;
	delete gpOpenGLDebugDraw;
}

void Sandbox2D::OnUpdate(Titan::Timestep ts)
{
	GameObject* player = gpGameObjectManager->GetPlayer();
	if (!player)
	{
		m_GameState = GameState::GameOver;
		return;
	}

	if (GameObjectManager::s_AICount <= 0)
	{
		m_GameState = GameState::Win;
		return;
	}

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	m_CameraController.OnUpdate(ts);
	gpPhysicsManager->Update(ts);

	Titan::Renderer2D::BeginScene(m_CameraController.GetCamera());
	gpGameObjectManager->Update(ts);
	Titan::Renderer2D::EndScene();

	//check out of the bound
	for (auto gameObj : gpGameObjectManager->m_GameObjects)
	{
		if (gameObj->Destroyed)
			return;
		float width = GetCameraController().GetZoomLevel() * GetCameraController().GetAspectRatio();
		float height = GetCameraController().GetZoomLevel();
		
		//for Debug -------------------------------------
		//float X = gameObj->GetPosX();
		//float Y = gameObj->GetPosY();
		//bool a = gameObj->GetPosX() > width;
		//bool b = gameObj->GetPosY() > height;
		//bool c = gameObj->GetPosX() < (-1) * width;
		//bool d = gameObj->GetPosY() < (-1) * height;
		// ----------------------------------------------

		Bullet* bullet = dynamic_cast<Bullet*>(gameObj);
		if (bullet)
		{
			if (bullet->IsBounceBall)
			{
				if ((gameObj->GetPosX() > width) || (gameObj->GetPosX() < (-1) * width))
					bullet->ChangeDir('x');
				
				if ((gameObj->GetPosY() > height) || (gameObj->GetPosY() < (-1) * height))
					bullet->ChangeDir('y');
			}
			else if ((gameObj->GetPosX() > width) || (gameObj->GetPosX() < (-1) * width) ||
				(gameObj->GetPosY() > height) || (gameObj->GetPosY() < (-1) * height))
			{
				if (gameObj->GetGameID() != 1)
					gpGameObjectManager->DestroyGameObject(gameObj->GetGameID());
			}
		}
		else if ((gameObj->GetPosX() > width) || (gameObj->GetPosX() < (-1) * width) ||
			(gameObj->GetPosY() > height) || (gameObj->GetPosY() < (-1) * height))
		{
			if (gameObj->GetGameID() != 1)
				gpGameObjectManager->DestroyGameObject(gameObj->GetGameID());
		}
	}
}

void Sandbox2D::OnImGuiRender()
{
	//ImGui::Begin("Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	//ImGui::End();
	
	switch (m_GameState)
	{
		case GameState::Play:
		{
			seconds_since_start = std::chrono::high_resolution_clock::now();
			std::string timeStr = std::string("Timer: ") + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(seconds_since_start - startTimer).count());
			
			auto pos = ImGui::GetWindowPos();
			auto width = Titan::Application::Get().GetWindow().GetWidth();
			auto height = Titan::Application::Get().GetWindow().GetHeight();
			pos.x += width * 0.5f - 400.0f;
			pos.y += 900.0f;
			ImGui::GetForegroundDrawList()->AddText(m_Font, 24.0f, pos, 0xffffffff, timeStr.c_str());
			break;
		}
		case GameState::Win:
		{
			auto pos = ImGui::GetWindowPos();
			auto width = Titan::Application::Get().GetWindow().GetWidth();
			auto height = Titan::Application::Get().GetWindow().GetHeight();
			pos.x += width * 0.5f - 300.0f;
			pos.y += 50.0f;
			ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "You Won!");
			break;
		}
		case GameState::GameOver:
		{
			auto pos = ImGui::GetWindowPos();
			auto width = Titan::Application::Get().GetWindow().GetWidth();
			auto height = Titan::Application::Get().GetWindow().GetHeight();
			pos.x += width * 0.5f - 300.0f;
			pos.y += 50.0f;
			ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "You Failed!");
			break;
		}
	}
}

void Sandbox2D::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
