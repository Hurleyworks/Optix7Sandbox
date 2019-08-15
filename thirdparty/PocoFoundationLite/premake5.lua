
project "PocoFoundationLite"
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
		"include/Poco/*.h", 
		"src/Foundation.cpp",
		"src/File.cpp",
		"src/Path.cpp",
		"src/Bugcheck.cpp",
		"src/BinaryReader.cpp",
		"src/BinaryWriter.cpp",
		"src/exception.cpp",
		"src/StreamTokenizer.cpp",
		"src/UnicodeConverter.cpp",
		"src/StringTokenizer.cpp",
		"src/Environment.cpp",
		"src/Debugger.cpp",
		"src/Timestamp.cpp",
		"src/DirectoryIterator.cpp",
		"src/NestedDiagnosticContext.cpp",
		"src/Thread.cpp",
		"src/Ascii.cpp",
		"src/TextConverter.cpp",
		"src/TextIterator.cpp",
		"src/UTF8Encoding.cpp",
		"src/UTF16Encoding.cpp",
		"src/UTF32Encoding.cpp",
		"src/UTF8String.cpp",
		"src/AtomicCounter.cpp",
		"src/TextEncoding.cpp",
		"src/ThreadLocal.cpp",
		"src/Windows1250Encoding.cpp",
		"src/Windows1251Encoding.cpp",
		"src/Windows1252Encoding.cpp",
		"src/ASCIIEncoding.cpp",
		"src/Latin1Encoding.cpp",
		"src/Latin2Encoding.cpp",
		"src/Latin9Encoding.cpp",
		"src/RWLock.cpp",
		"src/Mutex.cpp",
		"src/ErrorHandler.cpp",
		"src/Event.cpp",
		"src/NumberFormatter.cpp",
		"src/NumberParser.cpp",
		"src/NumericString.cpp",
		"src/Runnable.cpp",
		"src/Format.cpp",
		"src/RefCountedObject.cpp",
		"src/Logger.cpp",
		"src/ThreadPool.cpp",
		"src/Channel.cpp",
		"src/Message.cpp",
		"src/Process.cpp",
		"src/NamedEvent.cpp",
		"src/Configurable.cpp",
		"src/LoggingRegistry.cpp",
		"src/PipeImpl.cpp",
		"src/Pipe.cpp",
		"src/Error.cpp",
		"src/DateTime.cpp",
		"src/DateTimeFormat.cpp",
		"src/DateTimeFormatter.cpp",
		"src/DateTimeParser.cpp",
		"src/Timespan.cpp",
		"src/Timezone.cpp",
		"src/RegularExpression.cpp",
		"src/pcre_byte_order.c",
		"src/pcre_chartables.c",
		"src/pcre_compile.c",
		"src/pcre_config.c",
		"src/pcre_dfa_exec.c",
		"src/pcre_exec.c",
		"src/pcre_fullinfo.c",
		"src/pcre_get.c",
		"src/pcre_globals.c",
		"src/pcre_jit_compile.c",
		"src/pcre_maketables.c",
		"src/pcre_newline.c",
		"src/pcre_ord2utf8.c",
		"src/pcre_refcount.c",
		"src/pcre_string_utils.c",
		"src/pcre_study.c",
		"src/pcre_tables.c",
		"src/pcre_ucd.c",
		"src/pcre_valid_utf8.c",
		"src/pcre_version.c",
		"src/pcre_xclass.c",
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
			"_WIN32",
			"_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
			"POCO_OS_FAMILY_WINDOWS"
		}
		filter { "system:windows", "configurations:Release"}
			buildoptions "/MT"     

		filter { "system:windows", "configurations:Debug"}
			buildoptions "/MTd"  
			
