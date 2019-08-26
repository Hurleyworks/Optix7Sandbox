project "gdt"
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
	
	includedirs
	{
		
        "source",
    }

	files
	{
       "source/**.h", 
       "source/**.hpp", 
       "source/**.c",
       "source/**.cpp",
    }
    
	filter "system:windows"
	    staticruntime "On"
        systemversion "latest"
        disablewarnings { "4244", "5030" }
		characterset "MBCS"
       
		defines 
		{ 
			"_WIN32",
			"_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
			"NOMINMAX"
		}
	
