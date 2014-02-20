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

//============================================================================
//==== Titre: Standard_Address.hxx
//==== Role : The headr file of primitve type "Address" from package "Standard"
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef char* Address;
//============================================================================

#ifndef _Standard_Address_HeaderFile
#define _Standard_Address_HeaderFile

# include <string.h>
//typedef void* Standard_Address;


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif


class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_Address_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_Address, Standard_OStream& );
//============================================================================
//==== ShallowCopy: Returns a CString
//============================================================================
inline Standard_Address ShallowCopy (const Standard_Address Value)
{
  return Value;
}

//============================================================================
//==== HashCode : Returns a HashCode CString
//============================================================================
inline Standard_Integer HashCode (const Standard_Address Value, 
				  const Standard_Integer Upper)
{
  union {Standard_Address L ;
         Standard_Integer I[2] ;} U ;
  U.I[0] = 0 ;
  U.I[1] = 0 ;
  U.L = Value;
  return HashCode( ( ( U.I[0] ^ U.I[1] ) & 0x7fffffff )  , Upper ) ;
}


//============================================================================
//==== IsSimilar : Returns Standard_True if two booleans have the same value
//============================================================================
inline Standard_Boolean IsSimilar(const Standard_Address One
				 ,const Standard_Address Two)
{ return One == Two; }

//============================================================================
// IsEqual : Returns Standard_True if two CString have the same value
//============================================================================

inline Standard_Boolean IsEqual(const Standard_Address One
			       ,const Standard_Address Two)
{ return One == Two; }

#endif
