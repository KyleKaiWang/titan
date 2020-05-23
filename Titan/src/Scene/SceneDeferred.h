#pragma once
#include "Scene.h"
#include "Renderer/Buffer.h"

namespace Titan {

	class SceneDeferred
	{
	public:
		static void Init();
		static void Update(float t);
		static void Render(PerspectiveCamera& camera);
		static void CreateGBufferTexture(std::shared_ptr<Texture2D>& tex, uint32_t slot, uint32_t format);
		static void SetFrameBuffer(uint32_t width, uint32_t height);
		static void Pass1(PerspectiveCamera& camera);
		static void Pass2(PerspectiveCamera& camera);

		static std::vector<std::shared_ptr<Texture2D>> GBufferTextures;
	//private:
	//	std::shared_ptr<VertexBuffer> m_VertexBuffer;
	//	std::shared_ptr<VertexArray> m_VertexArray;
	//	std::shared_ptr<Shader> m_Shader;
	//	uint32_t m_RendererID;
	//	uint32_t m_DepthBuffer;
	//
	//	std::shared_ptr<Texture2D> m_NormalTexture;
	//	std::shared_ptr<Texture2D> m_PosTexture;
	//	std::shared_ptr<Texture2D> m_ColorTexture;
	};
}