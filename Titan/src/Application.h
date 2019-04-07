//
//  Application.hpp
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#include "Core.h"

namespace Titan {
    //TODO: class TITAN_API Application
    class Application
    {
    public:
        Application();
        virtual ~Application();
        
        void Run();
        
    };
    
    Application* CreateApplication();

}
