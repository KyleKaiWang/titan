#include "SandboxDeferred.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Scene/Scene.h"
#include "Scene/DeferredRendering.h"
#include "Renderer/Material.h"
#include "Renderer/Camera.h"
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

	//Phong Material
	std::shared_ptr<Titan::PhongMaterial> PhongMaterial1 = std::make_shared<Titan::PhongMaterial>();

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial2 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial2->GetPhong()->Ambient = glm::vec3(0.25);
	PhongMaterial2->GetPhong()->Diffuse = glm::vec3(0.5);
	PhongMaterial2->GetPhong()->Specular = glm::vec3(0.001);

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial3 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial3->GetPhong()->Ambient = glm::vec3(0.25);
	PhongMaterial3->GetPhong()->Diffuse = glm::vec3(0.8);
	PhongMaterial3->GetPhong()->Specular = glm::vec3(0.5);
	PhongMaterial3->GetTexture() = Titan::Texture2D::Create("assets/textures/T_Metal01_BC.png");

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial4 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial4->GetPhong()->Ambient = glm::vec3(1.0);
	PhongMaterial4->GetPhong()->Diffuse = glm::vec3(1.0);

	//Test Basic Mesh System
	m_DrawSphere = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>());
	m_DrawSphere->Init();
	m_DrawSphere->SetMeshMaterial(PhongMaterial1);

	m_DrawPlane = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Plane>(Titan::Plane(15.0f, 15.0f, 15, 15)));
	m_DrawPlane->Init();
	m_DrawPlane->SetMeshMaterial(PhongMaterial2);

	m_DrawCube = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Cube>());
	m_DrawCube->Init();
	m_DrawCube->SetMeshMaterial(PhongMaterial3);

	m_DrawLight = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>());
	m_DrawLight->Init();
	m_DrawLight->SetMeshMaterial(PhongMaterial4);

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
	m_Light.UpdateViewProjection();

	auto renderScene = [&](const std::shared_ptr<Titan::Shader>& shader){
		Titan::Renderer::BeginScene(m_CameraController.GetCamera());
		//for (int i = 1; i <= NUM_MESHS; i+=2) {
		//	for (int j = 1; j <= NUM_MESHS; ++j) {
		//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * j, 1.0f * i, 0.0f));
		//		Titan::Renderer::Submit(shader, m_DrawMesh, m_Light, transform);
		//	}
		//}
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		Titan::Renderer::Submit(shader, m_DrawSphere, m_Light, transform);

		transform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.5f, 5.0f));
		Titan::Renderer::Submit(shader, m_DrawCube, m_Light, transform);

		transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 2.0f));
		Titan::Renderer::Submit(shader, m_DrawPlane, m_Light, transform);
		
		transform = glm::translate(glm::mat4(1.0f), m_Light.Position) * glm::scale(glm::mat4(1.0f), { 0.1, 0.1, 0.1f });
		Titan::Renderer::Submit(shader, m_DrawLight, m_Light, transform);
		
		shader->Unbind();
		Titan::Renderer::EndScene();
	};

	//Geometry Pass
	Titan::DeferredRendering::BeginGeometryPass();
	renderScene(Titan::DeferredRendering::GetGeometryShader());
	Titan::DeferredRendering::EndGeometryPass();

	//Shadow Map Pass
	Titan::DeferredRendering::BeginShadowPass();

	Titan::Renderer::BeginScene(m_CameraController.GetCamera());
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	Titan::Renderer::Submit(Titan::DeferredRendering::GetShadowMapShader(), m_DrawSphere, m_Light, transform);

	transform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.5f, 5.0f));
	Titan::Renderer::Submit(Titan::DeferredRendering::GetShadowMapShader(), m_DrawCube, m_Light, transform);

	transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 2.0f));
	Titan::Renderer::Submit(Titan::DeferredRendering::GetShadowMapShader(), m_DrawPlane, m_Light, transform);

	Titan::DeferredRendering::GetShadowMapShader()->Unbind();
	Titan::Renderer::EndScene();

	Titan::DeferredRendering::EndShadowPass();

	//Blur Pass
	Titan::DeferredRendering::BlurShadowPass();

	//MSM Pass
	Titan::DeferredRendering::MomentShadowMapPass(m_CameraController.GetCamera(), m_Light);

	//Point Light Pass
	//Titan::DeferredRendering::PointLightPass(m_CameraController.GetCamera(), m_PointLights);
}

void SandboxDeferred::OnImGuiRender()
{
	ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Camera Position x: %f y: %f z: %f", m_CameraController.GetCamera().GetPosition().x, m_CameraController.GetCamera().GetPosition().y, m_CameraController.GetCamera().GetPosition().z);
	ImGui::NewLine();
	ImGui::BeginChild("Light");
	ImGui::DragFloat3("Light Position", glm::value_ptr(m_Light.Position));
	ImGui::DragFloat("Orthographic Left", &m_Light.orthoLeft);
	ImGui::DragFloat("Orthographic Right", &m_Light.orthoRight);
	ImGui::DragFloat("Orthographic Bottom", &m_Light.orthoButtom);
	ImGui::DragFloat("Orthographic Top", &m_Light.orthoTop);
	ImGui::DragFloat("Orthographic Near", &m_Light.orthoNear);
	ImGui::DragFloat("Orthographic Far", &m_Light.orthoFar);
	ImGui::EndChild();
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
