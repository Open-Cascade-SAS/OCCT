// File:      Standard_Macro.hxx
// Copyright: Open Cascade 2007
// Purpose:   This file is intended to be the first file #included to any
//            Open CASCADE source. It defines platform-specific pre-processor 
//            macros necessary for correct compilation of Open CASCADE code

#ifndef _Standard_Macro_HeaderFile
# define _Standard_Macro_HeaderFile

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

// Standard OCC macros: Handle(), STANDARD_TYPE()
# define   Handle(ClassName)  Handle_##ClassName
# define   STANDARD_TYPE(aType)   aType##_Type_()

//======================================================
// Windows-specific definitions
//======================================================

// check if WNT macro is not defined but compiler is MSVC
#if defined(_MSC_VER) && !defined(WNT)
#error "Wrong compiler options has been detected. Add /DWNT option for proper compilation!!!!!"
#endif

# if defined(WNT) && !defined(HAVE_NO_DLL)

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

# else  /* WNT */

//======================================================
// UNIX definitions
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

# endif  /* WNT */

//======================================================
// Other
//======================================================

# ifndef __Standard_API
//#  ifdef WNT
#   if !defined(WNT) || defined(__Standard_DLL) || defined(__FSD_DLL) || defined(__MMgt_DLL) || defined(__OSD_DLL) || defined(__Plugin_DLL) || defined(__Quantity_DLL) || defined(__Resource_DLL) || defined(__SortTools_DLL) || defined(__StdFail_DLL) || defined(__Storage_DLL) || defined(__TColStd_DLL) || defined(__TCollection_DLL) || defined(__TShort_DLL) || defined(__Units_DLL) || defined(__UnitsAPI_DLL) || defined(__Dico_DLL)
#    define __Standard_API Standard_EXPORT
#    define __Standard_APIEXTERN Standard_EXPORTEXTERN
#   else
#    define __Standard_API Standard_IMPORT
#    define __Standard_APIEXTERN Standard_IMPORT
#   endif  // __Standard_DLL
//#  else
//#   define __Standard_API
//#  endif  // WNT
# endif  // __Standard_API

// Define _OCC64 variable (unless already defined) if platform is known to be 64-bit
#ifndef _OCC64
#if defined (__alpha) || defined(DECOSF1) || defined(_WIN64) || defined(__amd64) || defined(__x86_64)
#define _OCC64 1
#endif
#endif

#endif  
