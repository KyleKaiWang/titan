#include "SandboxDeferred.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Scene/Scene.h"
#include "Scene/SceneDeferred.h"
#include "Renderer/Material.h"
#include "Renderer/PerspectiveCamera.h"
#include "Renderer/Material.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SandboxDeferred::SandboxDeferred()
	: Layer("SandboxDeferred"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 100.0f)
{
}

void SandboxDeferred::OnAttach()
{
	//PBR Material
	//std::shared_ptr<Titan::PBRMaterial> pbrMaterial = std::make_shared<Titan::PBRMaterial>();
	//pbrMaterial->GetPBRTextures()->Albedo = Titan::Texture2D::Create("assets/textures/Marble006_2K_Albedo.jpg");
	//pbrMaterial->GetPBRTextures()->Normal = Titan::Texture2D::Create("assets/textures/Marble006_2K_Normal.jpg");
	//pbrMaterial->GetPBRTextures()->Metallic = Titan::Texture2D::Create("assets/textures/Marble006_2K_Metallic.jpg");
	//pbrMaterial->GetPBRTextures()->Roughness = Titan::Texture2D::Create("assets/textures/Marble006_2K_Roughness.jpg");

	//Phong Material
	std::shared_ptr<Titan::PhongMaterial> PhongMaterial1 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial1->GetPhong()->Diffuse = glm::vec3(0.7);

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial2 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial2->GetPhong()->Ambient = glm::vec3(0.001);
	PhongMaterial2->GetPhong()->Diffuse = glm::vec3(3.0);
	//PhongMaterial2->GetTexture() = Titan::Texture2D::Create("assets/textures/Brick.png");

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial3 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial3->GetPhong()->Ambient = glm::vec3(1.0, 1.0, 1.0);

	//Test Basic Mesh System
	m_DrawMesh = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>());
	m_DrawMesh->Init();
	m_DrawMesh->SetMeshMaterial(PhongMaterial1);
	//m_DrawMesh->SetMeshMaterial(pbrMaterial);

	m_DrawMesh2 = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Plane>(Titan::Plane(100.0f, 100.0f, 100, 100)));
	m_DrawMesh2->Init();
	m_DrawMesh2->SetMeshMaterial(PhongMaterial2);

	m_DrawMesh3 = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>(Titan::Sphere()));
	m_DrawMesh3->Init();
	m_DrawMesh3->SetMeshMaterial(PhongMaterial3);

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
		for (int i = 1; i <= 5; ++i) {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 1.0f, 0.0f));
			//Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
			Titan::Renderer::Submit(shader, m_DrawMesh, m_Light, transform);
			transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 3.0f, 0.0f));
			//Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
			Titan::Renderer::Submit(shader, m_DrawMesh, m_Light, transform);
			transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * i, 5.0f, 0.0f));
			//Titan::Renderer::Submit(shader, m_DrawMesh->GetMeshVertexArray(), m_Light, transform);
			Titan::Renderer::Submit(shader, m_DrawMesh, m_Light, transform);
		}
		glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.0f, 0.0f));
		Titan::Renderer::Submit(shader, m_DrawMesh2, m_Light, transform2);
		
		//glm::mat4 transform3 = glm::translate(glm::mat4(1.0f), m_Light.Position) * glm::scale(glm::mat4(1.0f), { 0.1, 0.1, 0.1f });
		//Titan::Renderer::Submit(shader, m_DrawMesh3, m_Light, transform3);
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

	//MSM Pass
	//Titan::SceneDeferred::MomentShadowMapPass(m_CameraController.GetCamera(), m_Light);

	//Directional Light Pass
	Titan::SceneDeferred::DirectionalLightPass(m_CameraController.GetCamera(), m_Light);
	//renderScene(Titan::SceneDeferred::GetPBRShader(), false);

	//Point Light Pass
	//Titan::SceneDeferred::PointLightPass(m_CameraController.GetCamera());
}

void SandboxDeferred::OnImGuiRender()
{
	ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Camera Position x: %f y: %f z: %f", m_CameraController.GetCamera().GetPosition().x, m_CameraController.GetCamera().GetPosition().y, m_CameraController.GetCamera().GetPosition().z);
	//ImGui::Text("LightPosition x: %f y: %f z: %f", m_CameraController.GetCamera().GetPosition().x, m_CameraController.GetCamera().GetPosition().y, m_CameraController.GetCamera().GetPosition().z);
	ImGui::End();
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
