#include <Titan.h>

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
		PushOverlay(new Titan::ImGuiLayer());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

