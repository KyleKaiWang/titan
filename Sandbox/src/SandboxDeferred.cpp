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

	std::shared_ptr<Titan::PhongMaterial> PhongMaterial4 = std::make_shared<Titan::PhongMaterial>();
	PhongMaterial4->GetPhong()->Ambient = glm::vec3(1.0);
	PhongMaterial4->GetPhong()->Diffuse = glm::vec3(1.0);

	//PBR Textures
	std::shared_ptr<Titan::PBRTextures> pbrTexs = std::make_shared<Titan::PBRTextures>();

	pbrTexs->Albedo = Titan::Texture2D::Create("assets/textures/brushed-metal_A.png");
	pbrTexs->Normal = Titan::Texture2D::Create("assets/textures/brushed-metal_N.png");
	pbrTexs->Metallic = Titan::Texture2D::Create("assets/textures/brushed-metal_M.png");
	pbrTexs->Roughness = Titan::Texture2D::Create("assets/textures/brushed-metal_R.png");

	std::shared_ptr<Titan::PBRMaterial> pbrMaterial = std::make_shared<Titan::PBRMaterial>();
	pbrMaterial->SetPBRTextures(pbrTexs);
	m_PBRMats.push_back(pbrMaterial);

	std::shared_ptr<Titan::PBRTextures> pbrTexs2 = std::make_shared<Titan::PBRTextures>();
	pbrTexs2->Albedo = Titan::Texture2D::Create("assets/textures/Metal003_2K_A.jpg");
	pbrTexs2->Normal = Titan::Texture2D::Create("assets/textures/Metal003_2K_N.jpg");
	pbrTexs2->Metallic = Titan::Texture2D::Create("assets/textures/Metal003_2K_M.jpg");
	pbrTexs2->Roughness = Titan::Texture2D::Create("assets/textures/Metal003_2K_R.jpg");
	std::shared_ptr<Titan::PBRMaterial> pbrMaterial2 = std::make_shared<Titan::PBRMaterial>();
	pbrMaterial2->SetPBRTextures(pbrTexs2);
	m_PBRMats.push_back(pbrMaterial2);

	std::shared_ptr<Titan::PBRTextures> pbrTexs3 = std::make_shared<Titan::PBRTextures>();
	pbrTexs3->Albedo = Titan::Texture2D::Create("assets/textures/lightgold_A.png");
	pbrTexs3->Normal = Titan::Texture2D::Create("assets/textures/lightgold_N.png");
	pbrTexs3->Metallic = Titan::Texture2D::Create("assets/textures/lightgold_M.png");
	pbrTexs3->Roughness = Titan::Texture2D::Create("assets/textures/lightgold_R.png");

	std::shared_ptr<Titan::PBRMaterial> pbrMaterial3 = std::make_shared<Titan::PBRMaterial>();
	pbrMaterial3->SetPBRTextures(pbrTexs3);
	m_PBRMats.push_back(pbrMaterial3);

	std::shared_ptr<Titan::PBRTextures> pbrTexs4 = std::make_shared<Titan::PBRTextures>();
	pbrTexs4->Albedo = Titan::Texture2D::Create("assets/textures/Cerberus_A.tga");
	pbrTexs4->Normal = Titan::Texture2D::Create("assets/textures/Cerberus_N.tga");
	pbrTexs4->Metallic = Titan::Texture2D::Create("assets/textures/Cerberus_M.tga");
	pbrTexs4->Roughness = Titan::Texture2D::Create("assets/textures/Cerberus_R.tga");

	std::shared_ptr<Titan::PBRMaterial> pbrMaterial4 = std::make_shared<Titan::PBRMaterial>();
	pbrMaterial4->SetPBRTextures(pbrTexs4);
	m_PBRMats.push_back(pbrMaterial4);

	//Test Basic Mesh System
	m_DrawModel = std::make_shared<Titan::Model>("assets/meshes/sponza.obj");
	for (auto mesh : m_DrawModel->Meshes)
		mesh->SetMeshMaterial(pbrMaterial2);

	m_DrawSphere = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>());
	m_DrawSphere->Init();
	m_DrawSphere->SetMeshMaterial(pbrMaterial2);

	m_DrawCube = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Cube>());
	m_DrawCube->Init();
	m_DrawCube->SetMeshMaterial(pbrMaterial);

	m_DrawPlane = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Plane>(Titan::Plane(15.0f, 15.0f, 15, 15)));
	m_DrawPlane->Init();
	m_DrawPlane->SetMeshMaterial(PhongMaterial2);

	m_DrawLight = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Sphere>());
	m_DrawLight->Init();
	m_DrawLight->SetMeshMaterial(PhongMaterial4);

	m_DrawSkybox = static_cast<std::shared_ptr<Titan::Mesh>>(std::make_shared<Titan::Skybox>(1000.0f));
	m_DrawSkybox->Init();

	//Setting Deferred Shading
	//---------------------------------------------------------------------------------
	Titan::DeferredRendering::Init();

	auto& window = Titan::Application::Get().GetWindow();
	Titan::TextureDesc texDesc;
	texDesc.Width = window.GetWidth();
	texDesc.Height = window.GetHeight();
	texDesc.Format = 0x8814;
	texDesc.MipLevels = 0;
	
	Titan::FramebufferDesc desc;
	desc.Width = window.GetWidth();
	desc.Height = window.GetHeight();
	desc.nrColorAttachment = 1;
	desc.TexDesc = texDesc;
	m_FBO = Titan::Framebuffer::Create(desc);
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

		for (auto mesh : m_DrawModel->Meshes) {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
			Titan::Renderer::Submit(shader, mesh, m_Light, transform);
		}

		m_DrawSphere->SetMeshMaterial(m_PBRMats[0]);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, -2.0f));
		Titan::Renderer::Submit(shader, m_DrawSphere, m_Light, transform);
		//
		//m_DrawSphere->SetMeshMaterial(m_PBRMats[1]);
		//transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		//Titan::Renderer::Submit(shader, m_DrawSphere, m_Light, transform);
		//
		//m_DrawSphere->SetMeshMaterial(m_PBRMats[2]);
		//transform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		//Titan::Renderer::Submit(shader, m_DrawSphere, m_Light, transform);
		
		shader->Unbind();		

		Titan::Renderer::EndScene();
	};

	//Geometry Pass
	Titan::DeferredRendering::BeginGeometryPass();
	renderScene(Titan::DeferredRendering::GetGeometryShader());
	Titan::DeferredRendering::EndGeometryPass();	

	Titan::DeferredRendering::SSAOPass(m_CameraController.GetCamera());
	Titan::DeferredRendering::SSAOBlurPass();

	//Lighting Pass
	Titan::DeferredRendering::DirectionalLightPass(m_CameraController.GetCamera(), m_Light);

	//Draw Skybox
	Titan::DeferredRendering::BeginSkyboxPass();
	Titan::Renderer::Submit(Titan::DeferredRendering::GetSkyboxShader(), m_DrawSkybox->GetMeshVertexArray());
	Titan::DeferredRendering::EndSkyboxPass();
}

