#pragma once

#include "tpch.h"
#include "Core.h"
#include "Events/Event.h"

namespace Titan {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		
		WindowProps(const std::string& title = "Titan",
			unsigned int width = 1280,
			unsigned int height = 720)
			:Title(title), Width(width), Height(height)
		{

		}
	};

	class TITAN_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() = default;
		
		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		//Window attributes
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void GetDesktopResolution(unsigned int& width, unsigned int& height) = 0;
		static Window* Create(const WindowProps& props = WindowProps());
		

	};
}