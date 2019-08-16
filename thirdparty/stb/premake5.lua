
project "stb"
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
	
	targetdir ("../builds/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../builds/bin-int/" .. outputdir .. "/%{prj.name}")
	
	defines { "POCO_STATIC", "POCO_NO_AUTOMATIC_LIBS"}

	includedirs
	{
        "include",
    }

	files
	{
		"include/stb.cpp", 
		
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
			
