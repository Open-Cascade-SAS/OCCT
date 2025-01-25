// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//! @file
//! This file is intended to be the first file included to any
//! Open CASCADE source. It defines platform-specific pre-processor
//! macros necessary for correct compilation of Open CASCADE code.

#ifndef _Standard_Macro_HeaderFile
#define _Standard_Macro_HeaderFile

#if defined(_MSC_VER) && (_MSC_VER < 1600)
  #error C++11 compatible compiler is required (Visual Studio 2010 or newer)
#endif

//! @def Standard_OVERRIDE
//! Should be used in declarations of virtual methods overridden in the
//! derived classes, to cause compilation error in the case if that virtual
//! function disappears or changes its signature in the base class.
//!
//! Expands to C++11 keyword "override" on compilers that are known to
//! support it; empty in other cases.
#if defined(__cplusplus) && (__cplusplus >= 201100L)
  // part of C++11 standard
  #define Standard_OVERRIDE override
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
  // MSVC extension since VS2012
  #define Standard_OVERRIDE override
#else
  #define Standard_OVERRIDE
#endif

//! @def Standard_DELETE
//! Alias for C++11 keyword "=delete" marking methods to be deleted.
#if defined(__cplusplus) && (__cplusplus >= 201100L)
  // part of C++11 standard
  #define Standard_DELETE = delete
#elif defined(_MSC_VER) && (_MSC_VER >= 1800)
  // implemented since VS2013
  #define Standard_DELETE = delete
#else
  #define Standard_DELETE
#endif

//! @def Standard_FALLTHROUGH
//! Should be used in a switch statement immediately before a case label,
//! if code associated with the previous case label may fall through to that
//! next label (i.e. does not end with "break" or "return" etc.).
//! This macro indicates that the fall through is intentional and should not be
//! diagnosed by a compiler that warns on fallthrough.
//!
//! Expands to C++17 attribute statement "[[fallthrough]];" on compilers that
//! declare support of C++17, or to "__attribute__((fallthrough));" on
//! GCC 7+.
#if defined(__cplusplus) && (__cplusplus >= 201703L)
  // part of C++17 standard
  #define Standard_FALLTHROUGH [[fallthrough]];
#elif defined(__GNUC__) && (__GNUC__ >= 7)
  // gcc 7+
  #define Standard_FALLTHROUGH __attribute__((fallthrough));
#else
  #define Standard_FALLTHROUGH
#endif

//! @def Standard_NODISCARD
//! This attribute may appear in a function declaration,
//! enumeration declaration or class declaration. It tells the compiler to
//! issue a warning, if a return value marked by that attribute is discarded.
//!
//! Expands to C++17 attribute statement "[[nodiscard]]" on compilers that
//! declare support of this attribute, or equivalent attribute on GCC.
#if defined(__has_cpp_attribute)
  #if __has_cpp_attribute(nodiscard)
    #define Standard_NODISCARD [[nodiscard]]
  #else
    #define Standard_NODISCARD
  #endif
#elif defined(__GNUC__) && !defined(INTEL_COMPILER)
  // According to available documentation, GCC-style __attribute__ ((warn_unused_result))
  // should be available in GCC since version 3.4, and in CLang since 3.9;
  // Intel compiler does not seem to support this
  #define Standard_NODISCARD __attribute__((warn_unused_result))
#else
  #define Standard_NODISCARD
#endif

//! @def Standard_UNUSED
//! Macro for marking variables / functions as possibly unused
//! so that compiler will not emit redundant "unused" warnings.
//!
//! Expands to "__attribute__((unused))" on GCC and CLang.
#if defined(__GNUC__) || defined(__clang__)
  #define Standard_UNUSED __attribute__((unused))
#else
  #define Standard_UNUSED
#endif

//! @def Standard_NOINLINE
//! Macro for disallowing function inlining.
//! Expands to "__attribute__((noinline))" on GCC and CLang.
#if defined(__clang__)                                                                             \
  || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
  #define Standard_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
  #define Standard_NOINLINE __declspec(noinline)
#else
  #define Standard_NOINLINE
#endif

//! @def Standard_THREADLOCAL
//! Define Standard_THREADLOCAL modifier as C++11 thread_local keyword where it is available.
#if defined(__clang__)
  // CLang version: standard CLang > 3.3 or XCode >= 8 (but excluding 32-bit ARM)
  // Note: this has to be in separate #if to avoid failure of preprocessor on other platforms
  #if __has_feature(cxx_thread_local)
    #define Standard_THREADLOCAL thread_local
  #endif
