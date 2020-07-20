#include "tpch.h"
#include "DeferredRendering.h"
#include "../Renderer/Texture.h"
#include "../Renderer/FrameBuffer.h"
#include "Application.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

namespace Titan {

	SSAOParameters DeferredRendering::SSAOParams;
	std::vector<std::shared_ptr<Texture>> DeferredRendering::GBufferTextures;
	std::vector<std::shared_ptr<Texture>> DeferredRendering::DebugTextures;
	unsigned int DeferredRendering::NoiseTexture;
	bool DeferredRendering::EnableAO = false;
	std::vector<glm::vec3> ssaoKernel;
	std::vector<float> ssaoNoise;

	struct DeferredRenderingStorage
	{
		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<VertexArray> VertexArray;

		std::shared_ptr<Shader> GeometryShader;
		std::shared_ptr<Shader> ShadowMapShader;
		std::shared_ptr<Shader> BlurShadowMapShader;
		std::shared_ptr<Shader> LightingShader;
		std::shared_ptr<Shader> PointLightShader;
		std::shared_ptr<Shader> SkyboxShader;
		std::shared_ptr<Shader> SSAOShader;
		std::shared_ptr<Shader> SSAOBlurShader;

		std::shared_ptr<Framebuffer> GBufferFBO;
		std::shared_ptr<Framebuffer> ShadowMapFBO;
		std::vector<std::shared_ptr<Framebuffer>> BlurShadowMapFBOs;
		std::shared_ptr<Framebuffer> SSAOFBO;
		std::shared_ptr<Framebuffer> SSAOBlurFBO;

		std::shared_ptr<Texture2D> g_Posistion;
		std::shared_ptr<Texture2D> g_WorldNormal;
		std::shared_ptr<Texture2D> g_Albedo;
		std::shared_ptr<Texture2D> g_Normal;
		std::shared_ptr<Texture2D> g_MetallicRoughness;
		std::shared_ptr<Texture2D> g_ShadowMap;
		std::vector<std::shared_ptr<Texture2D>> g_BlurShadowMaps;
		std::shared_ptr<Texture2D> g_SSAO;
		std::shared_ptr<Texture2D> g_SSAOBlur;

		//IBL
		std::shared_ptr<TextureCube> EnvTexture;
		std::shared_ptr<TextureCube> IrradianceTexture;
		std::shared_ptr<Texture2D> BRDFLUTTexture;
	};

	static DeferredRenderingStorage* s_DeferredData;
	void DeferredRendering::Init()
	{
		s_DeferredData = new DeferredRenderingStorage();
		s_DeferredData->VertexArray = VertexArray::Create();

		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		s_DeferredData->VertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		s_DeferredData->VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_DeferredData->VertexArray->AddVertexBuffer(s_DeferredData->VertexBuffer);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->GeometryShader = Shader::Create("shaders/Geometry_PBR.vs", "shaders/Geometry_PBR.fs");
		s_DeferredData->ShadowMapShader = Shader::Create("shaders/ShadowMapPass.vs", "shaders/ShadowMapPass.fs");
		s_DeferredData->BlurShadowMapShader = Shader::Create("shaders/GaussianBlur.vs", "shaders/GaussianBlur.fs");
		s_DeferredData->LightingShader = Shader::Create("shaders/LightingPass_PBR.vs", "shaders/LightingPass_PBR.fs");
		s_DeferredData->PointLightShader = Shader::Create("shaders/PointLightPass.vs", "shaders/PointLightPass.fs");
		s_DeferredData->SkyboxShader = Shader::Create("shaders/Skybox.vs", "shaders/Skybox.fs");
		s_DeferredData->SSAOShader= Shader::Create("shaders/SSAO.vs", "shaders/SSAO.fs");
		s_DeferredData->SSAOBlurShader = Shader::Create("shaders/SSAOBlur.vs", "shaders/SSAOBlur.fs");
		
		auto& window = Application::Get().GetWindow();
		SetFrameBuffer(window.GetWidth(), window.GetHeight());
	}

