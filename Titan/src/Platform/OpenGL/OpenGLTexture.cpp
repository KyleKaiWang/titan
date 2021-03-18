#include "tpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace Titan {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glGenerateTextureMipmap(m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		uint32_t whiteTextureData = 0xffffffff;
		SetData(&whiteTextureData, sizeof(uint32_t));
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	{
		Texture::m_Path = path;
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		
		if (stbi_is_hdr(path.c_str())) 
		{
			float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			TITAN_CORE_ASSERT(data, "Failed to load HDR image!");
			m_HDR = true;
			m_Pixels.reset(reinterpret_cast<unsigned char*>(data));
		}
		else
		{
			stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			TITAN_CORE_ASSERT(data, "Failed to load image!");
			m_HDR = false;
			m_Pixels.reset(data);
		}
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
		else if (channels == 2)
		{
			m_InternalFormat = GL_RG8;
			m_DataFormat = GL_RG;
		}
		else if (channels == 1)
		{
			m_InternalFormat = GL_R8;
			m_DataFormat = GL_RED;
		}
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		if (!m_HDR)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, Pixels<unsigned char>());
			
			glGenerateTextureMipmap(m_RendererID);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			auto err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}
		else
		{
			m_InternalFormat = GL_RGB16F;
			m_DataFormat = GL_RGB;
			glTextureStorage2D(m_RendererID, NumMipmapLevels(m_Width, m_Height), m_InternalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_FLOAT, Pixels<float>());
			auto err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		}
		//stbi_image_free(data);
		
	}

	OpenGLTexture2D::OpenGLTexture2D(TextureDesc texDesc)
	{
		m_Width = texDesc.Width;
		m_Height = texDesc.Height;
		m_DataFormat = texDesc.Format;
		m_InternalFormat = texDesc.Format;

		if (texDesc.Target == GL_TEXTURE_2D_MULTISAMPLE)
		{
			glCreateTextures(texDesc.Target, 1, &m_RendererID);
			glTextureStorage2DMultisample(m_RendererID, texDesc.Samples, texDesc.Format, m_Width, m_Height, GL_TRUE);
			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		} 
		else
		{
			{
				int err = glGetError();
				TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
			}
			glCreateTextures(texDesc.Target, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, texDesc.Width, texDesc.Height);
			if(texDesc.MipLevels != 0) 
				glGenerateTextureMipmap(m_RendererID);
			else  
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAX_LEVEL, 0);

			float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
			{
				int err = glGetError();
				TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
			}
		}
		
		for (auto pair : texDesc.Parameters) {
			glTextureParameteri(m_RendererID, pair.first, pair.second);
			
			{
				int err = glGetError();
				TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
			}
		}
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
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		int width, height, channels;
		
		stbi_set_flip_vertically_on_load(0);
		
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			stbi_uc* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
			TITAN_CORE_ASSERT(data, "Failed to load image!");
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			
			stbi_image_free(data);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		int err = glGetError();
		TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
	}

	OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
			int err = glGetError();
			TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_DEPTH_COMPONENT, width, height, 0,
				GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		err = glGetError();
		TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
	}

	OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height, uint32_t internalFormat, int levels)
	{
		m_Width = width;
		m_Height = height;
		m_InternalFormat = internalFormat;
		m_Levels = (levels > 0) ? levels : Texture::NumMipmapLevels(width, height);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, m_Levels, internalFormat, width, height);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_Levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);

		int err = glGetError();
		TITAN_CORE_ASSERT(err == 0, "GL Error code", err);
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