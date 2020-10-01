#include "SandboxDeferredRendering.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SandboxDeferredRendering::SandboxDeferredRendering()
	: Layer("SandboxDeferredRendering"), m_CameraController(45.0f, (float)1280/(float)720, 0.01f, 1000.0f)
{
}

void SandboxDeferredRendering::OnAttach()
{
	m_DrawModel = std::make_shared<Titan::Model>("assets/meshes/sponza_crytek/sponza.obj");
	m_DrawModel2 = std::make_shared<Titan::Model>("assets/meshes/bunny.obj");

	m_ClusteredRenderer.Init(m_CameraController.GetCamera(), m_PointLights);

	using std::placeholders::_1;
	std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObj = std::bind(&SandboxDeferredRendering::DrawSceneObjects, this, _1);
	m_ClusteredRenderer.PointLightPass(m_PointLights, drawObj);
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
	
	// Directional Shadow
	m_ClusteredRenderer.DirectionalLightPass(m_DirLight, drawObj);

	// PreDepthPass
	m_ClusteredRenderer.MultisampledDepthPrePass(drawObj);

	// Light Calling
	m_ClusteredRenderer.LightCullingPass(m_CameraController.GetCamera());

	// Scene
	m_ClusteredRenderer.Render(drawObj);

	//m_ClusteredRenderer.BlitPass();

	// Post Processing
	m_ClusteredRenderer.PostProcessPass();
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
	ImGui::DragFloat("Orthographic Left", &m_DirLight.orthoLeft);
	ImGui::DragFloat("Orthographic Right", &m_DirLight.orthoRight);
	ImGui::DragFloat("Orthographic Bottom", &m_DirLight.orthoButtom);
	ImGui::DragFloat("Orthographic Top", &m_DirLight.orthoTop);
	ImGui::DragFloat("zNear", &m_DirLight.zNear);
	ImGui::DragFloat("zFar", &m_DirLight.zFar);
	ImGui::NewLine();
	m_ClusteredRenderer.RenderDebug();
	ImGui::End();

	auto& window = Titan::Application::Get().GetWindow();
	unsigned int x, y;
	window.GetDesktopResolution(x, y);
	ImGui::SetNextWindowSize(ImVec2(x, y));
	ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_NoCollapse);
	
	for(int i = 0; i < m_ClusteredRenderer.GetGBufferTextures().size(); ++i) {
		ImGui::Image((void*)m_ClusteredRenderer.GetGBufferTextures()[i]->GetTextureID(), ImVec2(x/2, y/2), ImVec2(0,1), ImVec2(1,0));
		if (i % 2 == 0) ImGui::SameLine();
	}
	ImGui::End();
}

void SandboxDeferredRendering::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}

