
project "Nanogui"
	if _ACTION == "vs2019" then
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
    cppdialect "C++17"
    
    flags { "MultiProcessorCompile" }
	
	targetdir ("../builds/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../builds/bin-int/" .. outputdir .. "/%{prj.name}")
	
	defines { "NANOGUI_GLAD"}
	
	includedirs
	{
        "include",
		"include/nanogui",
		"ext/eigen",
		"ext/glad/include",
		"ext/nanovg/src",
		"ext/glfw/include/GLFW",
		"ext/glfw/include",
    }

	files
	{
        "include/nanogui/nanogui.h",
		"include/nanogui/nanogui_resources.h",
        "src/button.cpp",
        "src/checkbox.cpp",
        "src/colorpicker.cpp",
        "src/colorwheel.cpp",
        "src/combobox.cpp",
        "src/common.cpp",
        "src/glcanvas.cpp",
		"src/glutil.cpp",
		"src/graph.cpp",
		"src/imagepanel.cpp",
		"src/imageview.cpp",
		"src/label.cpp",
		"src/layout.cpp",
		"src/messagedialog.cpp",
		"src/popup.cpp",
		"src/popupbutton.cpp",
		"src/progressbar.cpp",
		"src/screen.cpp",
		"src/serializer.cpp",
		"src/slider.cpp",
		"src/stackedwidget.cpp",
		"src/tabheader.cpp",
		"src/tabwidget.cpp",
		"src/textbox.cpp",
		"src/theme.cpp",
		"src/vscrollpanel.cpp",
		"src/widget.cpp",
		"src/window.cpp",
		"src/nanogui_resources.cpp",
		
		"ext/glad/src/glad.c",
		"ext/nanovg/src/nanovg.c"
    }
	
	filter "configurations:Release"
        optimize "On"
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        disablewarnings { "4244", "4996" }
		characterset "MBCS"
        files
        {
           
        }

		defines 
		{ 
			"_WIN32",
			"_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
		}