	void DeferredRendering::SetFrameBuffer(uint32_t width, uint32_t height)
	{
		//GBuffers
		{
			TextureDesc texDesc;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Format = GL_RGBA32F;
			texDesc.MipLevels = 0;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
			
			FramebufferDesc desc;
			desc.Width = width;
			desc.Height = height;
			desc.nrColorAttachment = 5;
			desc.TexDesc = texDesc;
			
			s_DeferredData->GBufferFBO = Framebuffer::Create(desc);
			s_DeferredData->g_Posistion = s_DeferredData->GBufferFBO->GetColorAttachment(0);
			s_DeferredData->g_WorldNormal = s_DeferredData->GBufferFBO->GetColorAttachment(1);
			s_DeferredData->g_Albedo = s_DeferredData->GBufferFBO->GetColorAttachment(2);
			s_DeferredData->g_Normal = s_DeferredData->GBufferFBO->GetColorAttachment(3);
			s_DeferredData->g_MetallicRoughness = s_DeferredData->GBufferFBO->GetColorAttachment(4);

			GBufferTextures.push_back(s_DeferredData->g_Posistion);
			GBufferTextures.push_back(s_DeferredData->g_WorldNormal);
			GBufferTextures.push_back(s_DeferredData->g_Albedo);
			GBufferTextures.push_back(s_DeferredData->g_Normal);
			GBufferTextures.push_back(s_DeferredData->g_MetallicRoughness);
		}

		////Shadow Map
		//{
		//	TextureDesc texDesc;
		//	texDesc.Width = 1024;
		//	texDesc.Height = 1024;
		//	texDesc.Format = GL_RGBA32F;
		//	texDesc.MipLevels = 0;
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		//	
		//	FramebufferDesc desc;
		//	desc.Width = 1024;
		//	desc.Height = 1024;
		//	desc.nrColorAttachment = 1;
		//	desc.TexDesc = texDesc;
		//	s_DeferredData->ShadowMapFBO = Framebuffer::Create(desc);
		//	s_DeferredData->g_ShadowMap = s_DeferredData->ShadowMapFBO->GetColorAttachment(0);
		//	GBufferTextures.push_back(s_DeferredData->g_ShadowMap);
		//}
		//
		////Blur Shadow Map
		//{
		//	TextureDesc texDesc;
		//	texDesc.Width = 1024;
		//	texDesc.Height = 1024;
		//	texDesc.Format = GL_RGBA32F;
		//	texDesc.MipLevels = 0;
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		//	texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		//	
		//	FramebufferDesc desc;
		//	desc.Width = 1024;
		//	desc.Height = 1024;
		//	desc.nrColorAttachment = 1;
		//	desc.TexDesc = texDesc;
		//
		//	//Horizen Blur
		//	s_DeferredData->BlurShadowMapFBOs.push_back(Framebuffer::Create(desc));
		//	s_DeferredData->g_BlurShadowMaps.push_back(s_DeferredData->BlurShadowMapFBOs[0]->GetColorAttachment(0));
		//	GBufferTextures.push_back(s_DeferredData->g_BlurShadowMaps[0]);
		//	
		//	//Vertical Blur
		//	s_DeferredData->BlurShadowMapFBOs.push_back(Framebuffer::Create(desc));
		//	s_DeferredData->g_BlurShadowMaps.push_back(s_DeferredData->BlurShadowMapFBOs[1]->GetColorAttachment(0));
		//	GBufferTextures.push_back(s_DeferredData->g_BlurShadowMaps[1]);
		//}

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

			s_DeferredData->EnvTexture = TextureCube::Create(tsize, tsize, GL_RGBA16F);

			// Copy 0th mipmap level into destination environment map.
			glCopyImageSubData(envTexUnfiltered->GetTextureID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				s_DeferredData->EnvTexture->GetTextureID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				s_DeferredData->EnvTexture->GetWidth(), s_DeferredData->EnvTexture->GetHeight(), 6);
			specularEnvMapShader->Bind();
			envTexUnfiltered->Bind();

			// Pre-filter rest of the mip chain.
			const float deltaRoughness = 1.0f / glm::max(float(s_DeferredData->EnvTexture->GetLevels() - 1), 1.0f);
			for (int level = 1, size = tsize / 2; level <= s_DeferredData->EnvTexture->GetLevels(); ++level, size /= 2)
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, s_DeferredData->EnvTexture->GetTextureID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(specularEnvMapShader->GetShaderID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			specularEnvMapShader->Unbind();
			GBufferTextures.push_back(s_DeferredData->EnvTexture);
		}

		// Compute diffuse irradiance cubemap.
		{
			std::shared_ptr<Shader> IrradianceMapShader = Shader::Create("shaders/IrradianceMap.comp");
			uint32_t tsize = 32;
			s_DeferredData->IrradianceTexture = TextureCube::Create(tsize, tsize, GL_RGBA16F, 1);

			IrradianceMapShader->Bind();
			s_DeferredData->IrradianceTexture->Bind();
			glBindImageTexture(0, s_DeferredData->IrradianceTexture->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(s_DeferredData->IrradianceTexture->GetWidth() / 32, s_DeferredData->IrradianceTexture->GetHeight() / 32, 6);
			GBufferTextures.push_back(s_DeferredData->IrradianceTexture);
		}

		// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
		{
			std::shared_ptr<Shader> SpecularBRDFShader = Shader::Create("shaders/SpecularBRDF.comp");

			uint32_t tsize = 256;
			TextureDesc desc;
			desc.Width = tsize;
			desc.Height = tsize;
			desc.Format = GL_RG16F;
			desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			s_DeferredData->BRDFLUTTexture = Texture2D::Create(desc);

			SpecularBRDFShader->Bind();
			glBindImageTexture(0, s_DeferredData->BRDFLUTTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(s_DeferredData->BRDFLUTTexture->GetWidth() / 32, s_DeferredData->BRDFLUTTexture->GetHeight() / 32, 1);
			GBufferTextures.push_back(s_DeferredData->BRDFLUTTexture);
		}

		//SSAO
		{
			TextureDesc texDesc;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Format = GL_RGBA32F;
			texDesc.MipLevels = 0;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			
			FramebufferDesc desc;
			desc.Width = width;
			desc.Height = height;
			desc.nrColorAttachment = 1;
			desc.Depth = false;
			desc.TexDesc = texDesc;
			s_DeferredData->SSAOFBO = Framebuffer::Create(desc);
			s_DeferredData->g_SSAO = s_DeferredData->SSAOFBO->GetColorAttachment(0);
			DebugTextures.push_back(s_DeferredData->g_SSAO);
		}

		//SSAO Blur
		{
			TextureDesc texDesc;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Format = GL_RGBA32F;
			texDesc.MipLevels = 0;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			FramebufferDesc desc;
			desc.Width = width;
			desc.Height = height;
			desc.nrColorAttachment = 1;
			desc.Depth = false;
			desc.TexDesc = texDesc;
			s_DeferredData->SSAOBlurFBO = Framebuffer::Create(desc);
			s_DeferredData->g_SSAOBlur = s_DeferredData->SSAOBlurFBO->GetColorAttachment(0);
			DebugTextures.push_back(s_DeferredData->g_SSAOBlur);
		}
		

		// generate sample kernel
		// ----------------------
		std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
		std::default_random_engine generator;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0;

			// scale samples s.t. they're more aligned to center of kernel
			// lerp
			scale = 0.1f + scale * scale * (1.0f - 0.1f);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise.x);
			ssaoNoise.push_back(noise.y);
			ssaoNoise.push_back(noise.z);
		}
		
		glCreateTextures(GL_TEXTURE_2D, 1, &NoiseTexture);
		glTextureStorage2D(NoiseTexture, 1, GL_RGB16F, 4, 4);
		glTextureSubImage2D(NoiseTexture, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTextureParameteri(NoiseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(NoiseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(NoiseTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(NoiseTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
		std::cout << glGetError() << std::endl;
	}

	void DeferredRendering::BeginGeometryPass()
	{
		s_DeferredData->GBufferFBO->Bind();
		
		std::vector<unsigned int> attachments;
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glNamedFramebufferDrawBuffers(s_DeferredData->GBufferFBO->GetFramebufferID(), GBufferTextures.size(), &attachments[0]);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DeferredRendering::EndGeometryPass()
	{
		s_DeferredData->GBufferFBO->Unbind();
	}

	void DeferredRendering::BeginShadowPass()
	{
		s_DeferredData->ShadowMapFBO->Bind();
		glViewport(0, 0, s_DeferredData->ShadowMapFBO->GetFramebufferSize().first, s_DeferredData->ShadowMapFBO->GetFramebufferSize().second);
		glNamedFramebufferDrawBuffer(s_DeferredData->ShadowMapFBO->GetFramebufferID(), GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
	}

	void DeferredRendering::EndShadowPass()
	{
		s_DeferredData->ShadowMapFBO->Unbind();
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DeferredRendering::BlurShadowPass()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, s_DeferredData->BlurShadowMapFBOs[0]->GetFramebufferSize().first, s_DeferredData->ShadowMapFBO->GetFramebufferSize().second);

		// --------------------Pixel Shader----------------------
		s_DeferredData->BlurShadowMapShader->Bind();
		s_DeferredData->VertexArray->Bind();
		
		//Horizen Pass
		s_DeferredData->BlurShadowMapFBOs[0]->Bind();
		s_DeferredData->BlurShadowMapShader->Bind();
		s_DeferredData->BlurShadowMapShader->SetInt("u_Horizontal", 1);
		
		//Original Shadow Map
		s_DeferredData->g_ShadowMap->Bind(0);
		s_DeferredData->BlurShadowMapShader->SetInt("u_SceneTexture", 0);
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->BlurShadowMapShader->Unbind();
		s_DeferredData->BlurShadowMapFBOs[0]->Unbind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Vertical Pass
		s_DeferredData->BlurShadowMapFBOs[1]->Bind();
		s_DeferredData->BlurShadowMapShader->Bind();
		s_DeferredData->BlurShadowMapShader->SetInt("u_Horizontal", 0);
		
		//Shadow Map after blur
		s_DeferredData->g_BlurShadowMaps[0]->Bind(0);
		s_DeferredData->BlurShadowMapShader->SetInt("u_SceneTexture", 0);
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->BlurShadowMapShader->Unbind();
		s_DeferredData->BlurShadowMapFBOs[1]->Unbind();
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DeferredRendering::SSAOPass(PerspectiveCamera& camera)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		s_DeferredData->SSAOFBO->Bind();
		s_DeferredData->SSAOShader->Bind();
		for (unsigned int i = 0; i < 64; ++i)
			s_DeferredData->SSAOShader->SetFloat3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		s_DeferredData->SSAOShader->SetMat4("u_ViewMatrix", camera.GetViewMatrix());
		s_DeferredData->SSAOShader->SetMat4("u_Projection", camera.GetProjectionMatrix());
		s_DeferredData->SSAOShader->SetInt("screenWidth", 1280);
		s_DeferredData->SSAOShader->SetInt("screenHeight", 720);
		s_DeferredData->SSAOShader->SetInt("kernelSize", SSAOParams.kernelSize);
		s_DeferredData->SSAOShader->SetFloat("radius", SSAOParams.radius);
		s_DeferredData->SSAOShader->SetFloat("bias", SSAOParams.bias);
		s_DeferredData->SSAOShader->SetInt("texNoise", 2);
		
		s_DeferredData->g_Posistion->Bind(0);
		s_DeferredData->g_Normal->Bind(1);
		glBindTextureUnit(2, NoiseTexture);

		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();

		s_DeferredData->SSAOShader->Unbind();
		s_DeferredData->SSAOFBO->Unbind();
	}

	void DeferredRendering::SSAOBlurPass()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		s_DeferredData->SSAOBlurShader->Bind();
		s_DeferredData->SSAOBlurFBO->Bind();
		s_DeferredData->g_SSAO->Bind();

		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();

		s_DeferredData->SSAOBlurShader->Unbind();
		s_DeferredData->SSAOBlurFBO->Unbind();
	}
	//G-Buffers breaddown
	//slot 0 : g_Posistion
	//slot 1 : g_WorldNormal
	//slot 2 : g_Albedo
	//slot 3 : g_Normal
	//slot 4 : g_MetallicRoughness
	//slot 5 : g_SpecularTexture;
	//slot 6 : g_IrradianceTexture
	//slot 7 : g_SpecularBRDF_LUT;
	//slot 8 : g_SSAO;
	//slot 9 : g_SSAOBlur;

	void DeferredRendering::DirectionalLightPass(PerspectiveCamera& camera, Light& light)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		s_DeferredData->LightingShader->Bind();
		light.ShaderBinding(s_DeferredData->LightingShader);
		s_DeferredData->LightingShader->SetFloat3("u_ViewPos", camera.GetPosition());
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i);
		}

		//SSAO binding
		s_DeferredData->g_SSAO->Bind(8);
		s_DeferredData->g_SSAO->Bind(9);
		
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->LightingShader->Unbind();
	}

	void DeferredRendering::PointLightPass(PerspectiveCamera& camera, std::vector<PointLight>& pointLights)
	{
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);

		s_DeferredData->PointLightShader->Bind();
		s_DeferredData->PointLightShader->SetInt("u_LightNum", pointLights.size());
		for (unsigned int i = 0; i < pointLights.size(); ++i) {
			s_DeferredData->PointLightShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Position", pointLights[i].Position);
			s_DeferredData->PointLightShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Color", pointLights[i].Diffuse);
			const float constant = 1.0f;
			const float linear = pointLights[i].Linear;
			const float quadratic = pointLights[i].Quadratic;
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Linear", linear);
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Quadratic", quadratic);
			const float maxBrightness = std::fmaxf(std::fmaxf(pointLights[i].Diffuse.r, pointLights[i].Diffuse.g), pointLights[i].Diffuse.b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Radius", radius);
		}
		s_DeferredData->PointLightShader->SetFloat3("u_ViewPos", camera.GetPosition());
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i + 1);
		}
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->PointLightShader->Unbind();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void DeferredRendering::MomentShadowMapPass(PerspectiveCamera& camera, Light& light)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		s_DeferredData->LightingShader->Bind();
		light.ShaderBinding(s_DeferredData->LightingShader);
		s_DeferredData->LightingShader->SetFloat3("u_ViewPos", camera.GetPosition());
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i);
		}

		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->LightingShader->Unbind();
	}

	void DeferredRendering::BeginSkyboxPass()
	{
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_DeferredData->GBufferFBO->GetFramebufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, s_DeferredData->GBufferFBO->GetFramebufferSize().first, s_DeferredData->GBufferFBO->GetFramebufferSize().second, 0, 0, s_DeferredData->GBufferFBO->GetFramebufferSize().first, s_DeferredData->GBufferFBO->GetFramebufferSize().second, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDepthFunc(GL_LEQUAL);

		s_DeferredData->SkyboxShader->Bind();
		s_DeferredData->EnvTexture->Bind(0);
		s_DeferredData->SkyboxShader->SetInt("u_Skybox", 0);
	}

	void DeferredRendering::EndSkyboxPass()
	{
		s_DeferredData->SkyboxShader->Unbind();
	}

	const std::shared_ptr<Shader>& DeferredRendering::GetGeometryShader()
	{
		return s_DeferredData->GeometryShader;
	}

	const std::shared_ptr<Shader>& DeferredRendering::GetShadowMapShader()
	{
		return s_DeferredData->ShadowMapShader;
	}

	const std::shared_ptr<Shader>& DeferredRendering::GetLightingShader()
	{
		return s_DeferredData->LightingShader;
	}

	const std::shared_ptr<Shader>& DeferredRendering::GetSkyboxShader()
	{
		return s_DeferredData->SkyboxShader;
	}

	const std::shared_ptr<Texture2D>& DeferredRendering::GetShadowMapTexture()
	{
		return s_DeferredData->g_ShadowMap;
	}
}