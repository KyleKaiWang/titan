#include "tpch.h"
#include "OpenGLDebugDraw.h"
#include "Renderer\RenderCommand.h"
#include "Renderer\Renderer2D.h"
#include <glm\gtx\transform.hpp>
#include <glm\ext\matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Titan {

	OpenGLDebugDraw::OpenGLDebugDraw()
	{
	}

	OpenGLDebugDraw::~OpenGLDebugDraw()
	{
	}

	void OpenGLDebugDraw::Init()
	{
	}
	
	void OpenGLDebugDraw::DrawDebugList()
	{
	}

	void OpenGLDebugDraw::AddLine(const glm::vec3& fromPosition, const glm::vec3& toPosition, glm::vec3 color, float lineWidth, float duration, bool depthEnabled)
	{
		glLineWidth(lineWidth);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		glVertex3f(fromPosition.x, fromPosition.y, fromPosition.z);
		glVertex3f(toPosition.x, toPosition.y, toPosition.z);
		glColor3f(color.x, color.y, color.z);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		
	}
	
	void OpenGLDebugDraw::AddCross(const glm::vec3& position, glm::vec3 color, float size, float duration, bool depthEnabled)
	{
		
	}
	
	void OpenGLDebugDraw::AddSphere(const glm::vec3& centerPosition, float radius, glm::vec3 color, float duration, bool depthEnabled)
	{
	}
	
	void OpenGLDebugDraw::AddCircle(const glm::vec3& centerPosition, const glm::vec3& planeNormal, float radius, glm::vec3 color, float duration, bool depthEnabled)
	{
	}

	void OpenGLDebugDraw::AddAABB(const glm::vec3& position, const glm::vec2& size, float rotation, glm::vec3& color, float lineWidth, float duration, bool depthEnabled)
	{
		Renderer2D::DrawDebugQuad(position, size, rotation, glm::vec4(color, 1.0f));
	}
}
