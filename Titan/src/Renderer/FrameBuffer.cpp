#include "tpch.h"
#include "Framebuffer.h"
#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Titan {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferDesc& desc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TITAN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(desc);
		}

		TITAN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}