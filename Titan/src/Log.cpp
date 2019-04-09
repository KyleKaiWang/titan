//
//  Log.cpp
//  Titan
//
//  Created by Kyle Wang on 28/03/2019.
//  Copyright © 2019 Kyle Wang. All rights reserved.
//

#include "Log.h"

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

// Core log macros
#define T_CORE_TRACE(...)    ::Titan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define T_CORE_INFO(...)     ::Titan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define T_CORE_WARN(...)     ::Titan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define T_CORE_ERROR(...)    ::Titan::Log::GetCoreLogger()->error(__VA_ARGS__)
#define T_CORE_FATAL(...)    ::Titan::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define T_TRACE(...)          ::Titan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define T_INFO(...)          ::Titan::Log::GetClientLogger()->info(__VA_ARGS__)
#define T_WARN(...)          ::Titan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define T_ERROR(...)          ::Titan::Log::GetClientLogger()->error(__VA_ARGS__)
#define T_FATAL(...)          ::Titan::Log::GetClientLogger()->fatal(__VA_ARGS__)
