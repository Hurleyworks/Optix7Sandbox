local ROOT = "../../"

project  "Instancing"
	if _ACTION == "vs2019" then
		cppdialect "C++17"
		location (ROOT .. "builds/VisualStudio2019/projects")
    end
	if _ACTION == "vs2017" then
		location (ROOT .. "builds/VisualStudio2017/projects")
    end
	if _ACTION == "vs2015" then
		location (ROOT .. "builds/VisualStudio2015/projects")
    end
	
	kind "ConsoleApp"
	
	local RESOURCES_ROOT = "../../resources/Instancing/"
	local FRAMEWORK_ROOT = "../../framework/"
	local ACTIVE_OPTIX7 = FRAMEWORK_ROOT .. "optix7_core/";
	local SOURCE_DIR = "source/*"
    files
    { 
      SOURCE_DIR .. "**.h", 
      SOURCE_DIR .. "**.hpp", 
      SOURCE_DIR .. "**.c",
      SOURCE_DIR .. "**.cpp",
	  
	  ACTIVE_OPTIX7 ..  "**.h", 
	  ACTIVE_OPTIX7 .. "**.cpp",
    }
	
	includedirs
	{
		FRAMEWORK_ROOT,
		--RESOURCES_ROOT .. "cuda",
	}
	
	defines {"OPTIX_APP"}
	
	filter "system:windows"
		staticruntime "On"
		systemversion "latest"
		defines {"_CRT_SECURE_NO_WARNINGS", "__WINDOWS_WASAPI__"}
		disablewarnings { "5030" , "4305", "4316", "4267"}
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
		
	filter {} -- clear filter!
	filter { "files:../../framework/**/excludeFromBuild/**.cpp"}
	flags {"ExcludeFromBuild"}
	filter {} -- clear filter!
	
-- add settings common to all project
dofile("../../premake/common/common.lua")

