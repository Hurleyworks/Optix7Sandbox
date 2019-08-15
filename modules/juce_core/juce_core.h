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





#pragma once
#define JUCE_CORE_H_INCLUDED

//==============================================================================
#ifdef _MSC_VER
 #pragma warning (push)
 // Disable warnings for long class names, padding, and undefined preprocessor definitions.
 #pragma warning (disable: 4251 4786 4668 4820)
 #ifdef __INTEL_COMPILER
  #pragma warning (disable: 1125)
 #endif
#endif

#include "excludeFromBuild/system/juce_TargetPlatform.h"

//==============================================================================
/** Config: JUCE_FORCE_DEBUG

    Normally, JUCE_DEBUG is set to 1 or 0 based on compiler and project settings,
    but if you define this value, you can override this to force it to be true or false.
*/
#ifndef JUCE_FORCE_DEBUG
 //#define JUCE_FORCE_DEBUG 0
#endif

//==============================================================================
/** Config: JUCE_LOG_ASSERTIONS

    If this flag is enabled, the jassert and jassertfalse macros will always use Logger::writeToLog()
    to write a message when an assertion happens.

    Enabling it will also leave this turned on in release builds. When it's disabled,
    however, the jassert and jassertfalse macros will not be compiled in a
    release build.

    @see jassert, jassertfalse, Logger
*/
#ifndef JUCE_LOG_ASSERTIONS
 #if JUCE_ANDROID
  #define JUCE_LOG_ASSERTIONS 1
 #else
  #define JUCE_LOG_ASSERTIONS 0
 #endif
#endif

//==============================================================================
/** Config: JUCE_CHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the LeakedObjectDetector
    class and the JUCE_LEAK_DETECTOR macro for more details about enabling leak checking for specific classes.
*/
#if JUCE_DEBUG && ! defined (JUCE_CHECK_MEMORY_LEAKS)
 #define JUCE_CHECK_MEMORY_LEAKS 1
#endif

//==============================================================================
/** Config: JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES

    In a Visual C++  build, this can be used to stop the required system libs being
    automatically added to the link stage.
*/
#ifndef JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
 #define JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES 0
#endif

/** Config: JUCE_INCLUDE_ZLIB_CODE
    This can be used to disable Juce's embedded 3rd-party zlib code.
    You might need to tweak this if you're linking to an external zlib library in your app,
    but for normal apps, this option should be left alone.

    If you disable this, you might also want to set a value for JUCE_ZLIB_INCLUDE_PATH, to
    specify the path where your zlib headers live.
*/
#ifndef JUCE_INCLUDE_ZLIB_CODE
 #define JUCE_INCLUDE_ZLIB_CODE 1
#endif

#ifndef JUCE_ZLIB_INCLUDE_PATH
 #define JUCE_ZLIB_INCLUDE_PATH <zlib.h>
#endif

/** Config: JUCE_USE_CURL
    Enables http/https support via libcurl (Linux only). Enabling this will add an additional
    run-time dynamic dependency to libcurl.

    If you disable this then https/ssl support will not be available on linux.
*/
#ifndef JUCE_USE_CURL
 #if JUCE_LINUX
  #define JUCE_USE_CURL 1
 #else
  #define JUCE_USE_CURL 0
 #endif
#endif

/** Config: JUCE_LOAD_CURL_SYMBOLS_LAZILY
    If enabled, JUCE will load libcurl lazily when required (for example, when WebInputStream
    is used). Enabling this flag may also help with library dependency erros as linking
    libcurl at compile-time may instruct the linker to hard depend on a specific version
    of libcurl. It's also useful if you want to limit the amount of JUCE dependencies and
    you are not using WebInputStream or the URL classes.
*/
#ifndef JUCE_LOAD_CURL_SYMBOLS_LAZILY
 #define JUCE_LOAD_CURL_SYMBOLS_LAZILY 0
#endif


