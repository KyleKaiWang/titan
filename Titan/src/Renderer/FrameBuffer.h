#pragma once

#include "Core.h"
#include "Texture.h"

namespace Titan {

	struct FramebufferDesc
	{
		uint32_t Width, Height;
		uint32_t nrColorAttachment = 1;
		TextureDesc TexDesc;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetFramebufferID() const = 0;
		virtual std::vector<std::shared_ptr<Texture2D>> GetColorAttachments() const = 0;
		virtual std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index) const = 0;
		virtual std::shared_ptr<Texture2D> GetDepthAttachment() const = 0;
		virtual const FramebufferDesc& GetFramebufferDesc() const = 0;
		
		const std::pair<uint32_t, uint32_t> GetFramebufferSize() const { return std::pair<uint32_t, uint32_t>(GetFramebufferDesc().Width, GetFramebufferDesc().Height); }

		static Ref<Framebuffer> Create(const FramebufferDesc& desc);
	};


}