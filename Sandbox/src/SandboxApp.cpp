#include "SandboxClusterShading.h"
#include "EntryPoint.h"

class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
        PushLayer(new SandboxClusterShading());
    }
    
    ~Sandbox()
    {
    }
};

Titan::Application* Titan::CreateApplication()
{
    return new Sandbox();
}

