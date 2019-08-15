
workspace "3rdParty"
	architecture "x64"
	location "builds"
	
	if _ACTION == "vs2017" then
	   location "builds/VisualStudio2017"
	end
	if _ACTION == "vs2019" then
	   location "builds/VisualStudio2019"
	end
	if _ACTION == "vs2015" then
	   location "builds/VisualStudio2015"
	end
	
	configurations 
	{ 
		"Debug", 
        "Release",
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
-- create projects 
include "nanogui"
include "nanogui/ext/glfw"
include "g3log"
include "PocoFoundationLite"
include "openexr"
include "cs_signal"
