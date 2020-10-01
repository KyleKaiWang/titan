#include "tpch.h"
#include "ClusteredRenderer.h"
#include "Application.h"
#include "../ImGui/imgui.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


namespace Titan {

	ClusteredRenderer::ClusteredRenderer()
	{
	}

	ClusteredRenderer::~ClusteredRenderer()
	{
	}

	void ClusteredRenderer::Init(Camera& camera, std::vector<PointLight>& pointLights)
	{
		// Camera info
		camData.zNear = static_cast<PerspectiveCamera&>(camera).GetNear();
		camData.zFar= static_cast<PerspectiveCamera&>(camera).GetFar();
		auto& window = Application::Get().GetWindow();
		
		BuildAABBGridCompShader = Shader::Create("shaders/ClusterGrid.comp");
		CullLightsCompShader = Shader::Create("shaders/ClusterCullLight.comp");
		DepthPrePassShader = Shader::Create("shaders/DepthPass.vs", "shaders/DepthPass.fs");
		PBRClusteredShader = Shader::Create("shaders/Clustered_PBR.vs", "shaders/Clustered_PBR.fs");
		PointLightShadowShader = Shader::Create("shaders/PointLightShadow.vs", "shaders/PointLightShadow.gs", "shaders/PointLightShadow.fs");
		DirLightShdowShader = Shader::Create("shaders/ShadowMapPass.vs", "shaders/ShadowMapPass.fs");

		{
			glCreateBuffers(1, &AABBvolumeGridSSBO);
			glNamedBufferData(AABBvolumeGridSSBO, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AABBvolumeGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}

		{
			glCreateBuffers(1, &screenToViewSSBO);
			
			sizeX = (unsigned int)std::ceilf(window.GetWidth() / (float)gridSizeX);
			
			screenToView.inverseProjectionMat = glm::inverse(camera.GetProjectionMatrix());
			screenToView.tileSizes[0] = gridSizeX;
			screenToView.tileSizes[1] = gridSizeY;
			screenToView.tileSizes[2] = gridSizeZ;
			screenToView.tileSizes[3] = sizeX;
			screenToView.screenWidth = window.GetWidth();
			screenToView.screenHeight = window.GetHeight();

			screenToView.sliceScalingFactor = (float)gridSizeZ / std::log2f(camData.zFar / camData.zNear);
			screenToView.sliceBiasFactor = -((float)gridSizeZ * std::log2f(camData.zNear) / std::log2f(camData.zFar / camData.zNear));

			glNamedBufferData(screenToViewSSBO, sizeof(struct ScreenToView), &screenToView, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screenToViewSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}

		{
			glCreateBuffers(1, &lightSSBO);
			glNamedBufferData(lightSSBO, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

			GLint bufMask = GL_READ_WRITE;

			struct GPULight* lights = (struct GPULight*)glMapNamedBuffer(lightSSBO, bufMask);
			PointLight light;
			numLights = pointLights.size();
			for (unsigned int i = 0; i < numLights; ++i) {
				
				light = pointLights[i];
				lights[i].position = glm::vec4(light.Position, 1.0f);
				lights[i].color = glm::vec4(light.Ambient, 1.0f);
				lights[i].enabled = 1;
				lights[i].intensity = 1.0f;
				lights[i].range = 65.0f;
			}
			glUnmapNamedBuffer(lightSSBO);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}

		{
			unsigned int totalNumLights = numClusters * maxLightsPerTile;
			glCreateBuffers(1, &lightIndexListSSBO);

			glNamedBufferData(lightIndexListSSBO, totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexListSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		{
			glCreateBuffers(1, &lightGridSSBO);

			glNamedBufferData(lightGridSSBO, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}

		{
			glCreateBuffers(1, &lightIndexGlobalCountSSBO);

			glNamedBufferData(lightIndexGlobalCountSSBO, sizeof(unsigned int), NULL, GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCountSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}

		{
			BuildAABBGridCompShader->Bind();
			BuildAABBGridCompShader->SetFloat("zNear", camData.zNear);
			BuildAABBGridCompShader->SetFloat("zFar", camData.zFar);
			glDispatchCompute(gridSizeX, gridSizeY, gridSizeZ);

			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}
		
		SetFrameBuffer(window.GetWidth(), window.GetHeight(), pointLights);
	}

	void ClusteredRenderer::SetFrameBuffer(uint32_t width, uint32_t height, const std::vector<PointLight>& pointLights)
	{
		TextureDesc texDesc;
		texDesc.Width = width;
		texDesc.Height = height;

		FramebufferDesc desc;
		desc.Width = width;
		desc.Height = height;
		
		// MultiSample
		{
			texDesc.Target = GL_TEXTURE_2D_MULTISAMPLE;
			texDesc.Format = GL_RGBA16F;
			texDesc.MipLevels = 0;
			texDesc.Samples = 4;
			desc.TexDesc = texDesc;
			desc.nrColorAttachment = 1;
			desc.Depth = true;
			desc.DepthFormat = GL_DEPTH_COMPONENT32F;
		
			MultiSampledFBO = Framebuffer::Create(desc);
			GBufferTextures.push_back(MultiSampledFBO->GetColorAttachment());
		}
		
		// Directional Light Shadow
		{
			texDesc.Target = GL_TEXTURE_2D;
			texDesc.Format = GL_RGBA16F;
			texDesc.MipLevels = 0;
			texDesc.Samples = 1;
		
			desc.TexDesc = texDesc;
			desc.nrColorAttachment = 0;
			desc.Depth = true;
			desc.DepthFormat = GL_DEPTH_COMPONENT32F;
		
			DirLightShadowFBO = Framebuffer::Create(desc);
			GBufferTextures.push_back(DirLightShadowFBO->GetDepthAttachment());
		}

		// Point Lights
		{
			uint32_t shadowRes = 1024;
			
			size_t size = pointLights.size();
			PointLightShadowFBOs.resize(size);
			for (int i = 0; i < size; ++i) {
				uint32_t id;
				glCreateFramebuffers(1, &id);
				auto texCube = TextureCube::Create(shadowRes, shadowRes);
				glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, texCube->GetTextureID(), 0);
				glNamedFramebufferDrawBuffer(id, GL_NONE);
				glNamedFramebufferReadBuffer(id, GL_NONE);
				
				PointLightShadowFBOs[i] = id;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}

		// Resolve
		{
			texDesc.Format = GL_RGBA16F;
			texDesc.MipLevels = 0;
			texDesc.Samples = 1;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			desc.nrColorAttachment = 1;
			desc.TexDesc = texDesc;
			desc.Depth = true;
			desc.DepthFormat = GL_DEPTH_COMPONENT32F;
		
			ResolveFBO = Framebuffer::Create(desc);
			GBufferTextures.push_back(ResolveFBO->GetColorAttachment());
		}

		// Load & convert equirectangular environment map to a cubemap texture
		uint32_t tsize = 1024;
		std::shared_ptr<TextureCube> envTexUnfiltered = TextureCube::Create(tsize, tsize, GL_RGBA16F);
		{
			std::shared_ptr<Shader> equirectEnvMapShader = Shader::Create("shaders/EquirectEnvMap.comp");
			std::shared_ptr<Texture2D> envTexEquirect = Texture2D::Create("assets/textures/Newport_Loft_Ref.hdr");
			equirectEnvMapShader->Bind();
			envTexEquirect->Bind();
			glBindImageTexture(0, envTexUnfiltered->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(envTexUnfiltered->GetWidth() / 32, envTexUnfiltered->GetHeight() / 32, 6);
			equirectEnvMapShader->Unbind();
		}
		glGenerateTextureMipmap(envTexUnfiltered->GetTextureID());
		
		// Compute pre-filtered specular environment map.
		{
			std::shared_ptr<Shader> specularEnvMapShader = Shader::Create("shaders/SpecularEnvMap.comp");
		
			EnvTexture = TextureCube::Create(tsize, tsize, GL_RGBA16F);
		
			// Copy 0th mipmap level into destination environment map.
			glCopyImageSubData(envTexUnfiltered->GetTextureID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				EnvTexture->GetTextureID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				EnvTexture->GetWidth(), EnvTexture->GetHeight(), 6);
			specularEnvMapShader->Bind();
			envTexUnfiltered->Bind();
		
			// Pre-filter rest of the mip chain.
			const float deltaRoughness = 1.0f / glm::max(float(EnvTexture->GetLevels() - 1), 1.0f);
			for (int level = 1, size = tsize / 2; level <= EnvTexture->GetLevels(); ++level, size /= 2)
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, EnvTexture->GetTextureID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(specularEnvMapShader->GetShaderID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			specularEnvMapShader->Unbind();
		}
		
		// Compute diffuse irradiance cubemap.
		{
			std::shared_ptr<Shader> IrradianceMapShader = Shader::Create("shaders/IrradianceMap.comp");
			uint32_t tsize = 32;
			IrradianceTexture = TextureCube::Create(tsize, tsize, GL_RGBA16F, 1);
		
			IrradianceMapShader->Bind();
			glBindImageTexture(0, IrradianceTexture->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(IrradianceTexture->GetWidth() / 32, IrradianceTexture->GetHeight() / 32, 6);
			IrradianceMapShader->Unbind();
		}
		
		// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
		{
			std::shared_ptr<Shader> SpecularBRDFShader = Shader::Create("shaders/SpecularBRDF.comp");
		
			uint32_t tsize = 256;
			TextureDesc desc;
			desc.Width = tsize;
			desc.Height = tsize;
			desc.Target = GL_TEXTURE_2D;
			desc.Format = GL_RG16F;
			desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			BRDFLUTTexture = Texture2D::Create(desc);
		
			SpecularBRDFShader->Bind();
			glBindImageTexture(0, BRDFLUTTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(BRDFLUTTexture->GetWidth() / 32, BRDFLUTTexture->GetHeight() / 32, 1);
			SpecularBRDFShader->Unbind();
		}
	}

	void ClusteredRenderer::MultisampledDepthPrePass(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject)
	{
		MultiSampledFBO->Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawObject(DepthPrePassShader);
		MultiSampledFBO->Unbind();
	}

	void ClusteredRenderer::LightCullingPass(PerspectiveCamera& camera)
	{
		CullLightsCompShader->Bind();
		CullLightsCompShader->SetMat4("u_ViewMatrix", camera.GetViewMatrix());
		glDispatchCompute(1, 1, 6);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void ClusteredRenderer::PointLightPass(std::vector<PointLight>& pointLights, std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject)
	{
		PointLightShadowShader->Bind();
		for (uint32_t i = 0; i < pointLights.size(); ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, PointLightShadowFBOs[i]);
			glClear(GL_DEPTH_BUFFER_BIT);
			PointLightShadowShader->SetFloat3("u_LightPos", pointLights[i].Position);
			PointLightShadowShader->SetFloat("u_FarPlane", pointLights[i].zNear);
			for (unsigned int face = 0; face < 6; ++face) {
				glm::mat4 lightMatrix, M;
				glm::mat4 shadowProj = pointLights[i].ProjectionMatrix;

				std::string number = std::to_string(face);
				lightMatrix = shadowProj * pointLights[i].LookAtPerFace[face];
				PointLightShadowShader->SetMat4(("u_ShadowMatrices[" + number + "]").c_str(), lightMatrix);
			}
			drawObject(PointLightShadowShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		PointLightShadowShader->Unbind();
	}

	void ClusteredRenderer::DirectionalLightPass(const DirectionalLight& dirLight, std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject)
	{
		DirLightShdowShader->Bind();
		DirLightShadowFBO->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		DirLightShdowShader->SetMat4("u_LightSpaceMatrix",dirLight.ProjectionMatrix);
		drawObject(DirLightShdowShader);
		DirLightShadowFBO->Unbind();
		DirLightShdowShader->Unbind();
	}

	void ClusteredRenderer::Render(std::function<void(const std::shared_ptr<Titan::Shader>&)> drawObject)
	{
		glDepthFunc(GL_LEQUAL);
		DirLightShadowFBO->GetDepthAttachment()->Bind(5);
		PBRClusteredShader->Bind();
		PBRClusteredShader->SetInt("g_ShadowMap", 5);
		EnvTexture->Bind(6);
		PBRClusteredShader->SetInt("g_PrefilterMap", 6);
		IrradianceTexture->Bind(7);
		PBRClusteredShader->SetInt("g_IrradianceMap", 7);
		BRDFLUTTexture->Bind(8);
		PBRClusteredShader->SetInt("g_SpecularBRDF_LUT", 8);

		PBRClusteredShader->SetInt("b_Slices", EnableZslices);
		PBRClusteredShader->SetInt("b_IBL", EnableIBL);
		drawObject(PBRClusteredShader);
	}

	void ClusteredRenderer::BlitPass()
	{
		MultiSampledFBO->BlitTo(ResolveFBO.get(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ClusteredRenderer::PostProcessPass()
	{
	}

	void ClusteredRenderer::RenderDebug()
	{
		ImGui::Checkbox("zSlices", &EnableZslices);
		ImGui::Checkbox("IBL", &EnableIBL);
	}
}