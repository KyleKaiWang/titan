workspace "Titan"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Titan/thirdparty/GLFW/include"
IncludeDir["Glad"] = "Titan/thirdparty/Glad/include"
IncludeDir["ImGui"] = "Titan/thirdparty/imgui"
IncludeDir["glm"] = "Titan/thirdparty/glm"
IncludeDir["stb_image"] = "Titan/thirdparty/stb_image"
IncludeDir["assimp"] = "Titan/thirdparty/assimp/include"

group "Dependencies"
	include "Titan/thirdparty/GLFW"
	include "Titan/thirdparty/Glad"
	include "Titan/thirdparty/imgui"
	
group ""
project "Titan"
	location "Titan"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"%{prj.name}/thirdparty/stb_image/**.h",
		"%{prj.name}/thirdparty/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/thirdparty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Titan/thirdparty/assimp/lib/assimp-vc142-mtd.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TITAN_PLATFORM_WINDOWS",
			"TITAN_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "TITAN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TITAN_RELEASE"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		systemversion "latest"

		defines
		{
			"TITAN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TITAN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TITAN_RELEASE"
		runtime "Release"
		optimize "on"