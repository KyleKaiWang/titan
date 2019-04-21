//
//  Application.cpp
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//

#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"



namespace Titan {
    
    Application::Application()
    {
		m_Window = std::unique_ptr<Window>(Window::Create());
    }
    
    Application::~Application()
    {
    }
    
    void Application::Run()
    {
		//WindowResizeEvent e(1280, 720);
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
    }
    
    
}


