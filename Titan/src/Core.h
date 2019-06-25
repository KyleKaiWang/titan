#pragma once

#ifdef TITAN_PLATFORM_WINDOWS
#if TITAN_DYNAMIC_LINK
    #ifdef TITAN_BUILD_DLL
        #define TITAN_API __declspec(dllexport)
    #else
        #define TITAN_API __declspec(dllimport)
    #endif
#else
	#define TITAN_API
#endif
#else
    #error Titan only work at windows
#endif

#ifdef TITAN_DEBUG
	#define TITAN_ENABLE_ASSERTS
#endif

#ifdef TITAN_ENABLE_ASSERTS
	#define TITAN_ASSERT(x, ...) { if(!(x)) { TITAN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TITAN_CORE_ASSERT(x, ...) { if(!(x)) { TITAN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TITAN_ASSERT(x, ...)
	#define TITAN_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)

#define TITAN_BIND_EVENT_FUNC(func) std::bind(&func, this, std::placeholders::_1)