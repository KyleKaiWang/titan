#include "SandboxDeferred.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Scene/Scene.h"
#include "Scene/SceneDeferred.h"
#include "Renderer/Material.h"
#include "Renderer/PerspectiveCamera.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SandboxDeferred::SandboxDeferred()
	: Layer("SandboxDeferred"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 100.0f)
{
}

void SandboxDeferred::OnAttach()
{
	//Test Basic Mesh System
	m_DrawMesh = static_cast<Titan::Mesh*>(new Titan::Cube());
	m_DrawMesh->Init();

	m_DrawMesh2 = static_cast<Titan::Mesh*>(new Titan::Cube());
	m_DrawMesh2->Init();

	//Test Basic Object Rendering
	m_Texture = Titan::Texture2D::Create("assets/textures/Brick.png");
	m_Shader = Titan::Shader::Create("shaders/Geometry.vs", "shaders/Geometry.fs");

	//Setting Deferred Shading
	//---------------------------------------------------------------------------------
	Titan::SceneDeferred::Init();
}

void SandboxDeferred::OnDetach()
{

}

void SandboxDeferred::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	//Deferred Shading Pass1
	Titan::SceneDeferred::BindingFramebuffer(m_CameraController.GetCamera());

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	Titan::Renderer::BeginScene(m_CameraController.GetCamera());
	m_Shader->Bind();
	m_Texture->Bind();

	m_Shader->SetInt("u_Texture", 0);
	for (int i = 1; i <= 5; ++i) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 1.0f, 0.0f));
		Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray(), transform);
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 3.0f, 0.0f));
		Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray(), transform);
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 5.0f, 0.0f));
		Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray(), transform);
	}
	m_Shader->Unbind();
	Titan::Renderer::EndScene();
	
	Titan::SceneDeferred::DeferredPass(m_CameraController.GetCamera());
}

void SandboxDeferred::OnImGuiRender()
{
	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);

	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	for(int i = 0; i < Titan::SceneDeferred::GBufferTextures.size(); ++i) {
		ImGui::Image((void*)Titan::SceneDeferred::GBufferTextures[i]->GetTextureID(), ImVec2(x/2, y/2), ImVec2(0,1), ImVec2(1,0));
		if (i % 2 == 0) ImGui::SameLine();
	}
	ImGui::End();
}

void SandboxDeferred::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
