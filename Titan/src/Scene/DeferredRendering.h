#pragma once
#include "Scene.h"
#include "Renderer/Buffer.h"
#include "Renderer/Lighting.h"

namespace Titan {

	class DeferredRendering
	{
	public:
		static void Init();
		static void SetFrameBuffer(uint32_t width, uint32_t height);
		static void BeginGeometryPass();
		static void EndGeometryPass();
		static void BeginShadowPass();
		static void EndShadowPass();
		static void DirectionalLightPass(PerspectiveCamera& camera, Light& light);
		static void PointLightPass(PerspectiveCamera& camera, std::vector<PointLight>& pointLights);
		static void MomentShadowMapPass(PerspectiveCamera& camera, Light& light);

		static const std::shared_ptr<Shader>& GetGeometryShader();
		static const std::shared_ptr<Shader>& GetShadowMapShader();
		static const std::shared_ptr<Shader>& GetPBRShader();
		static const std::shared_ptr<Texture2D>& GetShadowMapTexture();

		static std::vector<std::shared_ptr<Texture2D>> GBufferTextures;

	};
}