void SandboxDeferred::OnImGuiRender()
{
	//Camera and Lighting
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

	//SSAO Parameters
	ImGui::Begin("SSAO", 0, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginChild("SSAO");
	ImGui::DragInt("Kernel Size", &Titan::DeferredRendering::SSAOParams.kernelSize);
	ImGui::DragFloat("Radius", &Titan::DeferredRendering::SSAOParams.radius);
	ImGui::DragFloat("bias", &Titan::DeferredRendering::SSAOParams.bias);
	ImGui::EndChild();
	ImGui::End();

	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);
	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	
	//G-Buffers
	for(int i = 0; i < Titan::DeferredRendering::GBufferTextures.size(); ++i) {
		ImGui::Image((void*)Titan::DeferredRendering::GBufferTextures[i]->GetTextureID(), ImVec2(x/2, y/2), ImVec2(0,1), ImVec2(1,0));
		if (i % 2 == 0) ImGui::SameLine();
	}

	//Noise Texture for SSAO
	ImGui::Image((void*)Titan::DeferredRendering::NoiseTexture, ImVec2(x / 2, y / 2), ImVec2(0, 1), ImVec2(1, 0));
	
	//SSAO
	for (int i = 0; i < Titan::DeferredRendering::DebugTextures.size(); ++i) {
		ImGui::Image((void*)Titan::DeferredRendering::DebugTextures[i]->GetTextureID(), ImVec2(x / 2, y / 2), ImVec2(0, 1), ImVec2(1, 0));
		if (i % 2 == 0) ImGui::SameLine();
	}
	ImGui::End();
}

void SandboxDeferred::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
