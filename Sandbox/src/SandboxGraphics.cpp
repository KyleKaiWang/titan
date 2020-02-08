#include "SandboxGraphics.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Scene/Scene.h"
#include "Renderer/Material.h"
#include "Renderer/PerspectiveCamera.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SandboxGraphics::SandboxGraphics()
	: Layer("Sandbox3D"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 100.0f)
{
}

void SandboxGraphics::OnAttach()
{
	//Test Basic Mesh System
	//m_DrawMesh = static_cast<Titan::Mesh*>(new Titan::Sphere());
	//m_DrawMesh->Init();

	//Test Basic Object Rendering
	//m_Texture = Titan::Texture2D::Create("assets/textures/Brick.png");
	//m_Shader = Titan::Shader::Create("shaders/CubeMap.vs", "shaders/CubeMap.fs");
	
	//Test Scene System
	//---------------------------------------------------------------------------------
	m_Scene.reset(new Titan::Scene());
	m_Scene->SetCameraController(&m_CameraController);

	//Make a SceneNode 
	Titan::SceneNode* sceneNode = m_Scene->MakeSceneNode();

	//Setting Material
	std::shared_ptr<Titan::Material> material = std::make_shared<Titan::PhongMaterial>();
	sceneNode->SetMaterial(std::static_pointer_cast<Titan::Material>(material));

	//Setting Mesh
	std::shared_ptr<Titan::Mesh> mesh = std::make_shared<Titan::Cube>();
	sceneNode->SetMesh(mesh);

	//Add SceneNode to the Scene
	m_Scene->AddSceneNode(sceneNode);
	m_Scene->InitScene();
	//---------------------------------------------------------------------------------

	//Test Skybox
	//---------------------------------------------------------------------------------
	m_DrawSkybox = static_cast<Titan::Mesh*>(new Titan::Skybox());
	m_DrawSkybox->Init();
	
	std::vector<std::string> cubeMapFaces = {
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};
	m_TextureCube = Titan::TextureCube::Create(cubeMapFaces);
	m_SkyboxShader = Titan::Shader::Create("shaders/Skybox.vs", "shaders/Skybox.fs");
	//---------------------------------------------------------------------------------
}

void SandboxGraphics::OnDetach()
{

}

void SandboxGraphics::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	//SceneNode Test
	m_Scene->Update(ts);
	m_Scene->PreRender();
	m_PhongMaterial->BindPhongParam(m_Ambient, m_Diffuse, m_Specular, m_Shininess);
	m_Scene->Render();
	m_Scene->PostRender();

	//Titan::Renderer::BeginScene(m_CameraController.GetCamera());
	//m_Texture->Bind();
	//m_Shader->Bind();
	//glm::mat4 viewMatrix =  m_CameraController.GetCamera().GetViewMatrix();
	//glm::mat4 mv = viewMatrix * glm::mat4(1.0f);
	//CubeMap
	//m_TextureCube->Bind();
	//m_Shader->SetFloat3("cameraPos", m_CameraController.GetCamera().GetPosition());
	//m_Shader->SetInt("u_Skybox", 0);

	//PhongTexture
	//m_Shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	//m_Shader->SetFloat3("u_LightIntensity", glm::vec3(1.0f));
	//m_Shader->SetFloat3("u_LightPosition", viewMatrix * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	//m_Shader->SetFloat3("Ka", glm::vec3(0.9f, 0.5f, 0.3f));
	//m_Shader->SetFloat3("Kd", glm::vec3(0.9f, 0.5f, 0.3f));
	//m_Shader->SetFloat3("Ks", glm::vec3(0.05f, 0.05f, 0.05f));
	//m_Shader->SetFloat("u_Shininess", 1.0f);
	//m_Shader->SetInt("u_Texture", 0);
	//m_Shader->SetFloat4("u_Color", glm::vec4(1.0f));

	//Phong
	//m_Shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	//m_Shader->SetFloat3("u_LightPosition", viewMatrix * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	//m_Shader->SetFloat3("Kd", glm::vec3(0.9f, 0.5f, 0.3f));
	//m_Shader->SetFloat3("Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	//m_Shader->SetFloat3("Ka", glm::vec3(0.9f, 0.5f, 0.3f));
	//m_Shader->SetFloat3("La", glm::vec3(0.4f, 0.4f, 0.4f));
	//m_Shader->SetFloat3("Ks", glm::vec3(0.8f, 0.8f, 0.8f));
	//m_Shader->SetFloat3("Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	//m_Shader->SetFloat("u_Shininess", 100.0f);

	//Titan::Renderer::Submit(m_Shader, m_DrawMesh->GetMeshVertexArray());

	//Skybox Rendering
	m_TextureCube->Bind();
	m_SkyboxShader->Bind();
	m_SkyboxShader->SetInt("u_Skybox", 0);
	Titan::Renderer::Submit(m_SkyboxShader, m_DrawSkybox->GetMeshVertexArray());

	Titan::Renderer::EndScene();
}

void SandboxGraphics::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void SandboxGraphics::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
