
project "tinygltf"
	if _ACTION == "vs2019" then
		cppdialect "C++17"
		location ("../builds/VisualStudio2019/projects")
	end
	if _ACTION == "vs2017" then
		location ("../builds/VisualStudio2017/projects")
	end
	if _ACTION == "vs2015" then
		location ("../builds/VisualStudio2015/projects")
	end
    kind "StaticLib"
    language "C++"
    
    flags { "MultiProcessorCompile" }
	defines 
		{ 
            "TINYGLTF_NO_INCLUDE_JSON",
			"TINYGLTF_NO_INCLUDE_STB_IMAGE", 
			"TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE",
			"TINYGLTF_USE_CPP14",
		}
	
	targetdir ("../builds/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../builds/bin-int/" .. outputdir .. "/%{prj.name}")
	
	includedirs
	{
        "include",
    }

	files
	{
		"include/tiny_gltf.cpp", 
    }
	
	
	filter "configurations:Release"
        optimize "On"
    
	filter "system:windows"
        staticruntime "On"
        disablewarnings { "4244" }
        files
        {
			
        }

		defines 
		{ 
            "_CRT_SECURE_NO_WARNINGS"
		}
		filter { "system:windows", "configurations:Release"}
			buildoptions "/MT"     

		filter { "system:windows", "configurations:Debug"}
			buildoptions "/MTd"  
			