/** Config: JUCE_CATCH_UNHANDLED_EXCEPTIONS
    If enabled, this will add some exception-catching code to forward unhandled exceptions
    to your JUCEApplicationBase::unhandledException() callback.
*/
#ifndef JUCE_CATCH_UNHANDLED_EXCEPTIONS
 //#define JUCE_CATCH_UNHANDLED_EXCEPTIONS 1
#endif

/** Config: JUCE_ALLOW_STATIC_NULL_VARIABLES
    If disabled, this will turn off dangerous static globals like String::empty, var::null, etc
    which can cause nasty order-of-initialisation problems if they are referenced during static
    constructor code.
*/
#ifndef JUCE_ALLOW_STATIC_NULL_VARIABLES
 #define JUCE_ALLOW_STATIC_NULL_VARIABLES 1
#endif

/** Config: JUCE_STRICT_REFCOUNTEDPOINTER
    If enabled, this will make the ReferenceCountedObjectPtr class stricter about allowing
    itself to be cast directly to a raw pointer. By default this is disabled, for compatibility
    with old code, but if possible, you should always enable it to improve code safety!
*/
#ifndef JUCE_STRICT_REFCOUNTEDPOINTER
 #define JUCE_STRICT_REFCOUNTEDPOINTER 0
#endif


#ifndef JUCE_STRING_UTF_TYPE
 #define JUCE_STRING_UTF_TYPE 8
#endif

//==============================================================================
//==============================================================================

#if JUCE_CORE_INCLUDE_NATIVE_HEADERS
 #include "excludeFromBuild/native/juce_BasicNativeHeaders.h"
#endif

#if JUCE_WINDOWS
 #undef small
#endif

#include "excludeFromBuild/system/juce_StandardHeader.h"

namespace juce
{
    class StringRef;
    class MemoryBlock;
    class File;
    class InputStream;
    class OutputStream;
    class DynamicObject;
    class FileInputStream;
    class FileOutputStream;
    class XmlElement;

    extern JUCE_API bool JUCE_CALLTYPE juce_isRunningUnderDebugger() noexcept;
    extern JUCE_API void JUCE_CALLTYPE logAssertion (const char* file, int line) noexcept;
}

#include "excludeFromBuild/memory/juce_Memory.h"
#include "excludeFromBuild/maths/juce_MathsFunctions.h"
#include "excludeFromBuild/memory/juce_ByteOrder.h"
#include "excludeFromBuild/memory/juce_Atomic.h"
#include "excludeFromBuild/text/juce_CharacterFunctions.h"

#if JUCE_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4514 4996)
#endif

#include "excludeFromBuild/text/juce_CharPointer_UTF8.h"
#include "excludeFromBuild/text/juce_CharPointer_UTF16.h"
#include "excludeFromBuild/text/juce_CharPointer_UTF32.h"
#include "excludeFromBuild/text/juce_CharPointer_ASCII.h"

#if JUCE_MSVC
 #pragma warning (pop)
#endif

