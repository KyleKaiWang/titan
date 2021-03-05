#include "tpch.h"
#include "PostProcessTonemap.h"
#include "Shader.h"
#include "Application.h"
#include "DeferredRendering.h"
#include "FrameBuffer.h"
#include "../ImGui/imgui.h"
#include "../Utility/Random.h"
#include <glad/glad.h>

namespace Titan {

	static std::shared_ptr<Shader> tonemapShader;
	float exposure = 0.5f;
	float sceneFactor = 0.5f;
	float bloomFactor = 0.5f;
	void PostProcessTonemap::Init()
	{
		tonemapShader = Shader::Create("shaders/DrawQuad.vs", "shaders/Tonemap.fs");
		tonemapShader->SetInt("scene", 0);
		tonemapShader->SetInt("sceneBloomBlur", 1);
	}

	void PostProcessTonemap::Render(std::shared_ptr<Texture2D>& sceneTex, std::shared_ptr<Texture2D>& sceneBloomBlur)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		tonemapShader->Bind();
		sceneTex->Bind(0);
		sceneBloomBlur->Bind(1);
		tonemapShader->SetInt("enableBloom", true);
		tonemapShader->SetFloat("exposure", 0.5f);
		tonemapShader->SetFloat("sceneFactor", 1.0f);
		tonemapShader->SetFloat("bloomFactor", 1.0f);
		DeferredRendering::DrawQuad();
		tonemapShader->Unbind();
	}

	void PostProcessTonemap::RenderDebug()
	{
		ImGui::Begin("Bloom", 0, ImGuiWindowFlags_NoCollapse);
		ImGui::BeginChild("Bloom");
		ImGui::SliderFloat("Exposure", &Titan::exposure, 0.0f, 10.0f);
		ImGui::SliderFloat("Scene Factor", &Titan::sceneFactor, 0.0f, 10.0f);
		ImGui::SliderFloat("Bloom Factor", &Titan::bloomFactor, 0.0f, 10.0f);
		ImGui::EndChild();
		ImGui::End();
	}
}
