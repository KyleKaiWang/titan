#pragma once
#include <glm/glm.hpp>

namespace Titan {

	class Texture2D;

	class PostProcessBloom
	{
	public:
		void Init();
		void Render(std::shared_ptr<Texture2D>& emmisiveBuffer);
		std::shared_ptr<Texture2D> GetGaussianBlurTex();
		void RenderDebug();
	};
	
}