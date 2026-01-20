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

#include <stdint.h>

#if (defined(_MSC_VER) && (_MSC_VER < 1800))
  // only Visual Studio 2013 (vc12) provides <cinttypes> header
  // we do not defined all macros here - only used by OCCT framework
  #define PRIx64 "I64x"
  #define PRIX64 "I64X"
  #define PRId64 "I64d"
  #define PRIu64 "I64u"
  #define SCNd64 "I64d"
  #define SCNu64 "I64u"
  #ifdef _WIN64
    #define PRIxPTR "I64x"
    #define PRIXPTR "I64X"
    #define PRIdPTR "I64d"
    #define PRIuPTR "I64u"
    #define SCNdPTR "I64d"
    #define SCNuPTR "I64u"
  #else
    #define PRIxPTR "Ix"
    #define PRIXPTR "IX"
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

#define Standard_False false
#define Standard_True true

#include <Standard_Macro.hxx>

typedef int           Standard_Integer;
typedef unsigned int  Standard_UInteger;
typedef double        Standard_Real;
typedef bool          Standard_Boolean;
typedef float         Standard_ShortReal;
typedef char          Standard_Character;
typedef unsigned char Standard_Byte;
typedef void*         Standard_Address;
typedef size_t        Standard_Size;
typedef std::time_t   Standard_Time;

// Unicode primitives, char16_t, char32_t
typedef char          Standard_Utf8Char;  //!< signed   UTF-8 char
typedef unsigned char Standard_Utf8UChar; //!< unsigned UTF-8 char
#if ((defined(__GNUC__) && !defined(__clang__)                                                     \
      && ((__GNUC__ == 4 && __GNUC_MINOR__ <= 3) || __GNUC__ < 4))                                 \
     || (defined(_MSC_VER) && (_MSC_VER < 1600)))
// compatibility with old GCC and MSVC compilers
typedef uint16_t Standard_ExtCharacter;
typedef uint16_t Standard_Utf16Char;
typedef uint32_t Standard_Utf32Char;
#else
typedef char16_t Standard_ExtCharacter;
typedef char16_t Standard_Utf16Char; //!< UTF-16 char (always unsigned)
typedef char32_t Standard_Utf32Char; //!< UTF-32 char (always unsigned)
#endif
// clang-format off
typedef wchar_t       Standard_WideChar;     //!< wide char (unsigned UTF-16 on Windows platform and signed UTF-32 on Linux)
// clang-format on

//! @deprecated Use 'int' directly instead.
Standard_DEPRECATED("Standard_Integer is deprecated, use int directly")
typedef int Standard_Integer;

//! @deprecated Use 'unsigned int' directly instead.
Standard_DEPRECATED("Standard_UInteger is deprecated, use unsigned int directly")
typedef unsigned int Standard_UInteger;

//! @deprecated Use 'double' directly instead.
Standard_DEPRECATED("Standard_Real is deprecated, use double directly")
typedef double Standard_Real;

//! @deprecated Use 'bool' directly instead.
Standard_DEPRECATED("Standard_Boolean is deprecated, use bool directly")
typedef bool Standard_Boolean;

//! @deprecated Use 'float' directly instead.
Standard_DEPRECATED("Standard_ShortReal is deprecated, use float directly")
typedef float Standard_ShortReal;

//! @deprecated Use 'char' directly instead.
Standard_DEPRECATED("Standard_Character is deprecated, use char directly")
typedef char Standard_Character;

//! @deprecated Use 'uint8_t' directly instead.
Standard_DEPRECATED("Standard_Byte is deprecated, use uint8_t directly")
typedef uint8_t Standard_Byte;

//! @deprecated Use 'void*' directly instead.
Standard_DEPRECATED("Standard_Address is deprecated, use void* directly")
typedef void* Standard_Address;

//! @deprecated Use 'size_t' directly instead.
Standard_DEPRECATED("Standard_Size is deprecated, use size_t directly")
typedef size_t Standard_Size;

//! @deprecated Use 'std::time_t' directly instead.
Standard_DEPRECATED("Standard_Time is deprecated, use std::time_t directly")
typedef std::time_t Standard_Time;

//! @deprecated Use 'char' directly instead.
Standard_DEPRECATED("Standard_Utf8Char is deprecated, use char directly")
typedef char Standard_Utf8Char;

//! @deprecated Use 'unsigned char' directly instead.
Standard_DEPRECATED("Standard_Utf8UChar is deprecated, use unsigned char directly")
typedef unsigned char Standard_Utf8UChar;

//! @deprecated Use 'char16_t' directly instead.
Standard_DEPRECATED("Standard_ExtCharacter is deprecated, use char16_t directly")
typedef char16_t Standard_ExtCharacter;

//! @deprecated Use 'char16_t' directly instead.
Standard_DEPRECATED("Standard_Utf16Char is deprecated, use char16_t directly")
typedef char16_t Standard_Utf16Char;

//! @deprecated Use 'char32_t' directly instead.
Standard_DEPRECATED("Standard_Utf32Char is deprecated, use char32_t directly")
typedef char32_t Standard_Utf32Char;

//! @deprecated Use 'wchar_t' directly instead.
Standard_DEPRECATED("Standard_WideChar is deprecated, use wchar_t directly")
typedef wchar_t Standard_WideChar;

//! @deprecated Use 'const char*' directly instead.
Standard_DEPRECATED("Standard_CString is deprecated, use const char* directly")
typedef const char* Standard_CString;

//! @deprecated Use 'const char16_t*' directly instead.
Standard_DEPRECATED("Standard_ExtString is deprecated, use const char16_t* directly")
typedef const char16_t* Standard_ExtString;

#endif // _Standard_TypeDef_HeaderFile
