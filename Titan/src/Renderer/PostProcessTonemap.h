#pragma once

namespace Titan {

	class Texture2D;

	class PostProcessTonemap
	{
	public:
		void Init();
		void Render(std::shared_ptr<Texture2D>& sceneTex, std::shared_ptr<Texture2D>& sceneBloomBlur);

		void RenderDebug();
	};
	
}