// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//============================================================================
//==== Titre: Standard_CString.hxx
//==== Role : The headr file of primitve type "CString" from package "Standard"
//====
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef char* Standard_CString;
//============================================================================

#ifndef _Standard_CString_HeaderFile
# define _Standard_CString_HeaderFile

# ifndef _Standard_TypeDef_HeaderFile
#  include <Standard_TypeDef.hxx>
# endif

# ifdef _MSC_VER
#  define strcasecmp _stricmp
# endif

# include <string.h>
# include <stdio.h>

# ifndef _Standard_Integer_HeaderFile
#  include <Standard_Integer.hxx>
# endif


class Handle_Standard_Type;


__Standard_API const Handle_Standard_Type& Standard_CString_Type_();

inline  Standard_Integer Abs (const Standard_Integer);
inline Standard_CString ShallowCopy (const Standard_CString Value);
inline Standard_Boolean IsSimilar(const Standard_CString One
				 ,const Standard_CString Two);
__Standard_API Standard_Integer HashCode (const Standard_CString,
                           const Standard_Integer);
inline Standard_Integer HashCode (const Standard_CString,
                                  const Standard_Integer,
                                  const Standard_Integer);
inline Standard_Integer HashCode (const Standard_CString,
                                  const Standard_Integer ,
                                  const Standard_Integer ,
                                  Standard_Integer& );
Standard_Integer HashCodes (const Standard_CString ,
                            const Standard_Integer  );

//! Equivalents of functions from standard C library that use always C locale
__Standard_API double Atof    (const char* theStr);
__Standard_API double Strtod  (const char* theStr, char** theNextPtr);
__Standard_API int    Printf  (const char* theFormat, ...);
__Standard_API int    Fprintf (FILE* theFile, const char* theFormat, ...);
__Standard_API int    Sprintf (char* theBuffer, const char* theFormat, ...);

//============================================================================
//==== ShallowCopy: Returns a CString
//============================================================================
inline Standard_CString ShallowCopy (const Standard_CString Value)
{
  return Value;
}

//============================================================================
//==== IsSimilar : Returns Standard_True if two booleans have the same value
//============================================================================
inline Standard_Boolean IsSimilar(const Standard_CString One
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
