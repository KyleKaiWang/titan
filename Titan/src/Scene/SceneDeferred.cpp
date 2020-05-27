#include "tpch.h"
#include "SceneDeferred.h"
#include "../Renderer/Texture.h"
#include "Application.h"
#include <glad/glad.h>

namespace Titan {

	std::vector<std::shared_ptr<Texture2D>> SceneDeferred::GBufferTextures;

	struct SceneDeferredStorage
	{
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Shader> m_Shader;
		uint32_t m_RendererID;
		uint32_t m_DepthBuffer;

		std::shared_ptr<Texture2D> m_NormalTexture;
		std::shared_ptr<Texture2D> m_PosTexture;
		std::shared_ptr<Texture2D> m_DiffuseTexture;
	};

	static SceneDeferredStorage* s_DeferredData;

	void SceneDeferred::Init()
	{
		s_DeferredData = new SceneDeferredStorage();
		s_DeferredData->m_VertexArray = VertexArray::Create();

		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		s_DeferredData->m_VertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		s_DeferredData->m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_DeferredData->m_VertexArray->AddVertexBuffer(s_DeferredData->m_VertexBuffer);
		s_DeferredData->m_VertexArray->Unbind();

		s_DeferredData->m_Shader = Shader::Create("shaders/DeferredShading.vs", "shaders/DeferredShading.fs");
		s_DeferredData->m_Shader->Bind();
		s_DeferredData->m_Shader->SetInt("g_PositionTexture", 1);
		s_DeferredData->m_Shader->SetInt("g_NormalTexture", 2);
		s_DeferredData->m_Shader->SetInt("g_DiffuseTexture", 3);

		auto& window = Application::Get().GetWindow();
		SetFrameBuffer(window.GetWidth(), window.GetHeight());
	}

	void SceneDeferred::Update(float t)
	{

	}

	void SceneDeferred::Render(PerspectiveCamera& camera)
	{
	}

	void SceneDeferred::CreateGBufferTexture(std::shared_ptr<Texture2D>& tex, uint32_t slot, uint32_t format)
	{
		auto& window = Application::Get().GetWindow();
		TextureDesc texDesc;
		texDesc.Width = window.GetWidth();
		texDesc.Height = window.GetHeight();
		texDesc.Format = format;
		texDesc.MipLevels = 0;

		tex = Texture2D::Create(texDesc);
		GBufferTextures.push_back(tex);

		// Attach the textures to the framebuffer
		glNamedFramebufferTexture(s_DeferredData->m_RendererID, GL_COLOR_ATTACHMENT0 + slot, tex->GetTextureID(), 0);
	}

	void SceneDeferred::SetFrameBuffer(uint32_t width, uint32_t height)
	{
		glCreateFramebuffers(1, &s_DeferredData->m_RendererID);
		
		//Create G-Buffer
		CreateGBufferTexture(s_DeferredData->m_PosTexture, 0, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->m_NormalTexture, 1, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->m_DiffuseTexture, 2, GL_RGB8);

		if (glCheckNamedFramebufferStatus(s_DeferredData->m_RendererID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "framebuffer error\n";

		unsigned int attachments[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(s_DeferredData->m_RendererID, 4, attachments);

		// create and attach depth buffer (renderbuffer)
		glCreateRenderbuffers(1, &s_DeferredData->m_DepthBuffer);
		if (GL_TRUE != glIsRenderbuffer(s_DeferredData->m_DepthBuffer))
			std::cerr << "Error creating depth renderbuffer.\n";
		glNamedRenderbufferStorage(s_DeferredData->m_DepthBuffer, GL_DEPTH_COMPONENT, width, height);
		glNamedFramebufferRenderbuffer(s_DeferredData->m_RendererID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_DeferredData->m_DepthBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void SceneDeferred::BindingFramebuffer(PerspectiveCamera& camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData->m_RendererID);
		glEnable(GL_DEPTH_TEST);
	}
	void SceneDeferred::DeferredPass(PerspectiveCamera& camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		s_DeferredData->m_Shader->Bind();
		s_DeferredData->m_Shader->SetFloat4("u_LightPosition", glm::vec4(5.0f, 5.0f, 0.0f, 1.0f));
		s_DeferredData->m_Shader->SetFloat("u_LightIntensity", 5.0f);
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i+1);
		}
		
		s_DeferredData->m_VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		//Read Buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_DeferredData->m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitNamedFramebuffer(s_DeferredData->m_RendererID, 0, 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}