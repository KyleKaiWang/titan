#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLDebugDraw.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f/720.0f)
{
}

void Sandbox2D::OnAttach()
{
	Titan::Renderer2D::Init();
	m_Texture = m_Texture = Titan::Texture2D::Create("assets/textures/Brick.png");

	Titan::OpenGLDebugDraw::Init();

	//SpriteTestCase
	//-----------------------------------------------------------------------------------------
	m_Sprite = std::make_shared<Titan::Sprite>("assets/textures/SpriteSheetExample.png", 8, 4);
	m_Sprite->SetAnimSequence(24, 8);
	m_Sprite->SetAnimLooping(true);
	//-----------------------------------------------------------------------------------------

	//Particle
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.2f, m_Particle.SizeVariation = 0.1f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 0.0f, 0.0f, 0.0f };
	m_Particle.Position = { 0.0f, -0.7f, 0.0f };
	m_Particle.Speed = { 1.0f, 1.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
	Titan::OpenGLDebugDraw::End();
}

void Sandbox2D::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	Titan::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//Basic 2D Shape
	Titan::Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f, m_Texture);
	Titan::Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	
	//Sprite Rendering
	//m_Sprite->Update(ts.GetMilliSeconds());
	//m_Sprite->Draw();

	//Debug Draw 
	Titan::Renderer2D::DrawDebugQuad(glm::vec3(0.0f), glm::vec2(0.2f), 0.0f, glm::vec4(1.0f));
	Titan::OpenGLDebugDraw::DrawLine(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec3(0.0f), glm::vec4(1.0f), 3.0f);
	Titan::OpenGLDebugDraw::DrawCircle(glm::vec3(0.0f), glm::vec3(0.0f), 0.2f, glm::vec3(1.0f));

	Titan::Renderer2D::EndScene();


	//Particle Rendering
	if (m_ParticleActive)
	{
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}
	
	m_ParticleSystem.Update(ts);
	m_ParticleSystem.Render();
	
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::Begin("Settings");
	ImGui::Checkbox("ParticleActive", &m_ParticleActive);
	ImGui::SliderFloat("LifeTime", &m_Particle.LifeTime, 0.0f, 1.0f);
	ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("End Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::SliderFloat("SizeBegin", &m_Particle.SizeBegin, 0.0f, 1.0f);
	//ImGui::DragFloat("SizeBegin", &m_Particle.SizeBegin);
	ImGui::SliderFloat("SizeEnd", &m_Particle.SizeEnd, 0.0f, 1.0f);
	//ImGui::DragFloat("SizeEnd", &m_Particle.SizeEnd);
	ImGui::SliderFloat("SizeVariation", &m_Particle.SizeVariation, 0.0f, 1.0f);
	//ImGui::DragFloat("SizeVariation", &m_Particle.SizeVariation);
	ImGui::SliderFloat3("Velocity", glm::value_ptr(m_Particle.Velocity), -1.0f, 1.0f);
	//ImGui::DragFloat3("Velocity", glm::value_ptr(m_Particle.Velocity));
	ImGui::SliderFloat3("VelocityVariation", glm::value_ptr(m_Particle.VelocityVariation), -1.0f, 1.0f);
	//ImGui::DragFloat3("VelocityVariation", glm::value_ptr(m_Particle.VelocityVariation));
	ImGui::SliderFloat2("Speed", glm::value_ptr(m_Particle.Speed), 0.0f, 1.0f);
	ImGui::End();
}

void Sandbox2D::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
