//
//  Log.cpp
//  Titan
//
//  Created by Kyle Wang on 28/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//

#include "Log.h"
#include "tpch.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Titan {
    
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    
    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("Titan");
        s_CoreLogger->set_level(spdlog::level::trace);
        
        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);
    }
}

