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
		static void BindingFramebuffer(PerspectiveCamera& camera);
		static void DeferredPass(PerspectiveCamera& camera);
		static void LightingPass(PerspectiveCamera& camera);

		static std::vector<std::shared_ptr<Texture2D>> GBufferTextures;
	};
}