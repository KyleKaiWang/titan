#pragma once
#include "Scene.h"
#include "Renderer/Buffer.h"
#include "Renderer/Lighting.h"
#include "Renderer/Texture.h"

namespace Titan {

	struct SSAOParameters
	{
		int kernelSize = 64;
		float radius = 0.5;
		float bias = 0.025;
	};

	class DeferredRendering
	{
	public:
		static void Init();
		static void SetFrameBuffer(uint32_t width, uint32_t height);
		static void BeginGeometryPass();
		static void EndGeometryPass();
		static void BeginShadowPass();
		static void EndShadowPass();
		static void BlurShadowPass();
		static void SSAOPass(PerspectiveCamera& camera);
		static void SSAOBlurPass();
		static void DirectionalLightPass(PerspectiveCamera& camera, Light& light);
		static void PointLightPass(PerspectiveCamera& camera, std::vector<PointLight>& pointLights);
		static void MomentShadowMapPass(PerspectiveCamera& camera, Light& light);
		static void BeginSkyboxPass();
		static void EndSkyboxPass();

		static const std::shared_ptr<Shader>& GetGeometryShader();
		static const std::shared_ptr<Shader>& GetShadowMapShader();
		static const std::shared_ptr<Shader>& GetLightingShader();
		static const std::shared_ptr<Shader>& GetSkyboxShader();

		static const std::shared_ptr<Texture2D>& GetShadowMapTexture();

		static SSAOParameters SSAOParams;
		static std::vector<std::shared_ptr<Texture>> GBufferTextures;
		static std::vector<std::shared_ptr<Texture>> DebugTextures;
		static unsigned int NoiseTexture;
		static bool EnableAO;
	};
}