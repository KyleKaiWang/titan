#include "tpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Titan {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDesc& spec)
		: m_Desc(spec)
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		for (int i = 0; i < m_Desc.nrColorAttachment; ++i)
		{
			std::shared_ptr<Texture2D> tex = Texture2D::Create(m_Desc.TexDesc);
			glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0 + i, tex->GetTextureID(), 0);
			m_ColorAttachments.push_back(tex);
		}
		
		//Depth and Stencil
		TextureDesc desc;
		desc.Width = m_Desc.Width;
		desc.Height = m_Desc.Height;
		desc.Format = GL_DEPTH24_STENCIL8;

		std::shared_ptr<Texture2D> depthTex = Texture2D::Create(m_Desc.TexDesc);
		glNamedFramebufferTexture(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, depthTex->GetTextureID(), 0);
		m_DepthAttachment = depthTex;

		TITAN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
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
}