#include "excludeFromBuild/text/juce_String.h"
#include "excludeFromBuild/text/juce_StringRef.h"
#include "excludeFromBuild/logging/juce_Logger.h"
#include "excludeFromBuild/memory/juce_LeakedObjectDetector.h"
#include "excludeFromBuild/memory/juce_ContainerDeletePolicy.h"
#include "excludeFromBuild/memory/juce_HeapBlock.h"
#include "excludeFromBuild/memory/juce_MemoryBlock.h"
#include "excludeFromBuild/memory/juce_ReferenceCountedObject.h"
#include "excludeFromBuild/memory/juce_ScopedPointer.h"
#include "excludeFromBuild/memory/juce_OptionalScopedPointer.h"
#include "excludeFromBuild/memory/juce_Singleton.h"
#include "excludeFromBuild/memory/juce_WeakReference.h"
#include "excludeFromBuild/threads/juce_ScopedLock.h"
#include "excludeFromBuild/threads/juce_CriticalSection.h"
#include "excludeFromBuild/maths/juce_Range.h"
#include "excludeFromBuild/maths/juce_NormalisableRange.h"
#include "excludeFromBuild/maths/juce_StatisticsAccumulator.h"
#include "excludeFromBuild/containers/juce_ElementComparator.h"
#include "excludeFromBuild/containers/juce_ArrayAllocationBase.h"
#include "excludeFromBuild/containers/juce_ArrayBase.h"
#include "excludeFromBuild/containers/juce_Array.h"
#include "excludeFromBuild/containers/juce_LinkedListPointer.h"
#include "excludeFromBuild/containers/juce_ListenerList.h"
#include "excludeFromBuild/containers/juce_OwnedArray.h"
#include "excludeFromBuild/containers/juce_ReferenceCountedArray.h"
#include "excludeFromBuild/containers/juce_ScopedValueSetter.h"
#include "excludeFromBuild/containers/juce_SortedSet.h"
#include "excludeFromBuild/containers/juce_SparseSet.h"
#include "excludeFromBuild/containers/juce_AbstractFifo.h"
#include "excludeFromBuild/text/juce_NewLine.h"
#include "excludeFromBuild/text/juce_StringPool.h"
#include "excludeFromBuild/text/juce_Identifier.h"
#include "excludeFromBuild/text/juce_StringArray.h"
#include "excludeFromBuild/text/juce_StringPairArray.h"
#include "excludeFromBuild/text/juce_TextDiff.h"
#include "excludeFromBuild/text/juce_LocalisedStrings.h"
#include "excludeFromBuild/text/juce_Base64.h"
#include "excludeFromBuild/misc/juce_Result.h"
#include "excludeFromBuild/misc/juce_Uuid.h"
#include "excludeFromBuild/misc/juce_ConsoleApplication.h"
#include "excludeFromBuild/containers/juce_Variant.h"
#include "excludeFromBuild/containers/juce_NamedValueSet.h"
#include "excludeFromBuild/containers/juce_DynamicObject.h"
#include "excludeFromBuild/containers/juce_HashMap.h"
#include "excludeFromBuild/system/juce_SystemStats.h"
#include "excludeFromBuild/memory/juce_HeavyweightLeakedObjectDetector.h"
#include "excludeFromBuild/time/juce_RelativeTime.h"
#include "excludeFromBuild/time/juce_Time.h"
#include "excludeFromBuild/streams/juce_InputStream.h"
#include "excludeFromBuild/streams/juce_OutputStream.h"
#include "excludeFromBuild/streams/juce_BufferedInputStream.h"
#include "excludeFromBuild/streams/juce_MemoryInputStream.h"
#include "excludeFromBuild/streams/juce_MemoryOutputStream.h"
#include "excludeFromBuild/streams/juce_SubregionStream.h"
#include "excludeFromBuild/streams/juce_InputSource.h"
#include "excludeFromBuild/files/juce_File.h"
#include "excludeFromBuild/files/juce_DirectoryIterator.h"
#include "excludeFromBuild/files/juce_FileInputStream.h"
#include "excludeFromBuild/files/juce_FileOutputStream.h"
#include "excludeFromBuild/files/juce_FileSearchPath.h"
#include "excludeFromBuild/files/juce_MemoryMappedFile.h"
#include "excludeFromBuild/files/juce_TemporaryFile.h"
#include "excludeFromBuild/files/juce_FileFilter.h"
#include "excludeFromBuild/files/juce_WildcardFileFilter.h"
#include "excludeFromBuild/streams/juce_FileInputSource.h"
#include "excludeFromBuild/logging/juce_FileLogger.h"
#include "excludeFromBuild/javascript/juce_JSON.h"
#include "excludeFromBuild/javascript/juce_Javascript.h"
#include "excludeFromBuild/maths/juce_BigInteger.h"
#include "excludeFromBuild/maths/juce_Expression.h"
#include "excludeFromBuild/maths/juce_Random.h"
#include "excludeFromBuild/misc/juce_RuntimePermissions.h"
#include "excludeFromBuild/misc/juce_WindowsRegistry.h"
#include "excludeFromBuild/threads/juce_ChildProcess.h"
#include "excludeFromBuild/threads/juce_DynamicLibrary.h"
#include "excludeFromBuild/threads/juce_HighResolutionTimer.h"
#include "excludeFromBuild/threads/juce_InterProcessLock.h"
#include "excludeFromBuild/threads/juce_Process.h"
#include "excludeFromBuild/threads/juce_SpinLock.h"
#include "excludeFromBuild/threads/juce_WaitableEvent.h"
#include "excludeFromBuild/threads/juce_Thread.h"
#include "excludeFromBuild/threads/juce_ThreadLocalValue.h"
#include "excludeFromBuild/threads/juce_ThreadPool.h"
#include "excludeFromBuild/threads/juce_TimeSliceThread.h"
#include "excludeFromBuild/threads/juce_ReadWriteLock.h"
#include "excludeFromBuild/threads/juce_ScopedReadLock.h"
#include "excludeFromBuild/threads/juce_ScopedWriteLock.h"
#include "excludeFromBuild/network/juce_IPAddress.h"
#include "excludeFromBuild/network/juce_MACAddress.h"
#include "excludeFromBuild/network/juce_NamedPipe.h"
#include "excludeFromBuild/network/juce_Socket.h"
#include "excludeFromBuild/network/juce_URL.h"
#include "excludeFromBuild/network/juce_WebInputStream.h"
#include "excludeFromBuild/streams/juce_URLInputSource.h"
#include "excludeFromBuild/time/juce_PerformanceCounter.h"
#include "excludeFromBuild/unit_tests/juce_UnitTest.h"
#include "excludeFromBuild/xml/juce_XmlDocument.h"
#include "excludeFromBuild/xml/juce_XmlElement.h"
#include "excludeFromBuild/zip/juce_GZIPCompressorOutputStream.h"
#include "excludeFromBuild/zip/juce_GZIPDecompressorInputStream.h"
#include "excludeFromBuild/zip/juce_ZipFile.h"
#include "excludeFromBuild/containers/juce_PropertySet.h"
#include "excludeFromBuild/memory/juce_SharedResourcePointer.h"

