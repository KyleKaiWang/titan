#include <Titan.h>
#include "imgui/imgui.h"

class ExampleLayer : public Titan::Layer
{

public:

	ExampleLayer()
		:Layer("Example")
	{}

	void OnUpdate() override
	{
		if (Titan::Input::IsKeyPressed(TITAN_KEY_TAB))
		{
			TITAN_TRACE("Tab key is pressed (polling)!");
		}
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Titan::Event& event) override
	{
		if (event.GetEventType() == Titan::EventType::KeyPressed)
		{
			Titan::KeyPressedEvent& e = (Titan::KeyPressedEvent&)event;
			if (e.GetKeyCode() == TITAN_KEY_TAB)
			{
				TITAN_TRACE("Tab key is pressed (event)!");
			}
			TITAN_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
		PushLayer(new ExampleLayer());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

