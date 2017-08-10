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

// Purpose:   This file is intended to be the first file #included to any
//            Open CASCADE source. It defines platform-specific pre-processor 
//            macros necessary for correct compilation of Open CASCADE code

#ifndef _Standard_Macro_HeaderFile
# define _Standard_Macro_HeaderFile

#if defined(__cplusplus) && (__cplusplus >= 201100L)
  // part of C++11 standard
  #define Standard_OVERRIDE override
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
  // MSVC extension since VS2012
  #define Standard_OVERRIDE override
#else
  #define Standard_OVERRIDE
#endif

// Macro for marking variables / functions as possibly unused
// so that compiler will not emit redundant "unused" warnings.
#if defined(__GNUC__) || defined(__clang__)
  #define Standard_UNUSED __attribute__((unused))
#else
  #define Standard_UNUSED
#endif

// Macro Standard_DEPRECATED("message") can be used to declare a method deprecated.
// If OCCT_NO_DEPRECATED is defined, Standard_DEPRECATED is defined empty.
#ifdef OCCT_NO_DEPRECATED
  #define Standard_DEPRECATED(theMsg)
#else
#if defined(_MSC_VER)
  #define Standard_DEPRECATED(theMsg) __declspec(deprecated(theMsg))
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined(__clang__))
  #define Standard_DEPRECATED(theMsg) __attribute__((deprecated(theMsg)))
#elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
  #define Standard_DEPRECATED(theMsg) __attribute__((deprecated))
#else
  #define Standard_DEPRECATED(theMsg)
#endif
#endif

// Disable warnings about deprecated features.
// This is useful for sections of code kept for backward compatibility and scheduled for removal.

#if defined(__ICL) || defined (__INTEL_COMPILER)
  #define Standard_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable:1478))
  #define Standard_ENABLE_DEPRECATION_WARNINGS  __pragma(warning(pop))
#elif defined(_MSC_VER)
  #define Standard_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable:4996))
  #define Standard_ENABLE_DEPRECATION_WARNINGS  __pragma(warning(pop))
#elif (defined(__GNUC__) && __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || defined(__clang__)
  // available since at least gcc 4.2 (maybe earlier), however only gcc 4.6+ supports this pragma inside the function body
  // CLang also supports this gcc syntax (in addition to "clang diagnostic ignored")
  #define Standard_DISABLE_DEPRECATION_WARNINGS _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
  #define Standard_ENABLE_DEPRECATION_WARNINGS  _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
#else
  #define Standard_DISABLE_DEPRECATION_WARNINGS
  #define Standard_ENABLE_DEPRECATION_WARNINGS
#endif

# ifdef _WIN32

// We must be careful including windows.h: it is really poisonous stuff!
// The most annoying are #defines of many identifiers that you could use in 
// normal code without knowing that Windows has its own knowledge of them...
// So lets protect ourselves by switching OFF as much as possible of this in advance.
// If someone needs more from windows.h, he is encouraged to #undef these symbols
// or include windows.h prior to any OCCT stuff.
// Note that we define each symbol to itself, so that it still can be used
// e.g. as name of variable, method etc.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   /* exclude extra Windows stuff */
#endif
#ifndef NOMINMAX
#define NOMINMAX NOMINMAX     /* avoid #define min() and max() */
#endif
#ifndef NOMSG
#define NOMSG NOMSG           /* avoid #define SendMessage etc. */
#endif
#ifndef NODRAWTEXT
#define NODRAWTEXT NODRAWTEXT /* avoid #define DrawText etc. */
#endif
#ifndef NONLS
#define NONLS NONLS           /* avoid #define CompareString etc. */
#endif
#ifndef NOGDI
#define NOGDI NOGDI           /* avoid #define SetPrinter (winspool.h) etc. */
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

# if defined(_WIN32) && !defined(HAVE_NO_DLL)

//======================================================
// Windows-specific definitions
//======================================================

#  ifndef Standard_EXPORT
#   define Standard_EXPORT __declspec( dllexport )
// For global variables :
#   define Standard_EXPORTEXTERN __declspec( dllexport ) extern
#   define Standard_EXPORTEXTERNC extern "C" __declspec( dllexport )
#  endif  /* Standard_EXPORT */

#  ifndef Standard_IMPORT
#   define Standard_IMPORT __declspec( dllimport ) extern
#   define Standard_IMPORTC extern "C" __declspec( dllimport )
#  endif  /* Standard_IMPORT */

# else  /* UNIX */

//======================================================
// UNIX / static library definitions
//======================================================

#  ifndef Standard_EXPORT
#   define Standard_EXPORT
// For global variables :
#   define Standard_EXPORTEXTERN extern
#   define Standard_EXPORTEXTERNC extern "C"
#  endif  /* Standard_EXPORT */

#  ifndef Standard_IMPORT
#   define Standard_IMPORT extern
#   define Standard_IMPORTC extern "C"
#  endif  /* Standard_IMPORT */

// Compatibility with old SUN compilers

// This preprocessor directive is a kludge to get around
// a bug in the Sun Workshop 5.0 compiler, it keeps the
// /usr/include/memory.h file from being #included
// with an incompatible extern "C" definition of memchr
// October 18, 2000  <rboehne@ricardo-us.com>
#if __SUNPRO_CC_COMPAT == 5
#define	_MEMORY_H
#endif

# endif  /* _WIN32 */

//======================================================
// Other
//======================================================

# ifndef __Standard_API
#   if !defined(_WIN32) || defined(__Standard_DLL) || defined(__FSD_DLL) || defined(__MMgt_DLL) || defined(__OSD_DLL) || defined(__Plugin_DLL) || defined(__Quantity_DLL) || defined(__Resource_DLL) || defined(__SortTools_DLL) || defined(__StdFail_DLL) || defined(__Storage_DLL) || defined(__TColStd_DLL) || defined(__TCollection_DLL) || defined(__TShort_DLL) || defined(__Units_DLL) || defined(__UnitsAPI_DLL) || defined(__Dico_DLL)
#    define __Standard_API Standard_EXPORT
#    define __Standard_APIEXTERN Standard_EXPORTEXTERN
#   else
#    define __Standard_API Standard_IMPORT
#    define __Standard_APIEXTERN Standard_IMPORT
#   endif  // __Standard_DLL
# endif  // __Standard_API

// Support of Universal Windows Platform
#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_APP
 #define OCCT_UWP
#else
 #ifdef OCCT_UWP
   #undef OCCT_UWP
 #endif
#endif

#endif
