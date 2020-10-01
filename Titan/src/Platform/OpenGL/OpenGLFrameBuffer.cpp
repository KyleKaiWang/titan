#include "tpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Titan {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDesc& framebufferDesc)
		: m_FramebufferDesc(framebufferDesc)
	{
		Init();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLFramebuffer::Init()
	{
		glCreateFramebuffers(1, &m_RendererID);
		//Color Attachments
		for (uint32_t i = 0; i < m_FramebufferDesc.nrColorAttachment; ++i)
		{
			std::shared_ptr<Texture2D> tex = Texture2D::Create(m_FramebufferDesc.TexDesc);
			glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0 + i, tex->GetTextureID(), 0);
			m_ColorAttachments.push_back(tex);
		}

		//Depth and Stencil
		if (m_FramebufferDesc.Depth) 
		{
			TextureDesc desc;
			if (m_FramebufferDesc.TexDesc.Target == GL_TEXTURE_2D_MULTISAMPLE)
			{
				desc.Target = GL_TEXTURE_2D_MULTISAMPLE;
				desc.Samples = m_FramebufferDesc.TexDesc.Samples;
			}
			else
			{
				desc.Target = GL_TEXTURE_2D;
				desc.Samples = 1;
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_MAG_FILTER, GL_NEAREST));
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
				desc.Parameters.push_back(std::make_pair<uint32_t, uint32_t>(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL)); // GL_LESS
			}
			desc.Width = m_FramebufferDesc.Width;
			desc.Height = m_FramebufferDesc.Height;
			desc.Format = m_FramebufferDesc.DepthFormat;
			desc.MipLevels = 0;
			desc.IsDepth = true;

			m_DepthAttachment = Texture2D::Create(desc);
			glNamedFramebufferTexture(m_RendererID, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetTextureID(), 0);
		}
		unsigned int result = glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER);
		TITAN_CORE_ASSERT(result == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!", result);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::BlitTo(Framebuffer* fbo, uint32_t mask)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->GetFramebufferID());
		if ((mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT) {
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
		}
		glBlitFramebuffer(0, 0, m_FramebufferDesc.Width, m_FramebufferDesc.Height, 0, 0, m_FramebufferDesc.Width, m_FramebufferDesc.Height, mask, GL_NEAREST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
	}

	std::shared_ptr<Texture2D> OpenGLFramebuffer::GetColorAttachment(uint32_t index) const
	{
		if (index < m_ColorAttachments.size())
			return m_ColorAttachments[index];
		else
		{
			TITAN_CORE_ASSERT(false, "Index is not valid");
			return nullptr;
		}
	}

	void OpenGLFramebuffer::SetColorAttachment(uint32_t id, uint32_t index)
	{
		if (index < m_ColorAttachments.size())
			m_ColorAttachments[index]->SetTextureID(id);
		else
		{
			TITAN_CORE_ASSERT(false, "Index is not valid");
		}
	}
}