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
		virtual void BlitTo(Framebuffer* fbo, uint32_t mask) override;

		virtual void SetFramebufferID(uint32_t id) override { m_RendererID = id; }
		virtual uint32_t GetFramebufferID() const override { return m_RendererID; }
		virtual std::vector<std::shared_ptr<Texture2D>> GetColorAttachments() const override { return m_ColorAttachments; }
		virtual std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index = 0) const override;
		virtual std::shared_ptr<Texture2D> GetDepthAttachment() const override { return m_DepthAttachment; }
		virtual void SetColorAttachment(uint32_t id, uint32_t index) override;
		virtual void SetDepthAttachment(uint32_t id) override { m_DepthAttachment->SetTextureID(id); }
		virtual const FramebufferDesc& GetFramebufferDesc() const override { return m_FramebufferDesc; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<Texture2D>> m_ColorAttachments;
		std::shared_ptr<Texture2D> m_DepthAttachment;
		FramebufferDesc m_FramebufferDesc;
	};

}