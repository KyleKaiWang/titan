//
//  EntryPoint.h
//  Titan
//
//  Created by Kyle Wang on 17/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#pragma once

#ifdef TITAN_PLATFORM_WINDOWS

extern Titan::Application* Titan::CreateApplication();


int main(int argc, const char * argv[])
{
    auto app = Titan::CreateApplication();
    app->Run();
    delete app;
}

#endif
