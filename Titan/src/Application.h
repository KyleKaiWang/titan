//
//  Application.hpp
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#pragma once

#include "Core.h"
#include "Window.h"
#include "../LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Titan {
    
    class TITAN_API Application
    {
    public:
        Application();
        virtual ~Application();
        
        void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
        
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
    };
    
    Application* CreateApplication();

}
