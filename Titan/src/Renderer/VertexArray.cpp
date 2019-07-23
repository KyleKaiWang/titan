#include "tpch.h"
#include "VertexArray.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Titan {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TITAN_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}

		TITAN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}