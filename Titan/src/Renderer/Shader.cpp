#include "tpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"



namespace Titan {

	Ref<Shader> Shader::Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TITAN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(vertexShaderPath, fragmentShaderPath);
		}
		
		TITAN_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
