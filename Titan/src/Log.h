//
//  Log.h
//  Titan
//
//  Created by Kyle Wang on 28/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//
#pragma once

#include "tpch.h"
#include "Core.h"
#include "spdlog/spdlog.h"

namespace Titan {
    
    //TODO: class TITAN_API Log
    class TITAN_API Log
    {
    public:
        
        static void Init();
        
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        
    private:
        
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}
