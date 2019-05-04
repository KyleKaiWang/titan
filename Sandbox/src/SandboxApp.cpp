#include <Titan.h>

class ExampleLayer : public Titan::Layer
{

public:

	ExampleLayer()
		:Layer("Example")
	{}

	void OnUpdate() override
	{

		TITAN_INFO("ExampleLayer::Update");
	}

	void OnEvent(Titan::Event& event) override
	{
		TITAN_TRACE("{0}", event);
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

