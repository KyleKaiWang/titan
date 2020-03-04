#include "tpch.h"
#include "OpenGLDebugDraw.h"
#include "Renderer\RenderCommand.h"
#include "Renderer\Renderer2D.h"
#include "Renderer\Shader.h"
#include "Application.h"
#include <glm\gtx\transform.hpp>
#include <glm\ext\matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl/GLU.h>

namespace Titan {

	struct DebugDrawProps
	{
		std::shared_ptr<Shader> DebugShader;
		float LineArr[6] = { 0.0f };
		float CircleArr[37] = { 0.0f };
		uint32_t VBO, VAO;
	};
	std::vector<float> circleList;
	static DebugDrawProps* s_DebugDrawProps;

	void OpenGLDebugDraw::Init()
	{
		s_DebugDrawProps = new DebugDrawProps();
		s_DebugDrawProps->DebugShader = Shader::Create("shaders/DebugDraw.vs", "shaders/DebugDraw.fs");

		glGenVertexArrays(1, &s_DebugDrawProps->VAO);
		glGenBuffers(1, &s_DebugDrawProps->VBO);
		glBindVertexArray(s_DebugDrawProps->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_DebugDrawProps->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s_DebugDrawProps->LineArr), s_DebugDrawProps->LineArr, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void OpenGLDebugDraw::End()
	{
		glDeleteVertexArrays(1, &s_DebugDrawProps->VAO);
		glDeleteBuffers(1, &s_DebugDrawProps->VBO);
	}

	void OpenGLDebugDraw::DrawLine(const glm::vec3& fromPosition, const glm::vec3& toPosition, glm::vec3 color, float lineWidth, float duration, bool depthEnabled)
	{
		s_DebugDrawProps->DebugShader->Bind();
		s_DebugDrawProps->DebugShader->SetFloat4("u_Color", glm::vec4(color, 1.0f));

		s_DebugDrawProps->LineArr[0] = fromPosition.x;
		s_DebugDrawProps->LineArr[1] = fromPosition.y;
		s_DebugDrawProps->LineArr[2] = fromPosition.z;
		s_DebugDrawProps->LineArr[3] = toPosition.x;
		s_DebugDrawProps->LineArr[4] = toPosition.y;
		s_DebugDrawProps->LineArr[5] = toPosition.z;
		glBindBuffer(GL_ARRAY_BUFFER, s_DebugDrawProps->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s_DebugDrawProps->LineArr), s_DebugDrawProps->LineArr, GL_STATIC_DRAW);

		glLineWidth(lineWidth);
		glBindVertexArray(s_DebugDrawProps->VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
		glLineWidth(1.0f);
	}
	
	void OpenGLDebugDraw::DrawCircle(const glm::vec3& centerPosition, const glm::vec3& planeNormal, float radius, glm::vec3 color, float duration, bool depthEnabled)
	{
		const int numVertices = 37;

		float doublePi = 2.0f * glm::pi<float>();
		
		float circleDataX[numVertices];
		float circleDataY[numVertices];
		float circleDataZ[numVertices];

		for (int i = 0; i < numVertices; ++i)
		{
			circleDataX[i] = centerPosition.x + (radius * cos(i * doublePi / 36));
			circleDataY[i] = centerPosition.y + (radius * sin(i * doublePi / 36));
			circleDataZ[i] = centerPosition.z;
		}

		float circleData[numVertices * 3];

		for (int i = 0; i < numVertices; ++i)
		{
			circleData[i * 3] = circleDataX[i];
			circleData[(i * 3) + 1] = circleDataY[i];
			circleData[(i * 3) + 2] = circleDataZ[i];
		}
		auto& app =  Titan::Application::Get();
		Window& win = app.GetWindow();
		float aspectR = (float)win.GetWidth() / (float)win.GetHeight();
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), centerPosition) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), { 1 / aspectR, 1.0f, 1.0f });
		s_DebugDrawProps->DebugShader->SetMat4("u_Transform", trans);

		s_DebugDrawProps->DebugShader->Bind();
		s_DebugDrawProps->DebugShader->SetFloat4("u_Color", glm::vec4(color, 1.0f));
		glBindBuffer(GL_ARRAY_BUFFER, s_DebugDrawProps->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circleData), circleData, GL_STATIC_DRAW);

		glBindVertexArray(s_DebugDrawProps->VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 37);
		glBindVertexArray(0);
	}

	void OpenGLDebugDraw::DrawAABB(const glm::vec3& position, const glm::vec2& size, float rotation, glm::vec3& color, float lineWidth, float duration, bool depthEnabled)
	{
		Renderer2D::DrawDebugQuad(position, size, rotation, glm::vec4(color, 1.0f));
	}
}
