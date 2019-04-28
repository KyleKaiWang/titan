workspace "Titan"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Titan/thirdparty/GLFW/include"
IncludeDir["Glad"] = "Titan/thirdparty/Glad/include"
IncludeDir["ImGui"] = "Titan/thirdparty/imgui"
IncludeDir["glm"] = "Titan/thirdparty/glm"

include "Titan/thirdparty/GLFW"
include "Titan/thirdparty/Glad"
include "Titan/thirdparty/imgui"

project "Titan"
	location "Titan"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	pchheader "tpch.h"
	pchsource "Titan/src/tpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/thirdparty/glm/glm/**.hpp",
		"%{prj.name}/thirdparty/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/thirdparty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"TITAN_PLATFORM_WINDOWS",
			"TITAN_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "TITAN_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TITAN_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "TITAN_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Titan/thirdparty/spdlog/include",
		"Titan/src",
		"Titan/thirdparty",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Titan"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"TITAN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TITAN_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TITAN_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "TITAN_DIST"
		runtime "Release"
		optimize "On"