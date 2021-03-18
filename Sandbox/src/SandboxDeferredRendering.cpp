#include "SandboxDeferredRendering.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_POINT_LIGHTS 4

bool EnableSSAO = true;
bool EnableSkybox = false;
bool EnableBloom = false;

SandboxDeferredRendering::SandboxDeferredRendering()
	: Layer("SandboxDeferredRendering"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 1000.0f)
{
}

void SandboxDeferredRendering::OnAttach()
{
	m_DrawModel = std::make_shared<Titan::Model>("assets/meshes/sponza_crytek/sponza.obj");
	m_DrawModel2 = std::make_shared<Titan::Model>("assets/meshes/bunny.obj");

	m_DeferredRenderingPass.Init();

	// Point Lights
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		Titan::PointLight point_light;
		m_PointLights.push_back(point_light);
	}
}

void SandboxDeferredRendering::OnDetach()
{
}

void SandboxDeferredRendering::DrawSceneObjects(const std::shared_ptr<Titan::Shader>& shader)
{
	Titan::Renderer::BeginScene(m_CameraController.GetCamera());

	for (auto mesh : m_DrawModel->Meshes) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		Titan::Renderer::Submit(shader, mesh, m_DirLight, transform);
	}

	for (auto mesh : m_DrawModel2->Meshes) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.38f, 0.0f));
		Titan::Renderer::Submit(shader, mesh, m_DirLight, transform);
	}
	shader->Unbind();

	Titan::Renderer::EndScene();
}

void SandboxDeferredRendering::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	m_DirLight.UpdateViewProjection();

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	using std::placeholders::_1;
	std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObj = std::bind(&SandboxDeferredRendering::DrawSceneObjects, this, _1);

	// Geometry Pass
	Titan::DeferredRendering::GeometryPass(drawObj);

	// SSAO Pass
	if (EnableSSAO) {
		Titan::DeferredRendering::SSAOPass(m_CameraController.GetCamera());
	}

	// Lighting Pass
	Titan::DeferredRendering::DirectionalLightPass(m_CameraController.GetCamera(), m_DirLight, m_PointLights);

	// Draw Skybox
	if (EnableSkybox) {
		Titan::DeferredRendering::BeginSkyboxPass();
		Titan::Renderer::Submit(Titan::DeferredRendering::GetSkyboxShader(), m_DrawSkybox->GetMeshVertexArray());
		Titan::DeferredRendering::EndSkyboxPass();
	}

	// PostProcessing Pass
	if(EnableBloom)
		Titan::DeferredRendering::PostProcessPass_Bloom();

}

void SandboxDeferredRendering::OnImGuiRender()
{
	//Camera and Lighting
	ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoCollapse);
	
	ImGui::Text("Camera Position x: %f y: %f z: %f", m_CameraController.GetCamera().GetPosition().x, m_CameraController.GetCamera().GetPosition().y, m_CameraController.GetCamera().GetPosition().z);
	//Temp
	ImGui::SliderFloat("Camera zNear", &m_CameraController.GetCamera().m_NearClip, 0.1f, 1000.0f);
	ImGui::SliderFloat("Camera zFar", &m_CameraController.GetCamera().m_FarClip, m_CameraController.GetCamera().m_NearClip, 1000.0f);
	
	ImGui::NewLine();

	ImGui::DragFloat3("Light Position", glm::value_ptr(m_DirLight.Position));
	ImGui::ColorEdit3("Light Color", glm::value_ptr(m_DirLight.Ambient));
	ImGui::DragFloat("Light Orthographic Left", &m_DirLight.orthoLeft);
	ImGui::DragFloat("Light Orthographic Right", &m_DirLight.orthoRight);
	ImGui::DragFloat("Light Orthographic Bottom", &m_DirLight.orthoButtom);
	ImGui::DragFloat("Light Orthographic Top", &m_DirLight.orthoTop);
	ImGui::DragFloat("Light zNear", &m_DirLight.zNear);
	ImGui::DragFloat("Light zFar", &m_DirLight.zFar);
	ImGui::NewLine();
	ImGui::End();

	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);
	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	
	for(int i = 0; i < Titan::DeferredRendering::DebugTextures.size(); ++i) {
		ImGui::Image((void*)Titan::DeferredRendering::DebugTextures[i]->GetTextureID(), ImVec2(x/2, y/2), ImVec2(0,1), ImVec2(1,0));
		if (i % 2 == 0) ImGui::SameLine();
	}
	ImGui::End();
}

void SandboxDeferredRendering::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}

