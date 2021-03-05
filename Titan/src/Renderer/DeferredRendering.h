#pragma once
#include "../Scene/Scene.h"
#include "Buffer.h"
#include "Lighting.h"
#include "Texture.h"
#include "DOF.h"
#include "PostProcessBloom.h"
#include "PostProcessTonemap.h"
#include "RendererSSAO.h"

namespace Titan {

	class DeferredRendering
	{
	public:
		static void Init();
		static void SetFrameBuffer(uint32_t width, uint32_t height);
		static void DrawQuad();
		static void GeometryPass(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		static void ShadowPass(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		static void BlurShadowPass();
		static void SSAOPass(PerspectiveCamera& camera);
		static void DirectionalLightPass(PerspectiveCamera& camera, Light& light, bool enableSSAO = true);
		static void PointLightPass(PerspectiveCamera& camera, std::vector<PointLight>& pointLights);
		static void MomentShadowMapPass(PerspectiveCamera& camera, Light& light);
		static void BeginSkyboxPass();
		static void EndSkyboxPass();
		static void PostProcessPass_Bloom();

		static RendererSSAO Renderer_SSAO;
		static PostProcessBloom Renderer_Bloom;
		static PostProcessTonemap Renderer_Tonemap;

		// Shaders
		static const std::shared_ptr<Shader>& GetGeometryShader();
		static const std::shared_ptr<Shader>& GetShadowMapShader();
		static const std::shared_ptr<Shader>& GetLightingShader();
		static const std::shared_ptr<Shader>& GetSkyboxShader();

		// Textures
		static const std::shared_ptr<Texture2D>& GetShadowMapTexture();
		static std::vector<std::shared_ptr<Texture>> GBufferTextures;
		static std::vector<std::shared_ptr<Texture>> DebugTextures;
	};
}