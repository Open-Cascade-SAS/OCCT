// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _Standard_TypeDef_HeaderFile
#define _Standard_TypeDef_HeaderFile

#include <cstddef>
#include <ctime>

#if(defined(_MSC_VER) && (_MSC_VER < 1600))
  // old MSVC - hasn't stdint header
  typedef unsigned __int8   uint8_t;
  typedef unsigned __int16  uint16_t;
  typedef unsigned __int32  uint32_t;
  typedef unsigned __int64  uint64_t;
#else
  #include <stdint.h>
#endif

#if(defined(_MSC_VER) && (_MSC_VER < 1800))
  // only Visual Studio 2013 (vc12) provides <cinttypes> header
  // we do not defined all macros here - only used by OCCT framework
  #ifdef _WIN64
    #define PRIdPTR "I64d"
    #define PRIuPTR "I64u"
    #define SCNdPTR "I64d"
    #define SCNuPTR "I64u"
  #else
    #define PRIdPTR "d"
    #define PRIuPTR "u"
    #define SCNdPTR "d"
    #define SCNuPTR "u"
  #endif
#else
  // should be just <cinttypes> since C++11
  // however we use this code for compatibility with old C99 compilers
  #ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
  #endif
  #include <inttypes.h>
#endif

#define Standard_False (Standard_Boolean)0
#define Standard_True  (Standard_Boolean)1

#include <Standard_Macro.hxx>

typedef int           Standard_Integer;
typedef double        Standard_Real;
typedef unsigned int  Standard_Boolean;
typedef float         Standard_ShortReal;
typedef char          Standard_Character;
typedef short         Standard_ExtCharacter;
typedef unsigned char Standard_Byte;
typedef void*         Standard_Address;
typedef size_t        Standard_Size;
typedef std::time_t   Standard_Time;

//
typedef const char*   Standard_CString;
typedef const short*  Standard_ExtString;

// Unicode primitives, char16_t, char32_t
typedef char          Standard_Utf8Char;     //!< signed   UTF-8 char
typedef unsigned char Standard_Utf8UChar;    //!< unsigned UTF-8 char
typedef uint16_t      Standard_Utf16Char;    //!< UTF-16 char (always unsigned)
typedef uint32_t      Standard_Utf32Char;    //!< UTF-32 char (always unsigned)
typedef wchar_t       Standard_WideChar;     //!< wide char (unsigned UTF-16 on Windows platform and signed UTF-32 on Linux)

#endif // _Standard_TypeDef_HeaderFile
