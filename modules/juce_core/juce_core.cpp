/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/


#ifdef JUCE_CORE_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include "berserkpch.h"



#include "juce_core.h"

#include <locale>
#include <cctype>
#include <cstdarg>

#if ! JUCE_ANDROID
 #include <sys/timeb.h>
 #include <cwctype>
#endif

#if JUCE_WINDOWS
 #include <ctime>

 #if ! JUCE_MINGW
  #pragma warning (push)
  #pragma warning (disable: 4091)
  #include <Dbghelp.h>
  #pragma warning (pop)

  #if ! JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
   #pragma comment (lib, "DbgHelp.lib")
  #endif
 #endif

 #if JUCE_MINGW
  #include <ws2spi.h>
  #include <cstdio>
  #include <locale.h>
 #endif

#else
 #if JUCE_LINUX || JUCE_ANDROID
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/errno.h>
  #include <unistd.h>
  #include <netinet/in.h>
 #endif

 #if JUCE_LINUX
  #include <stdio.h>
  #include <langinfo.h>
  #include <ifaddrs.h>
  #include <sys/resource.h>

  #if JUCE_USE_CURL
   #include <curl/curl.h>
  #endif
 #endif

 #include <pwd.h>
 #include <fcntl.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
 #include <sys/time.h>
 #include <net/if.h>
 #include <sys/ioctl.h>

 #if ! JUCE_ANDROID
  #include <execinfo.h>
 #endif
#endif

#if JUCE_MAC || JUCE_IOS
 #include <xlocale.h>
 #include <mach/mach.h>
#endif

#if JUCE_ANDROID
 #include <ifaddrs.h>
 #include <android/log.h>
#endif

#undef check


//==============================================================================
#include "excludeFromBuild/containers/juce_AbstractFifo.cpp"
#include "excludeFromBuild/containers/juce_ArrayBase.cpp"
#include "excludeFromBuild/containers/juce_NamedValueSet.cpp"
#include "excludeFromBuild/containers/juce_OwnedArray.cpp"
#include "excludeFromBuild/containers/juce_PropertySet.cpp"
#include "excludeFromBuild/containers/juce_ReferenceCountedArray.cpp"
#include "excludeFromBuild/containers/juce_SparseSet.cpp"
#include "excludeFromBuild/containers/juce_Variant.cpp"
#include "excludeFromBuild/files/juce_DirectoryIterator.cpp"
#include "excludeFromBuild/files/juce_File.cpp"
#include "excludeFromBuild/files/juce_FileInputStream.cpp"
#include "excludeFromBuild/files/juce_FileOutputStream.cpp"
#include "excludeFromBuild/files/juce_FileSearchPath.cpp"
#include "excludeFromBuild/files/juce_TemporaryFile.cpp"
#include "excludeFromBuild/javascript/juce_JSON.cpp"
#include "excludeFromBuild/javascript/juce_Javascript.cpp"
#include "excludeFromBuild/containers/juce_DynamicObject.cpp"
#include "excludeFromBuild/logging/juce_FileLogger.cpp"
#include "excludeFromBuild/logging/juce_Logger.cpp"
#include "excludeFromBuild/maths/juce_BigInteger.cpp"
#include "excludeFromBuild/maths/juce_Expression.cpp"
#include "excludeFromBuild/maths/juce_Random.cpp"
#include "excludeFromBuild/memory/juce_MemoryBlock.cpp"
#include "excludeFromBuild/misc/juce_RuntimePermissions.cpp"
#include "excludeFromBuild/misc/juce_Result.cpp"
#include "excludeFromBuild/misc/juce_Uuid.cpp"
#include "excludeFromBuild/misc/juce_StdFunctionCompat.cpp"
#include "excludeFromBuild/misc/juce_ConsoleApplication.cpp"
#include "excludeFromBuild/network/juce_MACAddress.cpp"
#include "excludeFromBuild/network/juce_NamedPipe.cpp"
#include "excludeFromBuild/network/juce_Socket.cpp"
#include "excludeFromBuild/network/juce_IPAddress.cpp"
#include "excludeFromBuild/streams/juce_BufferedInputStream.cpp"
#include "excludeFromBuild/streams/juce_FileInputSource.cpp"
#include "excludeFromBuild/streams/juce_InputStream.cpp"
#include "excludeFromBuild/streams/juce_MemoryInputStream.cpp"
#include "excludeFromBuild/streams/juce_MemoryOutputStream.cpp"
#include "excludeFromBuild/streams/juce_SubregionStream.cpp"
#include "excludeFromBuild/system/juce_SystemStats.cpp"
#include "excludeFromBuild/text/juce_CharacterFunctions.cpp"
#include "excludeFromBuild/text/juce_Identifier.cpp"
#include "excludeFromBuild/text/juce_LocalisedStrings.cpp"
#include "excludeFromBuild/text/juce_String.cpp"
#include "excludeFromBuild/streams/juce_OutputStream.cpp"
#include "excludeFromBuild/text/juce_StringArray.cpp"
#include "excludeFromBuild/text/juce_StringPairArray.cpp"
#include "excludeFromBuild/text/juce_StringPool.cpp"
#include "excludeFromBuild/text/juce_TextDiff.cpp"
#include "excludeFromBuild/text/juce_Base64.cpp"
#include "excludeFromBuild/threads/juce_ReadWriteLock.cpp"
#include "excludeFromBuild/threads/juce_Thread.cpp"
#include "excludeFromBuild/threads/juce_ThreadPool.cpp"
#include "excludeFromBuild/threads/juce_TimeSliceThread.cpp"
#include "excludeFromBuild/time/juce_PerformanceCounter.cpp"
#include "excludeFromBuild/time/juce_RelativeTime.cpp"
#include "excludeFromBuild/time/juce_Time.cpp"
#include "excludeFromBuild/unit_tests/juce_UnitTest.cpp"
#include "excludeFromBuild/xml/juce_XmlDocument.cpp"
#include "excludeFromBuild/xml/juce_XmlElement.cpp"
#include "excludeFromBuild/zip/juce_GZIPDecompressorInputStream.cpp"
#include "excludeFromBuild/zip/juce_GZIPCompressorOutputStream.cpp"
#include "excludeFromBuild/zip/juce_ZipFile.cpp"
#include "excludeFromBuild/files/juce_FileFilter.cpp"
#include "excludeFromBuild/files/juce_WildcardFileFilter.cpp"

