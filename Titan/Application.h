//
//  Application.hpp
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#include "Core.h"

#endif /* Application_hpp */

namespace Titan {
    
    class TITAN_API Application
    {
    public:
        Application();
        virtual ~Application();
        
        void Run();
        
        Application* CreateApplication();
    }
}
