project "g3log"
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
        "src",
		"src/g3log",
    }

	files
	{
        "src/g3log/g3log.hpp",
        "src/filesink.cpp",
        "src/g2log.hpp",
        "src/g3log.cpp",
        "src/logcapture.cpp",
        "src/loglevels.cpp",
		"src/logmessage.cpp",
		"src/logworker.cpp",
		"src/time.cpp",
    }
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        disablewarnings { "4244", "5030" }
		characterset "MBCS"
        files
        {
            "src/crashhandler_windows.cpp",
			"src/stacktrace_windows.cpp",
        }

		defines 
		{ 
			"_WIN32",
			"_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
		}
	
	filter { "system:windows", "configurations:Release"}
		buildoptions "/MT"     

	filter { "system:windows", "configurations:Debug"}
		buildoptions "/MTd"  