//==============================================================================
#if ! JUCE_WINDOWS
#include "excludeFromBuild/native/juce_posix_SharedCode.h"
#include "excludeFromBuild/native/juce_posix_NamedPipe.cpp"
#if ! JUCE_ANDROID || __ANDROID_API__ >= 24
 #include "excludeFromBuild/native/juce_posix_IPAddress.h"
#endif
#endif

//==============================================================================
#if JUCE_MAC || JUCE_IOS
#include "excludeFromBuild/native/juce_mac_Files.mm"
#include "excludeFromBuild/native/juce_mac_Network.mm"
#include "excludeFromBuild/native/juce_mac_Strings.mm"
#include "excludeFromBuild/native/juce_mac_SystemStats.mm"
#include "excludeFromBuild/native/juce_mac_Threads.mm"

//==============================================================================
#elif JUCE_WINDOWS
#include "excludeFromBuild/native/juce_win32_Files.cpp"
#include "excludeFromBuild/native/juce_win32_Network.cpp"
#include "excludeFromBuild/native/juce_win32_Registry.cpp"
#include "excludeFromBuild/native/juce_win32_SystemStats.cpp"
#include "excludeFromBuild/native/juce_win32_Threads.cpp"

//==============================================================================
#elif JUCE_LINUX
#include "excludeFromBuild/native/juce_linux_CommonFile.cpp"
#include "excludeFromBuild/native/juce_linux_Files.cpp"
#include "excludeFromBuild/native/juce_linux_Network.cpp"
#if JUCE_USE_CURL
 #include "excludeFromBuild/native/juce_curl_Network.cpp"
#endif
#include "excludeFromBuild/native/juce_linux_SystemStats.cpp"
#include "excludeFromBuild/native/juce_linux_Threads.cpp"

//==============================================================================
#elif JUCE_ANDROID
#include "excludeFromBuild/native/juce_linux_CommonFile.cpp"
#include "excludeFromBuild/native/juce_android_Files.cpp"
#include "excludeFromBuild/native/juce_android_Misc.cpp"
#include "excludeFromBuild/native/juce_android_Network.cpp"
#include "excludeFromBuild/native/juce_android_SystemStats.cpp"
#include "excludeFromBuild/native/juce_android_Threads.cpp"
#include "excludeFromBuild/native/juce_android_RuntimePermissions.cpp"

#endif

#include "excludeFromBuild/threads/juce_ChildProcess.cpp"
#include "excludeFromBuild/threads/juce_HighResolutionTimer.cpp"
#include "excludeFromBuild/network/juce_URL.cpp"
#include "excludeFromBuild/network/juce_WebInputStream.cpp"
#include "excludeFromBuild/streams/juce_URLInputSource.cpp"

//==============================================================================
#if JUCE_UNIT_TESTS
#include "excludeFromBuild/containers/juce_HashMap_test.cpp"
#endif

//==============================================================================
namespace juce
{
/*
    As the very long class names here try to explain, the purpose of this code is to cause
    a linker error if not all of your compile units are consistent in the options that they
    enable before including JUCE headers. The reason this is important is that if you have
    two cpp files, and one includes the juce headers with debug enabled, and the other doesn't,
    then each will be generating code with different memory layouts for the classes, and
    you'll get subtle and hard-to-track-down memory corruption bugs!
*/
#if JUCE_DEBUG
 this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode
    ::this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode() noexcept {}
#else
 this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode
    ::this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode() noexcept {}
#endif
}

