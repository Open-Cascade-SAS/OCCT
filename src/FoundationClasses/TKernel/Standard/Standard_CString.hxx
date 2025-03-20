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

//!@file
//! Functions working with plain C strings

#ifndef _Standard_CString_HeaderFile
#define _Standard_CString_HeaderFile

#include <Standard_Macro.hxx>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
  #if !defined(strcasecmp)
    #define strcasecmp _stricmp
  #endif
  #if !defined(strncasecmp)
    #define strncasecmp _strnicmp
  #endif
#endif

// C++ only definitions
#ifdef __cplusplus

  #include <Standard_TypeDef.hxx>

//! Returns Standard_True if two strings are equal
inline Standard_Boolean IsEqual(const Standard_CString theOne, const Standard_CString theTwo)
{
  return strcmp(theOne, theTwo) == 0;
}

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  //! Equivalent of standard C function atof() that always uses C locale
  Standard_EXPORT double Atof(const char* theStr);

  //! Optimized equivalent of standard C function strtod() that always uses C locale
  Standard_EXPORT double Strtod(const char* theStr, char** theNextPtr);

  //! Equivalent of standard C function printf() that always uses C locale
  Standard_EXPORT int Printf(const char* theFormat, ...);

  //! Equivalent of standard C function fprintf() that always uses C locale
  Standard_EXPORT int Fprintf(FILE* theFile, const char* theFormat, ...);

  //! Equivalent of standard C function sprintf() that always uses C locale
  Standard_EXPORT int Sprintf(char* theBuffer, const char* theFormat, ...);

  //! Equivalent of standard C function vsprintf() that always uses C locale.
  //! Note that this function does not check buffer bounds and should be used with precaution
  //! measures (only with format fitting into the buffer of known size).
  //! @param[in][out] theBuffer    string buffer to fill
  //! @param[in] theFormat   format to apply
  //! @param[in] theArgList  argument list for specified format
  //! @return the total number of characters written, or a negative number on error
  Standard_EXPORT int Vsprintf(char* theBuffer, const char* theFormat, va_list theArgList);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