#elif defined(__INTEL_COMPILER)
  #if (defined(_MSC_VER) && _MSC_VER >= 1900 && __INTEL_COMPILER > 1400)
    // requires msvcrt vc14+ (Visual Studio 2015+)
    #define Standard_THREADLOCAL thread_local
  #elif (!defined(_MSC_VER) && __INTEL_COMPILER > 1500)
    #define Standard_THREADLOCAL thread_local
  #endif
#elif (defined(_MSC_VER) && _MSC_VER >= 1900)
  // msvcrt coming with vc14+ (VS2015+)
  #define Standard_THREADLOCAL thread_local
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
  // GCC >= 4.8
  #define Standard_THREADLOCAL thread_local
#endif

#ifndef Standard_THREADLOCAL
  #define Standard_THREADLOCAL
#endif

//! @def Standard_DEPRECATED("message")
//! Can be used in declaration of a method or a class to mark it as deprecated.
//! Use of such method or class will cause compiler warning (if supported by
//! compiler and unless disabled).
//! If macro OCCT_NO_DEPRECATED is defined, Standard_DEPRECATED is defined empty.
#ifdef OCCT_NO_DEPRECATED
  #define Standard_DEPRECATED(theMsg)
#else
  #if defined(_MSC_VER)
    #define Standard_DEPRECATED(theMsg) __declspec(deprecated(theMsg))
  #elif defined(__GNUC__)                                                                          \
    && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined(__clang__))
    #define Standard_DEPRECATED(theMsg) __attribute__((deprecated(theMsg)))
  #elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
    #define Standard_DEPRECATED(theMsg) __attribute__((deprecated))
  #else
    #define Standard_DEPRECATED(theMsg)
  #endif
#endif

//! @def Standard_DISABLE_DEPRECATION_WARNINGS
//! Disables warnings on use of deprecated features (see Standard_DEPRECATED),
//! from the current point till appearance of Standard_ENABLE_DEPRECATION_WARNINGS macro.
//! This is useful for sections of code kept for backward compatibility and scheduled for removal.
//!
//! @def Standard_ENABLE_DEPRECATION_WARNINGS
//! Enables warnings on use of deprecated features previously disabled by
//! Standard_DISABLE_DEPRECATION_WARNINGS.
#if defined(__ICL) || defined(__INTEL_COMPILER)
  #define Standard_DISABLE_DEPRECATION_WARNINGS                                                    \
    __pragma(warning(push)) __pragma(warning(disable : 1478))
  #define Standard_ENABLE_DEPRECATION_WARNINGS __pragma(warning(pop))
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))              \
  || defined(__clang__)
  // available since at least gcc 4.2 (maybe earlier), however only gcc 4.6+ supports this pragma
  // inside the function body CLang also supports this gcc syntax (in addition to "clang diagnostic
  // ignored")
  #define Standard_DISABLE_DEPRECATION_WARNINGS                                                    \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
  #define Standard_ENABLE_DEPRECATION_WARNINGS                                                     \
    _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
#elif defined(_MSC_VER)
  #define Standard_DISABLE_DEPRECATION_WARNINGS                                                    \
    __pragma(warning(push)) __pragma(warning(disable : 4996))
  #define Standard_ENABLE_DEPRECATION_WARNINGS __pragma(warning(pop))
#else
  #define Standard_DISABLE_DEPRECATION_WARNINGS
  #define Standard_ENABLE_DEPRECATION_WARNINGS
#endif

#ifdef _WIN32

  // We must be careful including windows.h: it is really poisonous stuff!
  // The most annoying are #defines of many identifiers that you could use in
  // normal code without knowing that Windows has its own knowledge of them...
  // So lets protect ourselves by switching OFF as much as possible of this in advance.
  // If someone needs more from windows.h, he is encouraged to #undef these symbols
  // or include windows.h prior to any OCCT stuff.
  // Note that we define each symbol to itself, so that it still can be used
  // e.g. as name of variable, method etc.
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN /* exclude extra Windows stuff */
  #endif
  #ifndef NOMINMAX
    #define NOMINMAX NOMINMAX /* avoid #define min() and max() */
  #endif
  #ifndef NOMSG
    #define NOMSG NOMSG /* avoid #define SendMessage etc. */
  #endif
  #ifndef NODRAWTEXT
    #define NODRAWTEXT NODRAWTEXT /* avoid #define DrawText etc. */
  #endif
  #ifndef NONLS
    #define NONLS NONLS /* avoid #define CompareString etc. */
  #endif
  #ifndef NOGDI
    #define NOGDI NOGDI /* avoid #define SetPrinter (winspool.h) etc. */
  #endif
  #ifndef NOSERVICE
    #define NOSERVICE NOSERVICE
  #endif
  #ifndef NOKERNEL
    #define NOKERNEL NOKERNEL
  #endif
  #ifndef NOUSER
    #define NOUSER NOUSER
  #endif
  #ifndef NOMCX
    #define NOMCX NOMCX
  #endif
  #ifndef NOIME
    #define NOIME NOIME
  #endif

