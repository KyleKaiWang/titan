#include "tpch.h"
#include "RendererSSAO.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Application.h"
#include "DeferredRendering.h"
#include "../imgui/imgui.h"
#include <glad/glad.h>
#include <random>

#define FBO_SIZE                2048

namespace Titan {

	SSAOParameters RendererSSAO::SSAOParams;
	std::vector<glm::vec3> ssaoKernel;
	std::vector<float> ssaoNoise;
	unsigned int NoiseTexture;

	std::shared_ptr<Shader> SSAOShader;
	std::shared_ptr<Shader> SSAOBlurShader;
	std::shared_ptr<Framebuffer> SSAOFBO;
	std::shared_ptr<Framebuffer> SSAOBlurFBO;
	std::shared_ptr<Texture2D> g_SSAO;
	std::shared_ptr<Texture2D> g_SSAOBlur;

	void RendererSSAO::Init()
	{

		auto& window = Application::Get().GetWindow();
		uint32_t width = window.GetWidth();
		uint32_t height = window.GetHeight();

		SSAOShader = Shader::Create("shaders/SSAO.vs", "shaders/SSAO.fs");
		SSAOBlurShader = Shader::Create("shaders/SSAOBlur.vs", "shaders/SSAOBlur.fs");
		
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
			SSAOFBO = Framebuffer::Create(desc);
			g_SSAO = SSAOFBO->GetColorAttachment(0);
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
			SSAOBlurFBO = Framebuffer::Create(desc);
			g_SSAOBlur = SSAOBlurFBO->GetColorAttachment(0);
		}

		DeferredRendering::DebugTextures.push_back(g_SSAO);
		DeferredRendering::DebugTextures.push_back(g_SSAOBlur);

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

	void RendererSSAO::RenderSSAO(Camera camera, std::shared_ptr<Texture2D>& gPosition, std::shared_ptr<Texture2D>& gNormal)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SSAOFBO->Bind();
		SSAOShader->Bind();
		for (unsigned int i = 0; i < 64; ++i)
			SSAOShader->SetFloat3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		SSAOShader->SetMat4("u_ViewMatrix", camera.GetViewMatrix());
		SSAOShader->SetMat4("u_Projection", camera.GetProjectionMatrix());
		SSAOShader->SetInt("screenWidth", 1280);
		SSAOShader->SetInt("screenHeight", 720);
		SSAOShader->SetInt("kernelSize", SSAOParams.kernelSize);
		SSAOShader->SetFloat("radius", SSAOParams.radius);
		SSAOShader->SetFloat("bias", SSAOParams.bias);
		SSAOShader->SetInt("texNoise", 2);

		gPosition->Bind(0);
		gNormal->Bind(1);
		glBindTextureUnit(2, NoiseTexture);

		DeferredRendering::DrawQuad();

		SSAOShader->Unbind();
		SSAOFBO->Unbind();
	}

	void RendererSSAO::RenderSSAOBlur()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SSAOBlurShader->Bind();
		SSAOBlurFBO->Bind();
		g_SSAO->Bind();

		DeferredRendering::DrawQuad();

		SSAOBlurShader->Unbind();
		SSAOBlurFBO->Unbind();
	}

	std::shared_ptr<Texture2D> RendererSSAO::GetSSAOTexture()
	{
		return g_SSAOBlur;
	}

	void RendererSSAO::RenderDebug()
	{
		ImGui::Begin("SSAO", 0, ImGuiWindowFlags_NoCollapse);
		ImGui::BeginChild("SSAO");
		ImGui::DragInt("Kernel Size", &SSAOParams.kernelSize);
		ImGui::DragFloat("Radius", &SSAOParams.radius);
		ImGui::DragFloat("bias", &SSAOParams.bias);
		ImGui::EndChild();
		ImGui::End();
	}
}
