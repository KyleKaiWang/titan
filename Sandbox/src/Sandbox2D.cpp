#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f/720.0f)
{
}

void Sandbox2D::OnAttach()
{

	m_SquareVA = Titan::VertexArray::Create();

	float squareVertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Titan::Ref<Titan::VertexBuffer> squareVB;
	squareVB = Titan::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Titan::ShaderDataType::Float3, "a_Position" },
		{ Titan::ShaderDataType::Float4, "a_Color"}
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Titan::Ref<Titan::IndexBuffer> squareIB;
	squareIB = Titan::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	std::string flatColorShaderVertexSrc = "shaders/BlueShader.vs";
	std::string flatColorShaderFragmentSrc = "shaders/BlueShader.fs";

	m_FlatColorShader = Titan::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

	//--------------------------------------------------------------------------------------------------

	float texVertices[5 * 4] = {
		-0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
		 0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
		 0.2f,  0.2f, 0.0f, 1.0f, 1.0f,
		-0.2f,  0.2f, 0.0f, 0.0f, 1.0f
	};

	m_VertexArray = Titan::VertexArray::Create();

	Titan::Ref<Titan::VertexBuffer> textureVB;
	textureVB = Titan::VertexBuffer::Create(texVertices, sizeof(texVertices));
	textureVB->SetLayout({
		{ Titan::ShaderDataType::Float3, "a_Position" },
		{ Titan::ShaderDataType::Float2, "a_TexCoord" }
	});

	m_VertexArray->AddVertexBuffer(textureVB);
	m_VertexArray->SetIndexBuffer(squareIB);

	std::string TextureShaderVertexSrc = "shaders/TextureShader.vs";
	std::string TextureShaderFragmentSrc = "shaders/TextureShader.fs";
	
	m_TextureShader = Titan::Shader::Create(TextureShaderVertexSrc, TextureShaderFragmentSrc);
	m_Texture = Titan::Texture2D::Create("assets/textures/checkerboard.png");
	
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_TextureShader)->Bind();
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_TextureShader)->SetFloat4("u_Color", glm::vec4(1.0f));
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Titan::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Titan::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Titan::RenderCommand::Clear();

	Titan::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Titan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
	Titan::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	m_Texture->Bind();
	Titan::Renderer::Submit(m_TextureShader, m_VertexArray, glm::scale(glm::mat4(0.5f), glm::vec3(1.5f)));

	Titan::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Titan::Event& event)
{
	m_CameraController.OnEvent(event);
}
