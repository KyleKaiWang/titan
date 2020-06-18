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

	m_DrawMesh2 = static_cast<Titan::Mesh*>(new Titan::Plane(100.0f, 100.0f, 100, 100));
	m_DrawMesh2->Init();

	m_DrawMesh3 = static_cast<Titan::Mesh*>(new Titan::Sphere());
	m_DrawMesh3->Init();

	//Test Basic Object Rendering
	m_Texture = Titan::Texture2D::Create("assets/textures/Brick.png");
	m_SimpleQuad = Titan::Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	m_SimpleQuad->SetData(&whiteTextureData, sizeof(uint32_t));

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

	//Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	//Titan::RenderCommand::Clear();

	auto renderScene = [&](const std::shared_ptr<Titan::Shader>& shader, bool shadowPass){
		Titan::Renderer::BeginScene(m_CameraController.GetCamera());
		shader->Bind();
		if (!shadowPass) {
			m_Texture->Bind();
			shader->SetInt("u_Texture", 0);
			shader->SetFloat4("u_Color", glm::vec4(1.0f));
		}
		for (int i = 1; i <= 5; ++i) {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 1.0f, 0.0f));
			Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
			transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 3.0f, 0.0f));
			Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
			transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 5.0f, 0.0f));
			Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
		}
		if (!shadowPass) {
			m_SimpleQuad->Bind();
			shader->SetInt("u_Texture", 0);
			shader->SetFloat4("u_Color", glm::vec4(0.3,0.3,0.3,1.0));
		}
		glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.0f, 0.0f));
		Titan::Renderer::Submit(shader, m_DrawMesh2->GetMeshVertexArray(), m_Light, transform2);
		if (!shadowPass) shader->SetFloat4("u_Color", glm::vec4(1.0));
		glm::mat4 transform3 = glm::translate(glm::mat4(1.0f), m_Light.Position) * glm::scale(glm::mat4(1.0f), { 0.1, 0.1, 0.1f });
		Titan::Renderer::Submit(shader, m_DrawMesh3->GetMeshVertexArray(), m_Light, transform3);
		shader->Unbind();
		Titan::Renderer::EndScene();
	};

	//Geometry Pass
	Titan::SceneDeferred::BeginGeometryPass();
	renderScene(Titan::SceneDeferred::GetGeometryShader(), false);
	Titan::SceneDeferred::EndGeometryPass();

	//Shadow Map Pass
	Titan::SceneDeferred::BeginShadowPass();
	renderScene(Titan::SceneDeferred::GetShadowMapShader(), true);
	Titan::SceneDeferred::EndShadowPass();

	//Directional Light Pass
	Titan::SceneDeferred::DirectionalLightPass(m_CameraController.GetCamera(), m_Light);

	//Point Light Pass
	//Titan::SceneDeferred::PointLightPass(m_CameraController.GetCamera());
}

void SandboxDeferred::OnImGuiRender()
{
	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);

	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::Image((void*)Titan::SceneDeferred::GetShadowMapTexture()->GetTextureID(), ImVec2(x / 2, y / 2), ImVec2(0, 1), ImVec2(1, 0));
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
