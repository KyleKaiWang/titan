#include "tpch.h"
#include "SceneDeferred.h"
#include "../Renderer/Texture.h"
#include "../Renderer/FrameBuffer.h"
#include "Application.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	const unsigned int NUM_LIGHTS = 5;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	std::vector<std::shared_ptr<Texture2D>> SceneDeferred::GBufferTextures;

	struct SceneDeferredStorage
	{
		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<VertexArray> VertexArray;

		std::shared_ptr<Shader> GeometryShader;
		std::shared_ptr<Shader> ShadowMapShader;
		std::shared_ptr<Shader> DirectionalLightShader;
		std::shared_ptr<Shader> PointLightShader;

		std::shared_ptr<Framebuffer> GBufferFBO;
		std::shared_ptr<Framebuffer> ShadowMapFBO;

		std::shared_ptr<Texture2D> g_Normal;
		std::shared_ptr<Texture2D> g_Posistion;
		std::shared_ptr<Texture2D> g_Diffuse;
		std::shared_ptr<Texture2D> g_ShadowMap;
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

		//float quadVertices[] = {
		//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		//	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		//	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		//	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		//	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		//};

		s_DeferredData->VertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		s_DeferredData->VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_DeferredData->VertexArray->AddVertexBuffer(s_DeferredData->VertexBuffer);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->GeometryShader = Titan::Shader::Create("shaders/Geometry.vs", "shaders/Geometry.fs");
		s_DeferredData->ShadowMapShader = Shader::Create("shaders/ShadowMapPass.vs", "shaders/ShadowMapPass.fs");

		s_DeferredData->DirectionalLightShader = Shader::Create("shaders/DirectionalLightPass.vs", "shaders/DirectionalLightPass.fs");
		s_DeferredData->DirectionalLightShader->Bind();
		s_DeferredData->DirectionalLightShader->SetInt("g_PositionTexture", 1);
		s_DeferredData->DirectionalLightShader->SetInt("g_NormalTexture", 2);
		s_DeferredData->DirectionalLightShader->SetInt("g_DiffuseTexture", 3);
		s_DeferredData->DirectionalLightShader->SetInt("g_ShadowMap", 4);

		s_DeferredData->PointLightShader = Shader::Create("shaders/PointLightPass.vs", "shaders/PointLightPass.fs");
		s_DeferredData->PointLightShader->Bind();
		s_DeferredData->PointLightShader->SetInt("g_PositionTexture", 1);
		s_DeferredData->PointLightShader->SetInt("g_NormalTexture", 2);
		s_DeferredData->PointLightShader->SetInt("g_DiffuseTexture", 3);

		auto& window = Application::Get().GetWindow();
		SetFrameBuffer(window.GetWidth(), window.GetHeight());

		// Local lighting info
		// -------------
		srand(13);
		for (unsigned int i = 0; i < NUM_LIGHTS; i++)
		{
			float xPos = i * 4.0f;
			float yPos = 3.0f;
			float zPos = 1.0f;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			lightColors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
		}
	}

	void SceneDeferred::SetFrameBuffer(uint32_t width, uint32_t height)
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
			desc.nrColorAttachment = 3;
			desc.TexDesc = texDesc;
			
			s_DeferredData->GBufferFBO = Framebuffer::Create(desc);
			s_DeferredData->g_Posistion = s_DeferredData->GBufferFBO->GetColorAttachment(0);
			s_DeferredData->g_Normal = s_DeferredData->GBufferFBO->GetColorAttachment(1);
			s_DeferredData->g_Diffuse = s_DeferredData->GBufferFBO->GetColorAttachment(2);

			GBufferTextures.push_back(s_DeferredData->g_Posistion);
			GBufferTextures.push_back(s_DeferredData->g_Normal);
			GBufferTextures.push_back(s_DeferredData->g_Diffuse);
		}

		//Shadow Map
		{
			TextureDesc texDesc;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Format = GL_RGBA32F;
			texDesc.MipLevels = 0;
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			texDesc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

			FramebufferDesc desc;
			desc.Width = width;
			desc.Height = height;
			desc.nrColorAttachment = 0;
			desc.TexDesc = texDesc;
			s_DeferredData->ShadowMapFBO = Framebuffer::Create(desc);
			s_DeferredData->g_ShadowMap = s_DeferredData->ShadowMapFBO->GetDepthAttachment();
			GBufferTextures.push_back(s_DeferredData->g_ShadowMap);
		}
	}

	void SceneDeferred::BeginGeometryPass()
	{
		s_DeferredData->GBufferFBO->Bind();
		unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(s_DeferredData->GBufferFBO->GetFramebufferID(), 3, attachments);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void SceneDeferred::EndGeometryPass()
	{
		s_DeferredData->GBufferFBO->Unbind();
	}

	void SceneDeferred::BeginShadowPass()
	{
		s_DeferredData->ShadowMapFBO->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, s_DeferredData->ShadowMapFBO->GetFramebufferSize().first, s_DeferredData->ShadowMapFBO->GetFramebufferSize().second);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	void SceneDeferred::EndShadowPass()
	{
		glCullFace(GL_BACK);
		s_DeferredData->ShadowMapFBO->Unbind();
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void SceneDeferred::DirectionalLightPass(PerspectiveCamera& camera, Light& light)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		s_DeferredData->DirectionalLightShader->Bind();
		s_DeferredData->DirectionalLightShader->SetFloat3("u_LightPos", light.Position);
		s_DeferredData->DirectionalLightShader->SetFloat3("u_LightDir", light.Direction);
		s_DeferredData->DirectionalLightShader->SetFloat3("u_ViewPos", camera.GetPosition());
		s_DeferredData->DirectionalLightShader->SetMat4("u_LightSpaceMatrix", light.SpaceMatrix);

		for (int i = 0; i < GBufferTextures.size(); ++i) {
			GBufferTextures[i]->Bind(i+1);
		}
		
		s_DeferredData->VertexArray->Bind();
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		s_DeferredData->VertexArray->Unbind();
		s_DeferredData->DirectionalLightShader->Unbind();

		//Read Buffer
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, s_DeferredData->RenderBuffer);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBlitNamedFramebuffer(s_DeferredData->RenderBuffer, 0, 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), 0, 0, GBufferTextures[0]->GetWidth(), GBufferTextures[0]->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void SceneDeferred::PointLightPass(PerspectiveCamera& camera)
	{
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);

		s_DeferredData->PointLightShader->Bind();
		for (unsigned int i = 0; i < lightPositions.size(); ++i) {
			s_DeferredData->PointLightShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			s_DeferredData->PointLightShader->SetFloat3("u_Lights[" + std::to_string(i) + "].Color", lightColors[i]);
			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Linear", linear);
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Quadratic", quadratic);
			s_DeferredData->PointLightShader->SetFloat("u_Lights[" + std::to_string(i) + "].Radius", 2.0);
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

	const std::shared_ptr<Shader>& SceneDeferred::GetGeometryShader()
	{
		return s_DeferredData->GeometryShader;
	}

	const std::shared_ptr<Shader>& SceneDeferred::GetShadowMapShader()
	{
		return s_DeferredData->ShadowMapShader;
	}

	const std::shared_ptr<Texture2D>& SceneDeferred::GetShadowMapTexture()
	{
		return s_DeferredData->g_ShadowMap;
	}
}