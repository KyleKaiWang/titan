#include "SandboxDeferredRendering.h"
#include "EntryPoint.h"

class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
        PushLayer(new SandboxDeferredRendering());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

