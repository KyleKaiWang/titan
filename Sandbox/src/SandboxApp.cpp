#include <Titan.h>
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Titan::Layer
{

public:

	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Titan::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Titan::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Titan::VertexBuffer::Create(vertices, sizeof(vertices)));

		Titan::BufferLayout layout = {
			{ Titan::ShaderDataType::Float3, "a_Position"},
			{ Titan::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0,1,2 };
		std::shared_ptr<Titan::IndexBuffer> indexBuffer;
		indexBuffer.reset(Titan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Titan::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<Titan::VertexBuffer> squareVB;
		squareVB.reset(Titan::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Titan::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Titan::IndexBuffer> squareIB;
		squareIB.reset(Titan::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = #include "../shaders/BasicShader.vs";
		std::string fragmentSrc = #include "../shaders/BasicShader.fs";

		m_Shader.reset(new Titan::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = "../shaders/BlueShader.vs";
		std::string blueShaderFragmentSrc = "../shaders/BlueShader.fs";

		m_BlueShader.reset(new Titan::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(Titan::Timestep ts) override
	{
		if (Titan::Input::IsKeyPressed(TITAN_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Titan::Input::IsKeyPressed(TITAN_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}

		if (Titan::Input::IsKeyPressed(TITAN_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		else if (Titan::Input::IsKeyPressed(TITAN_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}

		if (Titan::Input::IsKeyPressed(TITAN_KEY_Q))
		{
			m_CameraRotation += m_CameraMoveSpeed * ts;
		}
		else if (Titan::Input::IsKeyPressed(TITAN_KEY_E))
		{
			m_CameraRotation -= m_CameraMoveSpeed * ts;
		}
		
		Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Titan::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Titan::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		for(int y = 0; y = 10; y++)
		{
			for(int x = 0; x = 10; x++)
			{
				glm::vec3 pos(y * 0.1f, x * 0.1f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Titan::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
		}

		Titan::Renderer::Submit(m_Shader, m_VertexArray);

		Titan::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(Titan::Event& event) override
	{

	}

	private:

		std::shared_ptr<Titan::Shader> m_Shader;
		std::shared_ptr<Titan::VertexArray> m_VertexArray;

		std::shared_ptr<Titan::Shader> m_BlueShader;
		std::shared_ptr<Titan::VertexArray> m_SquareVA;

		Titan::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition;
		float m_CameraMoveSpeed = 5.0f;

		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 180.0f;
};

class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
		PushLayer(new ExampleLayer());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

