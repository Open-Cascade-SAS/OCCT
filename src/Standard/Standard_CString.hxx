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

# ifndef _Standard_TypeDef_HeaderFile
#  include <Standard_TypeDef.hxx>
# endif

# if defined(_MSC_VER) && ! defined(strcasecmp)
#  define strcasecmp _stricmp
# endif

# include <string.h>
# include <stdio.h>

# ifndef _Standard_Integer_HeaderFile
#  include <Standard_Integer.hxx>
# endif

__Standard_API Standard_Integer HashCode (const Standard_CString,
                           const Standard_Integer);
inline Standard_Integer HashCode (const Standard_CString,
                                  const Standard_Integer,
                                  const Standard_Integer);
inline Standard_Integer HashCode (const Standard_CString,
                                  const Standard_Integer ,
                                  const Standard_Integer ,
                                  Standard_Integer& );
__Standard_API Standard_Integer HashCodes (const Standard_CString,
                                           const Standard_Integer);

//! Equivalents of functions from standard C library that use always C locale
__Standard_API double Atof    (const char* theStr);
__Standard_API double Strtod  (const char* theStr, char** theNextPtr);
__Standard_API int    Printf  (const char* theFormat, ...);
__Standard_API int    Fprintf (FILE* theFile, const char* theFormat, ...);
__Standard_API int    Sprintf (char* theBuffer, const char* theFormat, ...);

//============================================================================
//==== IsEqual : Returns Standard_True if two booleans have the same value
//============================================================================
inline Standard_Boolean IsEqual(const Standard_CString One
				 ,const Standard_CString Two)
{
  return (strcmp(One,Two) == 0);
}

//============================================================================
//==== HashCode of CString. Returns the HashCode itself and
//====                              the HashCode % Upper
//============================================================================
inline Standard_Integer HashCode (const Standard_CString Value,
                                  const Standard_Integer Len ,
			          const Standard_Integer Upper ,
                                  Standard_Integer& aHashCode )
{
  aHashCode = HashCodes( Value , Len );
//  return (Abs( aHashCode ) % Upper ) + 1 ;
  return HashCode( (Standard_Integer)aHashCode , Upper ) ;
}

//============================================================================
//==== HashCode of CString. Returns the HashCode itself and
//====                              the HashCode % Upper
//============================================================================
inline Standard_Integer HashCode (const Standard_CString Value,
                                  const Standard_Integer Len ,
			          const Standard_Integer Upper )
{
//  return (Abs( HashCodes( Value , Len ) ) % Upper ) + 1 ;
  return HashCode( (Standard_Integer) HashCodes( Value , Len ) , Upper ) ;
}

#endif
