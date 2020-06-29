#include "tpch.h"
#include "Renderer.h"
#include "Application.h"
#include "Lighting.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		auto& window = Application::Get().GetWindow();
		camera.SetPerspectiveMatrix(45.0f, (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 100.0f);
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
		s_SceneData->ViewMatrix = camera.GetViewMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Shutdown()
	{
		delete s_SceneData;
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_ProjectionMatrix", s_SceneData->ProjectionMatrix);
		shader->SetMat4("u_ViewMatrix", s_SceneData->ViewMatrix);
		shader->SetMat4("u_Model", transform);
		
		glm::mat4 mv = s_SceneData->ViewMatrix * transform;
		shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Light& light, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_ProjectionMatrix", s_SceneData->ProjectionMatrix);
		shader->SetMat4("u_ViewMatrix", s_SceneData->ViewMatrix);
		shader->SetMat4("u_Model", transform);

		glm::mat4 mv = s_SceneData->ViewMatrix * transform;
		shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));

		//Lighting
		light.ShaderBinding(shader);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<Mesh>& mesh, Light& light, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_ProjectionMatrix", s_SceneData->ProjectionMatrix);
		shader->SetMat4("u_ViewMatrix", s_SceneData->ViewMatrix);
		shader->SetMat4("u_Model", transform);

		glm::mat4 mv = s_SceneData->ViewMatrix * transform;
		shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));

		//Lighting
		light.ShaderBinding(shader);

		//PBR Texture binding
		mesh->GetMeshMaterial()->Bind(shader);

		mesh->GetMeshVertexArray()->Bind();
		RenderCommand::DrawIndexed(mesh->GetMeshVertexArray());
		mesh->GetMeshVertexArray()->Unbind();
	}
}
