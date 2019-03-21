//
//  main.cpp
//  Sandbox
//
//  Created by Kyle Wang on 14/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//

#include <Titan.h>
#include <iostream>


class Sandbox : public Titan::Application
{
public:
    Sandbox()
    {
        
    }
    
    ~Sandbox()
    {
        
    }
}

Titan::Application* CreateApplication()
{
    return new Sandbox();
}
