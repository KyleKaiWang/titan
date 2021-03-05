#include "tpch.h"
#include "PostProcessBloom.h"
#include "Shader.h"
#include "Application.h"
#include "DeferredRendering.h"
#include "FrameBuffer.h"
#include "../ImGui/imgui.h"
#include "../Utility/Random.h"
#include <glad/glad.h>

namespace Titan {
	std::shared_ptr<Shader> gaussianBlurShader;
	std::shared_ptr<Framebuffer> tempFBO;
	std::shared_ptr<Framebuffer> gaussianBlurFBO;
	std::shared_ptr<Texture2D> gaussianBlurTex;

	void PostProcessBloom::Init()
	{
		auto& window = Application::Get().GetWindow();
		int width = window.GetWidth();
		int height = window.GetHeight();

		gaussianBlurShader = Shader::Create("shaders/DrawQuad.vs", "shaders/GaussianBlur_Bloom.fs");
		gaussianBlurShader->SetInt("image", 0);

		TextureDesc texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = GL_RGBA32F;
		texDesc.MipLevels = 0;
		texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		FramebufferDesc desc;
		desc.Name = "BloomGaussianBlur";
		desc.Width = width;
		desc.Height = height;
		desc.nrColorAttachment = 1;
		desc.Depth = false;
		desc.TexDesc = texDesc;
		gaussianBlurFBO = Framebuffer::Create(desc);
		gaussianBlurTex = gaussianBlurFBO->GetColorAttachment(0);
		tempFBO = Framebuffer::Create(desc);
		DeferredRendering::GBufferTextures.push_back(gaussianBlurTex);
	}

	void PostProcessBloom::Render(std::shared_ptr<Texture2D>& emmisiveBuffer)
	{
		glDisable(GL_DEPTH_TEST);
		gaussianBlurShader->Bind();
		bool first_iteration = true;
		int amount = 10;
		for (int i = 0; i < amount; ++i) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			tempFBO->Bind();

			//Horizen Pass
			if (first_iteration)
				emmisiveBuffer->Bind(0);
			else
				gaussianBlurTex->Bind(0);

			gaussianBlurShader->SetInt("horizontal", 1);
			DeferredRendering::DrawQuad();
			tempFBO->Unbind();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Vertical Pass
			gaussianBlurShader->Bind();
			gaussianBlurShader->SetInt("horizontal", 0);
			tempFBO->GetColorAttachment(0)->Bind(0);
			DeferredRendering::DrawQuad();
			gaussianBlurFBO->Unbind();
			if (first_iteration)
				first_iteration = false;
		}
		gaussianBlurShader->Unbind();
	}

	std::shared_ptr<Texture2D> PostProcessBloom::GetGaussianBlurTex()
	{
		return gaussianBlurTex;
	}

	void PostProcessBloom::RenderDebug()
	{
	}
}