#include "tpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Titan {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		
		glGenerateTextureMipmap(GL_TEXTURE_2D);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		TITAN_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;
		
		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}

		// OpenGL 4.5
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		glGenerateTextureMipmap(GL_TEXTURE_2D);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(TextureDesc texDesc)
		: m_Width(texDesc.Width), m_Height(texDesc.Height), m_InternalFormat(texDesc.Format)
	{
		m_DataFormat = texDesc.Format;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, texDesc.Width, texDesc.Height);

		if(texDesc.MipLevels != 0) 
			glGenerateTextureMipmap(GL_TEXTURE_2D);
		else  
			glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		TITAN_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	OpenGLTextureCube::OpenGLTextureCube(std::vector<std::string> faces)
	{
		//glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, GL_RGBA8, 2048, 2048);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		int width, height, channels;
		
		stbi_set_flip_vertically_on_load(0);
		
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			stbi_uc* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
			TITAN_CORE_ASSERT(data, "Failed to load image!");
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//glTextureSubImage3D(m_RendererID, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			
			stbi_image_free(data);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}