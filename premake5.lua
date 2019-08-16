
-- https://github.com/JohannesMP/Premake-for-Beginners

workspace "Optix7Sandbox"
	architecture "x64"
	location ("builds")
	
if _ACTION == "vs2017" then
   location ("builds/VisualStudio2017")
end
if _ACTION == "vs2019" then
   location ("builds/VisualStudio2019")
end
if _ACTION == "vs2015" then
   location ("builds/VisualStudio2015")
end
	
	configurations 
	{ 
		"Debug", 
        "Release",
    }
	
	filter "configurations:Debug"    defines { "DEBUG" }  symbols  "On"
    filter "configurations:Release"  defines { "NDEBUG" } optimize "On"
    
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
local ROOT = "../../"

-- the core
project "CoreLibrary"
    kind "StaticLib"
    language "C++"
	defines { "NANOGUI_GLAD", "POCO_NO_AUTOMATIC_LIBS", "JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED", "NANOVG_GL3", "_ENABLE_EXTENDED_ALIGNED_STORAGE"}
	flags { "MultiProcessorCompile", "NoMinimalRebuild" }
	
	if _ACTION == "vs2019" then
		cppdialect "C++17"
		location "builds/VisualStudio2019/projects"
	end
	if _ACTION == "vs2017" then
		location "builds/VisualStudio2017/projects"
	end
	if _ACTION == "vs2015" then
		location "builds/VisualStudio2015/projects"
	end
	
	pchheader "berserkpch.h"
    pchsource "core/source/berserkpch.cpp"
    
	targetdir ("builds/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("builds/bin-int/" .. outputdir .. "/%{prj.name}")

	local SOURCE_DIR = "core/source/"
	local BERSERKO_DIR = "core/source/jahley/"
	local MODULE_ROOT = "modules/"

    files
    { 
      SOURCE_DIR .. "**.h", 
      SOURCE_DIR .. "**.hpp", 
      SOURCE_DIR .. "**.c",
      SOURCE_DIR .. "**.cpp",
      SOURCE_DIR .. "**.tpp",
	  
	  MODULE_ROOT .. "**.h",  
      MODULE_ROOT .. "**.cpp",
	  MODULE_ROOT .. "**.cu",
    }
	local THIRD_PARTY_DIR = "thirdparty/"
	includedirs
	{
		SOURCE_DIR,
		MODULE_ROOT,
		BERSERKO_DIR,
		THIRD_PARTY_DIR,
		THIRD_PARTY_DIR .. "openexr/source",
		THIRD_PARTY_DIR .. "openexr/source/*",
		THIRD_PARTY_DIR .. "nanogui/ext/glfw/include",
		THIRD_PARTY_DIR .. "nanogui/ext/eigen",
		THIRD_PARTY_DIR .. "nanogui/include",
		THIRD_PARTY_DIR .. "nanogui/ext/glad/include",
		THIRD_PARTY_DIR .. "nanogui/ext/nanovg/src",
		THIRD_PARTY_DIR .. "g3log/src",
		THIRD_PARTY_DIR .. "PocoFoundationLite/include",
	}
	
	filter {} -- clear filter!
	
	 -- must go after files and includedirs
    filter { "files:modules/**/excludeFromBuild/**.cpp"}
		flags {"ExcludeFromBuild"}
		
	filter "system:windows"
        staticruntime "On"
        systemversion "latest"
        disablewarnings { 
			"5030", "4244", "4267", "4667", "4018", "4101", "4305", "4316",
		} 
		
		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS",
			"_USE_MATH_DEFINES",
			"__WINDOWS_WASAPI__"
		}
		
		 -- setting up visual studio filters (basically virtual folders).
		vpaths 
		{
		  ["Header Files/*"] = { 
			SOURCE_DIR .. "**.h", 
			SOURCE_DIR .. "**.hxx", 
			SOURCE_DIR .. "**.hpp",
		  },
		  ["Source Files/*"] = { 
			SOURCE_DIR .. "**.c", 
			SOURCE_DIR .. "**.cxx", 
			SOURCE_DIR .. "**.cpp",
		  },
		}
	
	filter "configurations:Debug"
        defines {"USE_DEBUG_EXCEPTIONS"}
               
     
  
-- add sandbox projects here
include "sandbox/HelloWorld"
include "sandbox/HelloPtx"
include "sandbox/HelloWindow"


	