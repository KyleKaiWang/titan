#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace Titan {


	class Renderer2D
	{
	public:
		static void Init();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Shutdown();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
		static void DrawQuad(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<IndexBuffer>& indexBuffer, const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
		
		static void DrawDebugQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	};
}