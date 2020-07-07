#pragma once

#include <string>
#include "Core.h"

namespace Titan {
	
	
	struct TextureDesc
	{
		uint32_t Width;
		uint32_t Height;
		uint32_t MipLevels;
		uint32_t Format;
		bool IsDepth = false;
		std::vector<std::pair<uint32_t, uint32_t>> Parameters;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const  { return m_Width; }
		virtual uint32_t GetHeight() const  { return m_Height; }
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual uint32_t GetTextureID() const = 0;

		bool isHDR() const { return m_HDR; }
		int BytesPerPixel() const { return m_DataFormat * (m_HDR ? sizeof(float) : sizeof(unsigned char)); }
		int Pitch() const { return m_Width * BytesPerPixel(); }

		template<typename T>
		const T* Pixels() const
		{
			assert(m_DataFormat * sizeof(T) == BytesPerPixel());
			return reinterpret_cast<const T*>(m_Pixels.get());
		}

		template<>
		const void* Pixels() const
		{
			return reinterpret_cast<const void*>(m_Pixels.get());
		}

		template<typename T> static constexpr T NumMipmapLevels(T width, T height)
		{
			T levels = 1;
			while ((width | height) >> levels) {
				++levels;
			}
			return levels;
		}

	protected:
		bool m_HDR = false;
		uint32_t m_Width, m_Height;
		uint32_t m_InternalFormat, m_DataFormat;
		std::unique_ptr<unsigned char> m_Pixels;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(TextureDesc texDesc);
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(std::vector<std::string> facesPath);
		static Ref<TextureCube> Create(uint32_t width, uint32_t height, uint32_t internalFormat, int levels = 0);
		inline int GetLevels() const { return m_Levels; }
	protected:
		int m_Levels;
	};
}