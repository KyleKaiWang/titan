#include "Sandbox3D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox3D::Sandbox3D()
	: Layer("Sandbox3D"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 100.0f)
{
}

void Sandbox3D::OnAttach()
{
	m_DrawMesh = static_cast<Titan::Mesh*>(new Titan::Cube());
	m_DrawMesh->Init();

	m_Texture = Titan::Texture2D::Create("assets/textures/Brick.png");
	m_Shader = Titan::Shader::Create("shaders/TextureShader.vs", "shaders/TextureShader.fs");
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_Shader)->Bind();
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_Shader)->UploadUniformInt("u_Texture", 0);
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_Shader)->SetFloat4("u_Color", glm::vec4(1.0f));
}

void Sandbox3D::OnDetach()
{

}

void Sandbox3D::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	Titan::Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();
	Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray());
	Titan::Renderer::EndScene();
}

void Sandbox3D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void Sandbox3D::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
