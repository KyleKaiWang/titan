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
		std::shared_ptr<Texture2D> m_ColorTexture;
	};

	static SceneDeferredStorage* s_DeferredData;

	void SceneDeferred::Init()
	{
		s_DeferredData = new SceneDeferredStorage();
		s_DeferredData->m_VertexArray = VertexArray::Create();

		float quadVertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f, 0.0f
		};

		s_DeferredData->m_VertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		s_DeferredData->m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
			});
		s_DeferredData->m_VertexArray->AddVertexBuffer(s_DeferredData->m_VertexBuffer);
		s_DeferredData->m_VertexArray->Unbind();

		auto& window = Application::Get().GetWindow();
		SetFrameBuffer(window.GetWidth(), window.GetHeight());

		s_DeferredData->m_Shader = Shader::Create("shaders/DeferredShading.vs", "shaders/DeferredShading.fs");
		s_DeferredData->m_Shader->Bind();
		s_DeferredData->m_Shader->SetInt("u_PositionTexture", 0);
		s_DeferredData->m_Shader->SetInt("u_NormalTexture", 1);
		s_DeferredData->m_Shader->SetInt("u_ColorTexture", 2);
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
		tex->Bind(slot);
		GBufferTextures.push_back(tex);
	}

	void SceneDeferred::SetFrameBuffer(uint32_t width, uint32_t height)
	{
		glCreateFramebuffers(1, &s_DeferredData->m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData->m_RendererID);
		
		glCreateRenderbuffers(1, &s_DeferredData->m_DepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, s_DeferredData->m_DepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		
		//Create G-Buffer
		CreateGBufferTexture(s_DeferredData->m_PosTexture, 0, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->m_NormalTexture, 1, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->m_ColorTexture, 2, GL_RGB8);
		
		// Attach the textures to the framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_DeferredData->m_DepthBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_DeferredData->m_PosTexture->GetTextureID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, s_DeferredData->m_NormalTexture->GetTextureID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, s_DeferredData->m_ColorTexture->GetTextureID(), 0);

		unsigned int drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(4, drawBuffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void SceneDeferred::Pass1(PerspectiveCamera& camera)
	{
		s_DeferredData->m_Shader->Bind();
		s_DeferredData->m_Shader->SetInt("Pass", 1);

		glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData->m_RendererID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//glFinish();
	}
	void SceneDeferred::Pass2(PerspectiveCamera& camera)
	{
		s_DeferredData->m_Shader->SetInt("Pass", 2);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		s_DeferredData->m_Shader->SetMat4("u_ProjectionMatrix", camera.GetProjectionMatrix());
		s_DeferredData->m_Shader->SetMat4("u_ViewMatrix", camera.GetViewMatrix());
		s_DeferredData->m_Shader->SetMat4("u_Model", glm::mat4(1.0f));
		s_DeferredData->m_Shader->SetMat3("u_NormalMatrix", glm::mat3(glm::vec3(camera.GetViewMatrix()[0]), glm::vec3(camera.GetViewMatrix()[1]), glm::vec3(camera.GetViewMatrix()[3])));

		s_DeferredData->m_VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}