#if JUCE_CORE_INCLUDE_OBJC_HELPERS && (JUCE_MAC || JUCE_IOS)
 #include "excludeFromBuild/native/juce_osx_ObjCHelpers.h"
#endif

#if JUCE_CORE_INCLUDE_COM_SMART_PTR && JUCE_WINDOWS
 #include "excludeFromBuild/native/juce_win32_ComSmartPtr.h"
#endif

#if JUCE_CORE_INCLUDE_JNI_HELPERS && JUCE_ANDROID
 #include <jni.h>
 #include "excludeFromBuild/native/juce_android_JNIHelpers.h"
#endif

#ifndef DOXYGEN
namespace juce
{
 /*
    As the very long class names here try to explain, the purpose of this code is to cause
    a linker error if not all of your compile units are consistent in the options that they
    enable before including JUCE headers. The reason this is important is that if you have
    two cpp files, and one includes the juce headers with debug enabled, and another does so
    without that, then each will be generating code with different class layouts, and you'll
    get subtle and hard-to-track-down memory corruption!
 */
 #if JUCE_DEBUG
  struct JUCE_API this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode
  { this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode() noexcept; };
  static this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode compileUnitMismatchSentinel;
 #else
  struct JUCE_API this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode
  { this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode() noexcept; };
  static this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode compileUnitMismatchSentinel;
 #endif
}
#endif

#if JUCE_MSVC
 #pragma warning (pop)

 // In DLL builds, need to disable this warnings for other modules
 #if defined (JUCE_DLL_BUILD) || defined (JUCE_DLL)
  #pragma warning (disable: 4251)
 #endif
#endif
