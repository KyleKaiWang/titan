#pragma once

#include "RendererAPI.h"

namespace Titan {

	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void DispatchCompute(uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z)
		{
			s_RendererAPI->DispatchCompute(num_groups_x, num_groups_y, num_groups_z);
		}

		inline static void PolyModeFill()
		{
			s_RendererAPI->PolyModeFill();
		}

		inline static void PolyModeLine()
		{
			s_RendererAPI->PolyModeLine();
		}

		inline static void BindDefaultFrameBuffer()
		{
			s_RendererAPI->BindDefaultFrameBuffer();
		}

		inline static void TransferColorBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h)
		{
			s_RendererAPI->TransferColorBit(src, src_w, src_h, dest, dest_w, dest_h);
		}

		inline static void TransferColorBit(uint32_t src, uint32_t src_tex, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_tex, uint32_t dest_w, uint32_t dest_h)
		{
			s_RendererAPI->TransferColorBit(src, src_tex, src_w, src_h, dest, dest_tex, dest_w, dest_h);
		}

		inline static void TransferDepthBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h)
		{
			s_RendererAPI->TransferDepthBit(src, src_w, src_h, dest, dest_w, dest_h);
		}

		inline static void TransferStencilBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h)
		{
			s_RendererAPI->TransferStencilBit(src, src_w, src_h, dest, dest_w, dest_h);
		}

		inline static void Reverse_Z(bool b)
		{
			s_RendererAPI->Reverse_Z(b);
		}

		inline static void DepthTest(bool test, bool write)
		{
			s_RendererAPI->DepthTest(test, write);
		}

		inline static void DepthFunc(RendererAPI::CompareEnum e)
		{
			s_RendererAPI->DepthFunc(e);
		}

		inline static void StencilTest(bool test, bool write)
		{
			s_RendererAPI->StencilTest(test, write);
		}

		inline static void CullFace(bool enabled, bool front)
		{
			s_RendererAPI->CullFace(enabled, front);
		}

		inline static void Blend(bool enabled)
		{
			s_RendererAPI->Blend(enabled);
		}

		inline static void BlendFunc(RendererAPI::BlendFuncEnum e)
		{
			s_RendererAPI->BlendFunc(e);
		}

		inline static void DepthClamp(bool enabled)
		{
			s_RendererAPI->DepthClamp(enabled);
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};

}