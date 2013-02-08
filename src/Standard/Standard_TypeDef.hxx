// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Standard_TypeDef_HeaderFile
#define _Standard_TypeDef_HeaderFile

#include <cstddef>
#include <ctime>

#if(defined(_MSC_VER) && (_MSC_VER < 1600))
  // old MSVC - hasn't stdint header
  typedef unsigned __int16  uint16_t;
  typedef unsigned __int32  uint32_t;
#else
  #include <stdint.h>
#endif

#define Standard_False (Standard_Boolean)0
#define Standard_True  (Standard_Boolean)1

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

typedef int Standard_Integer;
typedef double Standard_Real;
typedef unsigned int Standard_Boolean;
typedef float Standard_ShortReal;
typedef char  Standard_Character;
typedef short Standard_ExtCharacter;
typedef unsigned char  Standard_Byte;
typedef void* Standard_Address;
typedef size_t Standard_Size;

//
typedef const char*  Standard_CString;
typedef const short* Standard_ExtString;

// Unicode primitives, char16_t, char32_t
typedef char          Standard_Utf8Char;     //!< signed   UTF-8 char
typedef unsigned char Standard_Utf8UChar;    //!< unsigned UTF-8 char
typedef uint16_t      Standard_Utf16Char;    //!< UTF-16 char (always unsigned)
typedef uint32_t      Standard_Utf32Char;    //!< UTF-32 char (always unsigned)
typedef wchar_t       Standard_WideChar;     //!< wide char (unsigned UTF-16 on Windows platform and signed UTF-32 on Linux)

typedef std::time_t Standard_Time;

#endif
