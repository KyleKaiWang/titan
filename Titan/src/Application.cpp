//
//  Application.cpp
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//

#include "tpch.h"
#include "Application.h"
#include "Log.h"

#include <GLFW/glfw3.h>


namespace Titan {

#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)
    
    Application::Application()
    {
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));
    }
    
    Application::~Application()
    {
    }
    
    void Application::Run()
    {
		while (m_Running)
		{
			//glClearColor(1, 0, 1, 1);
			//glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
    }

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

		TITAN_CORE_TRACE("{0}", e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
    
    
}


