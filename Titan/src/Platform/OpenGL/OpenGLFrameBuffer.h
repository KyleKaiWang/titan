#pragma once

#include "Renderer/FrameBuffer.h"
#include "Renderer/Texture.h"

namespace Titan {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferDesc& spec);
		virtual ~OpenGLFramebuffer();

		void Init();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual std::vector<std::shared_ptr<Texture2D>> GetColorAttachments() const override { return m_ColorAttachments; }
		virtual std::shared_ptr<Texture2D> GetColorAttachment(uint32_t index) const;
		virtual std::shared_ptr<Texture2D> GetDepthAttachments() const override { return m_DepthAttachment; }
		virtual const FramebufferDesc& GetDesc() const override { return m_Desc; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<Texture2D>> m_ColorAttachments;
		std::shared_ptr<Texture2D> m_DepthAttachment;
		FramebufferDesc m_Desc;
	};

}