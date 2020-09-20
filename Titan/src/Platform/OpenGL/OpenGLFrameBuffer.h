#pragma once

#include "Renderer/FrameBuffer.h"
#include "Renderer/Texture.h"

namespace Titan {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferDesc& framebufferDesc);
		virtual ~OpenGLFramebuffer();

		void Init();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void BlitTo(const Framebuffer& fbo, uint32_t mask) override;

		virtual uint32_t GetFramebufferID() const override { return m_RendererID; }
		virtual std::vector<std::shared_ptr<Texture2D>> GetColorAttachments() const override { return m_ColorAttachments; }
		virtual std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index) const override;
		virtual std::shared_ptr<Texture2D> GetDepthAttachment() const override { return m_DepthAttachment; }
		virtual const FramebufferDesc& GetFramebufferDesc() const override { return m_FramebufferDesc; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<Texture2D>> m_ColorAttachments;
		std::shared_ptr<Texture2D> m_DepthAttachment;
		FramebufferDesc m_FramebufferDesc;
	};

}