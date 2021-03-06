#pragma once

#include <memory>

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

namespace Titan {
	
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}