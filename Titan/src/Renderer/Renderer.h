#pragma once

#include "RenderCommand.h"

#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Texture.h"
#include "Lighting.h"
#include "Mesh/Mesh.h"

namespace Titan {


	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(PerspectiveCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Light& light, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh, Light& light, const glm::mat4& transform);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
		};

		static SceneData* s_SceneData;
	};
}