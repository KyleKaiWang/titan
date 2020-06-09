#include "tpch.h"
#include "SceneDeferred.h"
#include "../Renderer/Texture.h"
#include "Application.h"
#include <glad/glad.h>

namespace Titan {

	const unsigned int NUM_LIGHTS = 5;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	std::vector<std::shared_ptr<Texture2D>> SceneDeferred::GBufferTextures;

	struct SceneDeferredStorage
	{
		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<VertexArray> VertexArray;
		std::shared_ptr<Shader> DeferredShader;
		std::shared_ptr<Shader> LightingShader;

		uint32_t RenderBuffer;
		uint32_t DepthBuffer;

		std::shared_ptr<Texture2D> g_Normal;
		std::shared_ptr<Texture2D> g_Posistion;
		std::shared_ptr<Texture2D> g_Diffuse;
		std::shared_ptr<Texture2D> g_DepthMap;
	};

	static SceneDeferredStorage* s_DeferredData;

	void SceneDeferred::Init()
	{
		s_DeferredData = new SceneDeferredStorage();
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

		s_DeferredData->DeferredShader = Shader::Create("shaders/DirectionalLightPass.vs", "shaders/DirectionalLightPass.fs");
		s_DeferredData->DeferredShader->Bind();
		s_DeferredData->DeferredShader->SetInt("g_PositionTexture", 1);
		s_DeferredData->DeferredShader->SetInt("g_NormalTexture", 2);
		s_DeferredData->DeferredShader->SetInt("g_DiffuseTexture", 3);

		s_DeferredData->LightingShader = Shader::Create("shaders/PointLightPass.vs", "shaders/PointLightPass.fs");
		s_DeferredData->LightingShader->Bind();
		s_DeferredData->LightingShader->SetInt("g_PositionTexture", 1);
		s_DeferredData->LightingShader->SetInt("g_NormalTexture", 2);
		s_DeferredData->LightingShader->SetInt("g_DiffuseTexture", 3);

		auto& window = Application::Get().GetWindow();
		SetFrameBuffer(window.GetWidth(), window.GetHeight());

		// lighting info
		// -------------
		srand(13);
		for (unsigned int i = 0; i < NUM_LIGHTS; i++)
		{
			// calculate slightly random offsets
			//float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			//float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			//float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float xPos = i * 4.0f;
			float yPos = 3.0f;
			float zPos = 1.0f;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			lightColors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
		}
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
		glNamedFramebufferTexture(s_DeferredData->RenderBuffer, GL_COLOR_ATTACHMENT0 + slot, tex->GetTextureID(), 0);
	}

	void SceneDeferred::SetFrameBuffer(uint32_t width, uint32_t height)
	{
		glCreateFramebuffers(1, &s_DeferredData->RenderBuffer);
		
		//Create G-Buffer
		CreateGBufferTexture(s_DeferredData->g_Posistion, 0, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->g_Normal, 1, GL_RGB32F);
		CreateGBufferTexture(s_DeferredData->g_Diffuse, 2, GL_RGB8);
		CreateGBufferTexture(s_DeferredData->g_DepthMap, 3, GL_RGB8);

		if (glCheckNamedFramebufferStatus(s_DeferredData->RenderBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "framebuffer error\n";

		unsigned int attachments[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glNamedFramebufferDrawBuffers(s_DeferredData->RenderBuffer, 5, attachments);

		// create and attach depth buffer (renderbuffer)
		glCreateRenderbuffers(1, &s_DeferredData->DepthBuffer);
		if (GL_TRUE != glIsRenderbuffer(s_DeferredData->DepthBuffer))
			std::cerr << "Error creating depth renderbuffer.\n";
		glNamedRenderbufferStorage(s_DeferredData->DepthBuffer, GL_DEPTH_COMPONENT, width, height);
		glNamedFramebufferRenderbuffer(s_DeferredData->RenderBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_DeferredData->DepthBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void SceneDeferred::BindingFramebuffer(PerspectiveCamera& camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData->RenderBuffer);
		glEnable(GL_DEPTH_TEST);
	}
	void SceneDeferred::DeferredPass(PerspectiveCamera& camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		s_DeferredData->DeferredShader->Bind();
		s_DeferredData->DeferredShader->SetFloat3("u_DirLight.Direction", glm::vec3(-1, -1, -1));
		s_DeferredData->DeferredShader->SetFloat3("u_ViewPos", camera.GetPosition());
			
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i+1);
		}
		
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->DeferredShader->Unbind();

		//Read Buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_DeferredData->RenderBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitNamedFramebuffer(s_DeferredData->RenderBuffer, 0, 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//LightingPass(camera);
	}
	void SceneDeferred::LightingPass(PerspectiveCamera& camera)
	{
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		s_DeferredData->LightingShader->Bind();
		for (unsigned int i = 0; i < lightPositions.size(); ++i) {
			s_DeferredData->LightingShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			s_DeferredData->LightingShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Color", lightColors[i]);
			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			s_DeferredData->LightingShader->SetFloat("u_Lights[" + std::to_string(i) + "].Linear", linear);
			s_DeferredData->LightingShader->SetFloat("u_Lights[" + std::to_string(i) + "].Quadratic", quadratic);
			s_DeferredData->LightingShader->SetFloat("u_Lights[" + std::to_string(i) + "].Radius", 2.0);
		}
		s_DeferredData->LightingShader->SetFloat3("u_ViewPos", camera.GetPosition());
		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i + 1);
		}
		s_DeferredData->VertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		//s_DeferredData->LightingShader->Unbind();

		//glCullFace(GL_BACK);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}