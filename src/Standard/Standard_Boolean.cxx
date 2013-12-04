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

#include <Standard_Boolean.hxx>
#include <Standard_RangeError.hxx>
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

const Handle_Standard_Type& Standard_Boolean_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_Boolean",sizeof(Standard_Boolean),0,NULL);
  
  return _aType;
}

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given Integer
// ------------------------------------------------------------------
//Standard_Integer HashCode(const Standard_Boolean me
//			  ,const Standard_Integer Upper)
//{
//  if (Upper < 1){
//    Standard_RangeError::
//      Raise("Try to apply HashCode method with negative or null argument.");
//  }
//  return (me % Upper) + 1;
//}

// ----------------------------------------
// ShallowCopy : Makes a copy of a boolean
// ----------------------------------------
Standard_Boolean ShallowCopy (const Standard_Boolean me) 
{ return me; }

// ------------------------------------------------------------------
// ShallowDump : Writes a boolean value
// ------------------------------------------------------------------
Standard_EXPORT void ShallowDump (const Standard_Boolean Value, Standard_OStream& s)
{ s << Value << " Standard_Boolean" << "\n"; }
