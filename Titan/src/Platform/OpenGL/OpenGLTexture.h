#pragma once
#include "Renderer/Texture.h"

namespace Titan {
	
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t  width, uint32_t  height);
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(TextureDesc texDesc);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void SetData(void* data, uint32_t size) override;
		virtual uint32_t GetTextureID() const override { return m_RendererID; }

	private:
		std::string m_Path;
		uint32_t m_RendererID;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(std::vector<std::string> faces);
		OpenGLTextureCube(uint32_t width, uint32_t height, uint32_t internalFormat, int levels = 0);
		virtual ~OpenGLTextureCube();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual uint32_t GetTextureID() const override { return m_RendererID; }
		virtual void SetData(void* data, uint32_t size) override {}

	private:
		uint32_t m_RendererID;
		uint32_t m_Width, m_Height;
		
	};
}