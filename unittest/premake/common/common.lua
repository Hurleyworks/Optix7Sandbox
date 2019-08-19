
local REPO_ROOT = "../../../"
local ROOT = "../../"
	language "C++"

	defines{
		"NANOGUI_GLAD", "JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED", "POCO_NO_AUTOMATIC_LIBS",
		"_USE_MATH_DEFINES", "_ENABLE_EXTENDED_ALIGNED_STORAGE"
	}

	flags { "MultiProcessorCompile", "NoMinimalRebuild" }
	
	local CORE_DIR = REPO_ROOT .. "core/source/"
	local JAHLEY_DIR = REPO_ROOT .. "core/source/jahley/"
	local THIRD_PARTY_DIR = REPO_ROOT .. "thirdparty/"
	local MODULE_DIR = REPO_ROOT .. "modules/"
	local CUDA_INCLUDE_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/include"
	local CUDA_EXTRA_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/extras/cupti/include"
	local CUDA_LIB_DIR =  "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/lib/x64"
	local OPTIX_REPO_ROOT = "C:/ProgramData/NVIDIA Corporation"
	local OPTIX7_INCLUDE_DIR = OPTIX_REPO_ROOT .. "/OptiX SDK 7.0.0/include"
	includedirs
	{
		CORE_DIR,
		JAHLEY_DIR,
		MODULE_DIR,
		CUDA_INCLUDE_DIR,
		CUDA_EXTRA_DIR,
		OPTIX7_INCLUDE_DIR,
		THIRD_PARTY_DIR,
		THIRD_PARTY_DIR .. "nanogui/ext/glfw/include",
		THIRD_PARTY_DIR .. "nanogui/include",
		THIRD_PARTY_DIR .. "nanogui/ext/eigen",
		THIRD_PARTY_DIR .. "nanogui/ext/glad/include",
		THIRD_PARTY_DIR .. "nanogui/ext/nanovg/src",
		THIRD_PARTY_DIR .. "g3log/src",
		THIRD_PARTY_DIR .. "PocoFoundationLite/include",
		THIRD_PARTY_DIR .. "openexr/source",
		THIRD_PARTY_DIR .. "openexr/source/*",
		THIRD_PARTY_DIR .. "blosc/include",
		THIRD_PARTY_DIR .. "taskflow",
		THIRD_PARTY_DIR .. "stb/include",
		THIRD_PARTY_DIR .. "libigl/include",
		THIRD_PARTY_DIR .. "cs_signal/source",
		THIRD_PARTY_DIR .. "doctest",
	}
	
	links 
	{ 
		"Nanogui",
		"GLFW",
		"CoreLibrary",
		"g3log",
		"PocoFoundationLite",
		"advapi32",
		"IPHLPAPI",
		"openexr",
		"zlib",
		"zstd_static",
		"cudart_static",
		"cuda",
		"nvrtc",
		"stb",
		"cs_signal",
    }
	
	targetdir (ROOT .."builds/bin/" .. outputdir .. "/%{prj.name}")
	objdir (ROOT .. "builds/bin-int/" .. outputdir .. "/%{prj.name}")
	
	filter { "system:windows"}
		disablewarnings { 
			"5030", "4244", "4267", "4667", "4018", "4101", "4305", "4316",
		} 
		defines 
		{ 
			"POCO_OS_FAMILY_WINDOWS",
			"WIN32"
		}
		
	filter "configurations:Debug"
		defines { "DEBUG", "USE_DEBUG_EXCEPTIONS" }
		symbols "On"
		libdirs { THIRD_PARTY_DIR .. "builds/bin/" .. outputdir .. "/**",
				  THIRD_PARTY_DIR .. "precompiled/bin/" .. outputdir .. "/**",
				  REPO_ROOT .. "builds/bin/" .. outputdir .. "/**",
				  CUDA_LIB_DIR
		}
		
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		libdirs { THIRD_PARTY_DIR .. "builds/bin/" .. outputdir .. "/**",
				  THIRD_PARTY_DIR .. "precompiled/bin/" .. outputdir .. "/**",
				  REPO_ROOT .. "builds/bin/" .. outputdir .. "/**",
				  CUDA_LIB_DIR
		}
	
	  


	 		

