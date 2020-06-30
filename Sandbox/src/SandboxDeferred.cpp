#include "SandboxDeferred.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Scene/Scene.h"
#include "Scene/DeferredRendering.h"
#include "Renderer/Material.h"
#include "Renderer/PerspectiveCamera.h"
#include "Renderer/Material.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int NUM_LIGHTS = 15;
const int NUM_MESHS = 10;

SandboxDeferred::SandboxDeferred()
	: Layer("SandboxDeferred"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 100.0f)
{
}

void SandboxDeferred::OnAttach()
{
	//Point Lights
	for (int i = 1; i <= NUM_LIGHTS; i+=4) {
		for (int j = 1; j <= NUM_LIGHTS; j+=2) {
			Titan::PointLight pLight;
			float xPos = j * 2.0f;
			float yPos = i * 2.0f;
			float zPos = 2.0f;
			pLight.Position = glm::vec3(xPos, yPos, zPos);

			glm::vec3 colorArr[] = { glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f) };
			pLight.Diffuse = colorArr[rand() % 3];

			m_PointLights.push_back(pLight);
		}
	}

	//PBR Material
	//std::shared_ptr<Titan::PBRMaterial> pbrMaterial = std::make_shared<Titan::PBRMaterial>();
	//pbrMaterial->GetPBRTextures()->Albedo = Titan::Texture2D::Create("assets/textures/Marble006_2K_Albedo.jpg");
	//pbrMaterial->GetPBRTextures()->Normal = Titan::Texture2D::Create("assets/textures/Marble006_2K_Normal.jpg");
	//pbrMaterial->GetPBRTextures()->Metallic = Titan::Texture2D::Create("assets/textures/Marble006_2K_Metallic.jpg");
	//pbrMaterial->GetPBRTextures()->Roughness = Titan::Texture2D::Create("assets/textures/Marble006_2K_Roughness.jpg");

	//Phong Material
	std::shared_ptr<Titan::PhongMaterial> PhongMaterial1 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial1->GetPhong()->Diffuse = glm::vec3(0.3);

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial2 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial2->GetPhong()->Ambient = glm::vec3(0.001);
	PhongMaterial2->GetPhong()->Diffuse = glm::vec3(3.0);
	//PhongMaterial2->GetTexture() = Titan::Texture2D::Create("assets/textures/Brick.png");

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial3 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial3->GetPhong()->Ambient = glm::vec3(1.0, 1.0, 1.0);

	//Test Basic Mesh System
	m_DrawMesh = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Cube>());
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
	Titan::DeferredRendering::Init();
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
		for (int i = 1; i <= NUM_MESHS; i+=2) {
			for (int j = 1; j <= NUM_MESHS; ++j) {
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * j, 1.0f * i, 0.0f));
				Titan::Renderer::Submit(shader, m_DrawMesh, m_Light, transform);
			}
		}
		//glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -0.0f, 0.0f));
		//Titan::Renderer::Submit(shader, m_DrawMesh2, m_Light, transform2);
		
		//glm::mat4 transform3 = glm::translate(glm::mat4(1.0f), m_Light.Position) * glm::scale(glm::mat4(1.0f), { 0.1, 0.1, 0.1f });
		//Titan::Renderer::Submit(shader, m_DrawMesh3, m_Light, transform3);
		shader->Unbind();
		Titan::Renderer::EndScene();
	};

	//Geometry Pass
	Titan::DeferredRendering::BeginGeometryPass();
	renderScene(Titan::DeferredRendering::GetGeometryShader(), false);
	Titan::DeferredRendering::EndGeometryPass();

	//Shadow Map Pass
	Titan::DeferredRendering::BeginShadowPass();
	renderScene(Titan::DeferredRendering::GetShadowMapShader(), true);
	Titan::DeferredRendering::EndShadowPass();

	//MSM Pass
	//Titan::DeferredRendering::MomentShadowMapPass(m_CameraController.GetCamera(), m_Light);

	//Directional Light Pass
	Titan::DeferredRendering::DirectionalLightPass(m_CameraController.GetCamera(), m_Light);
	//renderScene(Titan::DeferredRendering::GetPBRShader(), false);

	//Point Light Pass
	Titan::DeferredRendering::PointLightPass(m_CameraController.GetCamera(), m_PointLights);
}

void SandboxDeferred::OnImGuiRender()
{
	ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Camera Position x: %f y: %f z: %f", m_CameraController.GetCamera().GetPosition().x, m_CameraController.GetCamera().GetPosition().y, m_CameraController.GetCamera().GetPosition().z);
	ImGui::End();
	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);

	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::Image((void*)Titan::DeferredRendering::GetShadowMapTexture()->GetTextureID(), ImVec2(x / 2, y / 2), ImVec2(0, 1), ImVec2(1, 0));
	for(int i = 0; i < Titan::DeferredRendering::GBufferTextures.size(); ++i) {
		ImGui::Image((void*)Titan::DeferredRendering::GBufferTextures[i]->GetTextureID(), ImVec2(x/2, y/2), ImVec2(0,1), ImVec2(1,0));
		if (i % 2 == 0) ImGui::SameLine();
	}
		
	ImGui::End();
}

void SandboxDeferred::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
