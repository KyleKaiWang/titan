#pragma once

#include "FrameBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Titan {

	class MomentShadowRendering
	{
	public:
		MomentShadowRendering();
		~MomentShadowRendering();

		void Init(uint32_t width, uint32_t height);
		void BeginShadowPass(std::function<void()> drawQuad);
		void EndShadowPass();

		void BeginBlurShadowPass(std::function<void()> drawQuad);
		void EndBlurShadowPass();
		
		std::shared_ptr<Shader> ShadowMapShader;
		std::shared_ptr<Shader> BlurShadowMapShader;
		std::shared_ptr<Framebuffer> ShadowMapFBO;
		std::vector<std::shared_ptr<Framebuffer>> BlurShadowMapFBOs;
		std::shared_ptr<Texture2D> g_ShadowMap;
		std::vector<std::shared_ptr<Texture2D>> g_BlurShadowMaps;
	};
	
}