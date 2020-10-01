#pragma once
#include "../Scene/Scene.h"
#include "Buffer.h"
#include "Lighting.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "DOF.h"
#include "PostProcessBloom.h"
#include "PostProcessTonemap.h"
#include "RendererSSAO.h"
#include "MomentShadowRendering.h"

namespace Titan {

	class ClusteredRenderer
	{
	public:
		ClusteredRenderer();
		~ClusteredRenderer();
		void Init(Camera& camera, std::vector<PointLight>& pointLights);
		void SetFrameBuffer(uint32_t width, uint32_t height, const std::vector<PointLight>& pointLights);
		
		// Passes
		void PointLightPass(std::vector<PointLight>& pointLights, std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		void DirectionalLightPass(const DirectionalLight& dirLight, std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		void LightCullingPass(PerspectiveCamera& camera);
		void MultisampledDepthPrePass(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		void Render(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject);
		void BlitPass();
		void PostProcessPass();

		const std::vector<std::shared_ptr<Texture>>& GetGBufferTextures() const { return GBufferTextures; }

		struct CameraData {
			float zFar;
			float zNear;
		} camData;

		struct VolumeTileAABB {
			glm::vec4 minPoint;
			glm::vec4 maxPoint;
		} frustrum;

		struct ScreenToView {
			glm::mat4 inverseProjectionMat;
			unsigned int tileSizes[4];
			unsigned int screenWidth;
			unsigned int screenHeight;
			float sliceScalingFactor;
			float sliceBiasFactor;
		}screenToView;

		const unsigned int gridSizeX = 16;
		const unsigned int gridSizeY = 9;
		const unsigned int gridSizeZ = 24;
		const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ;
		unsigned int sizeX, sizeY;

		unsigned int numLights;
		const unsigned int maxLights = 1000;
		const unsigned int maxLightsPerTile = 50;

		unsigned int AABBvolumeGridSSBO, screenToViewSSBO;
		unsigned int lightSSBO, lightIndexListSSBO, lightGridSSBO, lightIndexGlobalCountSSBO;
		
		// Special case - Using framebuffer id
		std::vector<uint32_t> PointLightShadowFBOs;

		std::shared_ptr<Framebuffer> MultiSampledFBO;
		std::shared_ptr<Framebuffer> ResolveFBO;
		std::shared_ptr<Framebuffer> DirLightShadowFBO;
		std::shared_ptr<Shader> BuildAABBGridCompShader;
		std::shared_ptr<Shader> CullLightsCompShader;
		std::shared_ptr<Shader> DepthPrePassShader;
		std::shared_ptr<Shader> PointLightShadowShader;
		std::shared_ptr<Shader> PBRClusteredShader;
		std::shared_ptr<Shader> DirLightShdowShader;

		std::shared_ptr<MomentShadowRendering> m_MSMRendering;
		std::shared_ptr<RendererSSAO> m_RendererSSAO;
		SSAOParameters SSAOParams;

		std::vector<std::shared_ptr<Texture>> GBufferTextures;

		//IBL
		std::shared_ptr<TextureCube> EnvTexture;
		std::shared_ptr<TextureCube> IrradianceTexture;
		std::shared_ptr<Texture2D> BRDFLUTTexture;

		//Debug
		bool EnableZslices = false;
		bool EnableIBL = true;

		void RenderDebug();
	};
}