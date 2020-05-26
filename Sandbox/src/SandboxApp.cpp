#include "SandboxGraphics.h"
#include "SandboxDeferred.h"
#include "Sandbox2D.h"
#include "EntryPoint.h"

class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
		//Test Graphics Feature
		//PushLayer(new SandboxGraphics());

        //Test Graphics Feature
        PushLayer(new SandboxDeferred());
		
		//Test 2D feature
		//PushLayer(new Sandbox2D());

		//Test 3D feature
		//PushLayer(new Sandbox3D());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

