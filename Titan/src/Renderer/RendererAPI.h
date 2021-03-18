#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Titan {
	
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};

		enum class CompareEnum : uint8_t
		{
			LESS = 0,
			LEQUAL,
			EQUAL,
			GEQUAL,
			GREATER,
		};

		enum class BlendFuncEnum : uint8_t
		{
			ADDITION = 0,
			ALPHA_BLEND_1,
			ALPHA_BLEND_2,
			ONE_MINUS_SRC_APLHA
		};

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void DispatchCompute(uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z) = 0;
		virtual void PolyModeFill() = 0;
		virtual void PolyModeLine() = 0;
		virtual void BindDefaultFrameBuffer() = 0;
		virtual void TransferColorBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) = 0;
		virtual void TransferColorBit(uint32_t src, uint32_t src_tex, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_tex, uint32_t dest_w, uint32_t dest_h) = 0;
		virtual void TransferDepthBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) = 0;
		virtual void TransferStencilBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) = 0;
		virtual void Reverse_Z(bool b) = 0;
		virtual void DepthTest(bool test, bool write) = 0;
		virtual void DepthFunc(CompareEnum e) = 0;
		virtual void StencilTest(bool test, bool write) = 0;
		virtual void CullFace(bool enabled, bool front) = 0;
		virtual void Blend(bool enabled) = 0;
		virtual void BlendFunc(BlendFuncEnum e) = 0;
		virtual void DepthClamp(bool enabled) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}