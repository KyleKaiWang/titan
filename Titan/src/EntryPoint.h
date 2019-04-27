//
//  EntryPoint.h
//  Titan
//
//  Created by Kyle Wang on 17/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#pragma once

#ifdef TITAN_PLATFORM_WINDOWS

//TODO: fix it
//#include "Log.h"
//#include "Titan.h"

extern Titan::Application* Titan::CreateApplication();


int main(int argc, char** argv)
{
    Titan::Log::Init();
	TITAN_CORE_WARN("Initail Log!");
    
    auto app = Titan::CreateApplication();
    app->Run();
    delete app;
    
    return 0;
}

#endif
