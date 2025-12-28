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

#include <Standard_Macro.hxx>

//! @deprecated Use 'false' directly instead.
Standard_DEPRECATED("Standard_False is deprecated, use false directly")
inline constexpr bool Standard_False = false;

//! @deprecated Use 'true' directly instead.
Standard_DEPRECATED("Standard_True is deprecated, use true directly")
inline constexpr bool Standard_True = true;

//! @deprecated Use 'int' directly instead.
typedef int Standard_Integer [[deprecated("Standard_Integer is deprecated, use int directly")]];

//! @deprecated Use 'unsigned int' directly instead.
typedef unsigned int Standard_UInteger
  [[deprecated("Standard_UInteger is deprecated, use unsigned int directly")]];

//! @deprecated Use 'double' directly instead.
typedef double Standard_Real [[deprecated("Standard_Real is deprecated, use double directly")]];

//! @deprecated Use 'bool' directly instead.
typedef bool Standard_Boolean [[deprecated("Standard_Boolean is deprecated, use bool directly")]];

//! @deprecated Use 'float' directly instead.
typedef float Standard_ShortReal
  [[deprecated("Standard_ShortReal is deprecated, use float directly")]];

//! @deprecated Use 'char' directly instead.
typedef char Standard_Character
  [[deprecated("Standard_Character is deprecated, use char directly")]];

//! @deprecated Use 'uint8_t' directly instead.
typedef uint8_t Standard_Byte [[deprecated("Standard_Byte is deprecated, use uint8_t directly")]];

//! @deprecated Use 'void*' directly instead.
typedef void* Standard_Address [[deprecated("Standard_Address is deprecated, use void* directly")]];

//! @deprecated Use 'size_t' directly instead.
typedef size_t Standard_Size [[deprecated("Standard_Size is deprecated, use size_t directly")]];

//! @deprecated Use 'std::time_t' directly instead.
typedef std::time_t Standard_Time
  [[deprecated("Standard_Time is deprecated, use std::time_t directly")]];

//! @deprecated Use 'char' directly instead.
typedef char Standard_Utf8Char [[deprecated("Standard_Utf8Char is deprecated, use char directly")]];

//! @deprecated Use 'unsigned char' directly instead.
typedef unsigned char Standard_Utf8UChar
  [[deprecated("Standard_Utf8UChar is deprecated, use unsigned char directly")]];

//! @deprecated Use 'char16_t' directly instead.
typedef char16_t Standard_ExtCharacter
  [[deprecated("Standard_ExtCharacter is deprecated, use char16_t directly")]];

//! @deprecated Use 'char16_t' directly instead.
typedef char16_t Standard_Utf16Char
  [[deprecated("Standard_Utf16Char is deprecated, use char16_t directly")]];

//! @deprecated Use 'char32_t' directly instead.
typedef char32_t Standard_Utf32Char
  [[deprecated("Standard_Utf32Char is deprecated, use char32_t directly")]];

//! @deprecated Use 'wchar_t' directly instead.
typedef wchar_t Standard_WideChar
  [[deprecated("Standard_WideChar is deprecated, use wchar_t directly")]];

//! @deprecated Use 'const char*' directly instead.
typedef const char* Standard_CString
  [[deprecated("Standard_CString is deprecated, use const char* directly")]];

//! @deprecated Use 'const char16_t*' directly instead.
typedef const char16_t* Standard_ExtString
  [[deprecated("Standard_ExtString is deprecated, use const char16_t* directly")]];

#endif // _Standard_TypeDef_HeaderFile
