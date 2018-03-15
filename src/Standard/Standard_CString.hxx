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

//!@file Functions working with plain C strings

#ifndef _Standard_CString_HeaderFile
# define _Standard_CString_HeaderFile

#include <Standard_Macro.hxx>

# include <string.h>
# include <stdio.h>

# if defined(_MSC_VER) && ! defined(strcasecmp)
#  define strcasecmp _stricmp
# endif

// C++ only definitions
#ifdef __cplusplus

#include <Standard_Integer.hxx>

//! Returns bounded hash code for a null-terminated string, in range [1, theUpper]
Standard_EXPORT Standard_Integer HashCode (const Standard_CString theStr, const Standard_Integer theUpper);

//! Returns 32-bit hash code for the first theLen characters in the string theStr 
Standard_EXPORT Standard_Integer HashCodes (const Standard_CString theStr, const Standard_Integer theLen);

//! Returns bounded hash code for the first theLen characters in 
//! the string theStr, in range [1, theUpper]
inline Standard_Integer HashCode (const Standard_CString theStr,
                                  const Standard_Integer theLen,
                                  const Standard_Integer theUpper)
{
//  return (Abs( HashCodes( Value , Len ) ) % Upper ) + 1 ;
  return HashCode (HashCodes (theStr, theLen), theUpper);
}

//! Returns Standard_True if two strings are equal
inline Standard_Boolean IsEqual (const Standard_CString theOne, const Standard_CString theTwo)
{
  return strcmp (theOne, theTwo) == 0;
}

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//! Equivalent of standard C function atof() that always uses C locale
Standard_EXPORT double Atof (const char* theStr);

//! Optimized equivalent of standard C function strtod() that always uses C locale
Standard_EXPORT double Strtod (const char* theStr, char** theNextPtr);

//! Equivalent of standard C function printf() that always uses C locale
Standard_EXPORT int Printf (const char* theFormat, ...);

//! Equivalent of standard C function fprintf() that always uses C locale
Standard_EXPORT int Fprintf (FILE* theFile, const char* theFormat, ...);

//! Equivalent of standard C function sprintf() that always uses C locale
Standard_EXPORT int Sprintf (char* theBuffer, const char* theFormat, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
