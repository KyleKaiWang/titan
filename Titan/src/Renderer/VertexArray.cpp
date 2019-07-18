#include "tpch.h"
#include "VertexArray.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Titan {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: TITAN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			case RendererAPI::OpenGL: return new OpenGLVertexArray();
		}

		TITAN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}