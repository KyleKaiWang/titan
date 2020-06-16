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
		std::vector<std::pair<uint32_t, uint32_t>> Parameters;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual uint32_t GetTextureID() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(TextureDesc texDesc);
	};

	class TextureCube
	{
	public:
		virtual ~TextureCube() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;

		static Ref<TextureCube> Create(std::vector<std::string> facesPath);
	};
}