#endif

//! @def Standard_EXPORT
//! This macro should be used in declarations of public methods
//! to ensure that they are exported from DLL on Windows and thus
//! can be called from other (dependent) libraries or applications.
//!
//! If macro OCCT_STATIC_BUILD is defined, then Standard_EXPORT
//! is set to empty.

#if defined(_WIN32) && !defined(OCCT_STATIC_BUILD) && !defined(HAVE_NO_DLL)

  //======================================================
  // Windows-specific definitions
  //======================================================

  #ifndef Standard_EXPORT
    #define Standard_EXPORT __declspec(dllexport)
    // For global variables :
    #define Standard_EXPORTEXTERN __declspec(dllexport) extern
    #define Standard_EXPORTEXTERNC extern "C" __declspec(dllexport)
  #endif /* Standard_EXPORT */

  #ifndef Standard_IMPORT
    #define Standard_IMPORT __declspec(dllimport) extern
    #define Standard_IMPORTC extern "C" __declspec(dllimport)
  #endif /* Standard_IMPORT */

#else /* UNIX */

  //======================================================
  // UNIX / static library definitions
  //======================================================

  #ifndef Standard_EXPORT
    #define Standard_EXPORT
    // For global variables :
    #define Standard_EXPORTEXTERN extern
    #define Standard_EXPORTEXTERNC extern "C"
  #endif /* Standard_EXPORT */

  #ifndef Standard_IMPORT
    #define Standard_IMPORT extern
    #define Standard_IMPORTC extern "C"
  #endif /* Standard_IMPORT */

  // Compatibility with old SUN compilers

  // This preprocessor directive is a kludge to get around
  // a bug in the Sun Workshop 5.0 compiler, it keeps the
  // /usr/include/memory.h file from being #included
  // with an incompatible extern "C" definition of memchr
  // October 18, 2000  <rboehne@ricardo-us.com>
  #if __SUNPRO_CC_COMPAT == 5
    #define _MEMORY_H
  #endif

#endif /* _WIN32 */

//! @def Standard_HIDDEN
//! This macro should be used in declarations of methods or functions
//! to ensure that they are hidden and not exported from the shared library.
//!
//! Expands to "__attribute__((visibility("hidden")))" on GCC and Clang
#if defined(__has_attribute)
  #if __has_attribute(visibility)
    #define Standard_HIDDEN __attribute__((visibility("hidden")))
  #else
    #define Standard_HIDDEN
  #endif
#elif defined(__GNUC__) || defined(__clang__)
  #define Standard_HIDDEN __attribute__((visibility("hidden")))
#else
  #define Standard_HIDDEN
#endif

//! @def OCCT_UWP
//! This macro is defined on Windows platform in the case if the code
//! is being compiled for UWP (Universal Windows Platform).
#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_APP
  #define OCCT_UWP
#else
  #ifdef OCCT_UWP
    #undef OCCT_UWP
  #endif
#endif

//! @def Standard_ATOMIC
//! Definition of Standard_ATOMIC for C++11 or visual studio that supports it.
//! Before usage there must be "atomic" included in the following way:
//! #ifdef Standard_HASATOMIC
//!   #include <atomic>
//! #endif
#if (defined(__cplusplus) && __cplusplus >= 201100L) || (defined(_MSC_VER) && _MSC_VER >= 1800)    \
  || (defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)))
  #define Standard_HASATOMIC
  #define Standard_ATOMIC(theType) std::atomic<theType>
#else
  #define Standard_ATOMIC(theType) theType
#endif

//! @def Standard_Noexcept
//! Definition of Standard_Noexcept:
//! if noexcept is accessible, Standard_Noexcept is "noexcept" and "throw()" otherwise.
#ifdef _MSC_VER
  #if _MSC_VER >= 1900
    #define Standard_Noexcept noexcept
  #else
    #define Standard_Noexcept throw()
  #endif
#else
  #if __cplusplus >= 201103L
    #define Standard_Noexcept noexcept
  #else
    #define Standard_Noexcept throw()
  #endif
#endif

#endif
