// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
inline Standard_Boolean ISEQUAL(const Standard_CString One ,
                                const Standard_Integer LenOne ,
			        const Standard_CString Two,
                                const Standard_Integer LenTwo );
__Standard_API Standard_Boolean ISSIMILAR(const Standard_CString One ,
                                          const Standard_Integer Len ,
			                  const Standard_CString Two );
inline Standard_Integer HASHCODE (const Standard_CString,
                                  const Standard_Integer,
                                  const Standard_Integer);
inline Standard_Integer HASHCODE (const Standard_CString,
                                  const Standard_Integer,
                                  const Standard_Integer ,
                                  Standard_Integer& );
__Standard_API Standard_Integer HASHCODES (const Standard_CString,
                                           const Standard_Integer);

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

//============================================================================
//==== HashCode of CString converted to uppercase. Returns the HashCode itself
//====                               and the HashCode % Upper
//============================================================================
inline Standard_Integer HASHCODE (const Standard_CString Value,
                                  const Standard_Integer Len ,
			          const Standard_Integer Upper ,
                                  Standard_Integer& aHashCode )
{
  aHashCode = HASHCODES( Value , Len );
//  return (Abs( aHashCode ) % Upper ) + 1 ;
  return HashCode( (Standard_Integer) aHashCode , Upper ) ;
}

//============================================================================
//==== HashCode of a CString converted to uppercase
//============================================================================
inline Standard_Integer HASHCODE (const Standard_CString Value,
                                  const Standard_Integer Len ,
                                  const Standard_Integer Upper)
{
//  return (Abs( HASHCODES( Value , Len ) ) % Upper ) + 1 ;
  return HashCode( (Standard_Integer) HASHCODES( Value , Len ) , Upper ) ;
}

//============================================================================
// IsEqual : Returns Standard_True if two CString have the same value
// Comparison is done with discard of bit 5 (UpperCase<-->LowerCase)
// Efficient for Types and MethodNames (without copy of characters)
// Valid if we have only alphanumeric characters and "_" (unicity)
// Valid if the Strings address are aligned for Integers

//============================================================================
inline Standard_Boolean ISEQUAL(const Standard_CString One ,
                                const Standard_Integer LenOne ,
                                const Standard_CString Two,
                                const Standard_Integer LenTwo )
{

 if ( One == Two )
   return Standard_True ;
 if ( LenOne != LenTwo )
   return Standard_False ;

 return ISSIMILAR( One , LenOne , Two ) ;

}

#endif
