//
//  Core.h
//  Titan
//
//  Created by Kyle Wang on 16/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#pragma once

#ifdef TITAN_PLATFORM_WINDOWS
    #ifdef TITAN_BUILD_DLL
        #define TITAN_API __deslspec(dllimport)
    #else
        #define TITAN_API __deslspec(dllimport)
#else
    #error Titan only work at windows
#endif
