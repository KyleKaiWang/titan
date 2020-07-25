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

	std::shared_ptr<Shader> SSAOShader;
	std::shared_ptr<Shader> SSAOBlurShader;
	std::shared_ptr<Framebuffer> SSAOFBO;
	std::shared_ptr<Framebuffer> SSAOBlurFBO1;
	std::shared_ptr<Framebuffer> SSAOBlurFBO2;
	std::shared_ptr<Texture2D> g_SSAO;
	std::shared_ptr<Texture2D> g_SSAOBlur1;
	std::shared_ptr<Texture2D> g_SSAOBlur2;

	void RendererSSAO::Init()
	{

		auto& window = Application::Get().GetWindow();
		uint32_t width = window.GetWidth();
		uint32_t height = window.GetHeight();

		SSAOShader = Shader::Create("shaders/SSAO.vs", "shaders/SSAO.fs");
		SSAOBlurShader = Shader::Create("shaders/SSAO.vs", "shaders/BilateralFilter.fs");
		
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
			SSAOBlurFBO1 = Framebuffer::Create(desc);
			g_SSAOBlur1 = SSAOBlurFBO1->GetColorAttachment(0);
			SSAOBlurFBO2 = Framebuffer::Create(desc);
			g_SSAOBlur2 = SSAOBlurFBO2->GetColorAttachment(0);
		}

		DeferredRendering::DebugTextures.push_back(g_SSAO);
		DeferredRendering::DebugTextures.push_back(g_SSAOBlur1);
		DeferredRendering::DebugTextures.push_back(g_SSAOBlur2);
	}

	void RendererSSAO::RenderSSAO(Camera camera, std::shared_ptr<Texture2D>& gPosition, std::shared_ptr<Texture2D>& gNormal)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SSAOFBO->Bind();
		SSAOShader->Bind();
		SSAOShader->SetMat4("u_ViewMatrix", camera.GetViewMatrix());
		SSAOShader->SetMat4("u_Projection", camera.GetProjectionMatrix());
		SSAOShader->SetInt("screenWidth", 1280);
		SSAOShader->SetInt("screenHeight", 720);
		
		//SSAO2
		SSAOShader->SetInt("use_rendered_normals", SSAOParams.use_rendered_normals);
		SSAOShader->SetInt("n_samples", SSAOParams.n_samples);
		SSAOShader->SetInt("turns", SSAOParams.turns);
		SSAOShader->SetFloat("ball_radius", SSAOParams.ball_radius);
		SSAOShader->SetFloat("sigma", SSAOParams.sigma);
		SSAOShader->SetFloat("kappa", SSAOParams.kappa);
		SSAOShader->SetFloat("beta", SSAOParams.beta);
		gPosition->Bind(0);
		gNormal->Bind(1);

		DeferredRendering::DrawQuad();

		SSAOShader->Unbind();
		SSAOFBO->Unbind();
	}

	void RendererSSAO::RenderSSAOBlur(Camera camera, std::shared_ptr<Texture2D>& depthTex, std::shared_ptr<Texture2D>& gNormal)
	{
		auto& window = Application::Get().GetWindow();
		uint32_t width = window.GetWidth();
		uint32_t height = window.GetHeight();
		
		//BilateralFilter
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SSAOBlurShader->Bind();
		SSAOBlurFBO1->Bind();
		g_SSAO->Bind(0);
		depthTex->Bind(1);
		gNormal->Bind(2);
		SSAOBlurShader->SetFloat2("invDstSize", 1.0f / glm::vec2(width, height));
		glm::mat4 proj = camera.GetProjectionMatrix();
		SSAOBlurShader->SetFloat2("projParams", glm::vec2(proj[2][2], proj[3][2]));
		
		SSAOBlurShader->SetInt("axis", 0);
		DeferredRendering::DrawQuad();
		SSAOBlurFBO1->Unbind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SSAOBlurFBO2->Bind();
		SSAOBlurShader->SetInt("axis", 1);
		g_SSAOBlur1->Bind(0);
		DeferredRendering::DrawQuad();
		SSAOBlurFBO2->Unbind();
		SSAOBlurShader->Unbind();
	}

	std::shared_ptr<Texture2D> RendererSSAO::GetSSAOTexture()
	{
		return g_SSAOBlur2;
	}

	void RendererSSAO::RenderDebug()
	{
		ImGui::Begin("SSAO", 0, ImGuiWindowFlags_NoCollapse);
		ImGui::BeginChild("SSAO");
		if (ImGui::CollapsingHeader("AO Params")) {
			ImGui::Checkbox("Use Rendered Normals", &SSAOParams.use_rendered_normals);
			ImGui::SliderInt("Num Samples", &SSAOParams.n_samples, 1, 64);
			ImGui::SliderInt("Num Turns", &SSAOParams.turns, 1, 64);
			ImGui::SliderFloat("Ball Radius", &SSAOParams.ball_radius, 0.1f, 10.f);
			ImGui::SliderFloat("Sigma", &SSAOParams.sigma, 0.1f, 20.f);
			ImGui::SliderFloat("Kappa", &SSAOParams.kappa, 0.1f, 10.f);
			ImGui::SliderFloat("beta", &SSAOParams.beta, 0.0f, 5.0f);
		}

		ImGui::EndChild();
		ImGui::End();
	}
}
