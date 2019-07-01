#include "tpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Titan {
	
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		TITAN_CORE_ASSERT(windowHandle, "Window handle is null")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TITAN_CORE_ASSERT(status, "Failed to initialize Glad");

		TITAN_CORE_INFO(" OpenGL Info:");
		TITAN_CORE_INFO(" Vender: {0}", glGetString(GL_VENDOR));
		TITAN_CORE_INFO(" Renderer: {0}", glGetString(GL_RENDERER));
		TITAN_CORE_INFO(" Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}

