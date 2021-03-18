#include "tpch.h"
#include "MomentShadowRendering.h"
#include "Camera.h"
#include "Application.h"
#include "../imgui/imgui.h"
#include <glad/glad.h>

namespace Titan {

	MomentShadowRendering::MomentShadowRendering()
	{
	}

	MomentShadowRendering::~MomentShadowRendering()
	{
	}

	void MomentShadowRendering::Init(uint32_t width, uint32_t height)
	{
		ShadowMapShader = Shader::Create("shaders/ShadowMapPass.vs", "shaders/ShadowMapPass.fs");
		BlurShadowMapShader = Shader::Create("shaders/GaussianBlur.vs", "shaders/GaussianBlur.fs");

		//Shadow Map
		{
			TextureDesc texDesc;
			texDesc.Target = GL_TEXTURE_2D;
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
			desc.nrColorAttachment = 1;
			desc.TexDesc = texDesc;
			ShadowMapFBO = Framebuffer::Create(desc);
			g_ShadowMap = ShadowMapFBO->GetColorAttachment(0);
		}
		
		//Blur Shadow Map
		{
			TextureDesc texDesc;
			texDesc.Target = GL_TEXTURE_2D;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Format = GL_RGBA32F;
			texDesc.MipLevels = 0;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			
			FramebufferDesc desc;
			desc.Width = width;
			desc.Height = height;
			desc.nrColorAttachment = 1;
			desc.TexDesc = texDesc;
		
			//Horizen Blur
			BlurShadowMapFBOs.push_back(Framebuffer::Create(desc));
			g_BlurShadowMaps.push_back(BlurShadowMapFBOs[0]->GetColorAttachment(0));
			
			//Vertical Blur
			BlurShadowMapFBOs.push_back(Framebuffer::Create(desc));
			g_BlurShadowMaps.push_back(BlurShadowMapFBOs[1]->GetColorAttachment(0));
		}
	}

	void MomentShadowRendering::BeginShadowPass(std::function<void()> drawQuad)
	{
		ShadowMapFBO->Bind();
		glViewport(0, 0, ShadowMapFBO->GetFramebufferSize().first, ShadowMapFBO->GetFramebufferSize().second);
		glNamedFramebufferDrawBuffer(ShadowMapFBO->GetFramebufferID(), GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
	}

	void MomentShadowRendering::EndShadowPass()
	{
		ShadowMapFBO->Unbind();
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void MomentShadowRendering::BeginBlurShadowPass(std::function<void()> drawQuad)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, BlurShadowMapFBOs[0]->GetFramebufferSize().first, ShadowMapFBO->GetFramebufferSize().second);

		// --------------------Pixel Shader----------------------
		BlurShadowMapShader->Bind();

		//Horizen Pass
		BlurShadowMapFBOs[0]->Bind();
		BlurShadowMapShader->Bind();
		BlurShadowMapShader->SetInt("u_Horizontal", 1);

		//Original Shadow Map
		g_ShadowMap->Bind(0);
		BlurShadowMapShader->SetInt("u_SceneTexture", 0);
		drawQuad();
		BlurShadowMapShader->Unbind();
		BlurShadowMapFBOs[0]->Unbind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Vertical Pass
		BlurShadowMapFBOs[1]->Bind();
		BlurShadowMapShader->Bind();
		BlurShadowMapShader->SetInt("u_Horizontal", 0);

		//Shadow Map after blur
		g_BlurShadowMaps[0]->Bind(0);
		BlurShadowMapShader->SetInt("u_SceneTexture", 0);
		drawQuad();
		BlurShadowMapShader->Unbind();
		BlurShadowMapFBOs[1]->Unbind();
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void MomentShadowRendering::EndBlurShadowPass()
	{
	}
}
