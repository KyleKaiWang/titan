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
	
	//Test Scene System
	//---------------------------------------------------------------------------------
	//m_Scene.reset(new Titan::Scene());
	//m_Scene->SetCameraController(&m_CameraController);

	//Make a SceneNode 
	//Titan::SceneNode* sceneNode = m_Scene->MakeSceneNode();

	//Setting Material
	//m_PhongMaterial = std::make_shared<Titan::PhongMaterial>();
	//sceneNode->SetMaterial(std::static_pointer_cast<Titan::Material>(m_PhongMaterial));

	//Setting Mesh
	//std::shared_ptr<Titan::Mesh> mesh = std::make_shared<Titan::Cube>();
	//sceneNode->SetMesh(mesh);

	//Add SceneNode to the Scene
	//m_Scene->AddSceneNode(sceneNode);
	//m_Scene->InitScene();

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

	//PhongTexture
	m_Shader->SetFloat3("Ka", glm::vec3(0.9f, 0.5f, 0.3f));
	m_Shader->SetFloat3("Kd", glm::vec3(0.9f, 0.5f, 0.3f));
	m_Shader->SetFloat3("Ks", glm::vec3(0.05f, 0.05f, 0.05f));
	m_Shader->SetFloat("u_Shininess", 1.0f);
	m_Shader->SetInt("u_Texture", 0);
	Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray());
	//Titan::Renderer::Submit(m_Shader, m_DrawMesh2->GetMeshVertexArray(), );
	m_Shader->Unbind();
	Titan::Renderer::EndScene();
	
	Titan::SceneDeferred::DeferredPass(m_CameraController.GetCamera());
}

void SandboxDeferred::OnImGuiRender()
{
	ImGui::SetNextWindowSize(ImVec2(1280, 720));
	ImGui::Begin("GBuffer");
	for (auto tex : Titan::SceneDeferred::GBufferTextures) {
		ImGui::Image((void*)tex->GetTextureID(), ImVec2((float)tex->GetWidth(), (float)tex->GetHeight()), ImVec2(0,1), ImVec2(1,0));
	}
	ImGui::End();
}

void SandboxDeferred::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
