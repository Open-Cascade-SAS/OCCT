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
//==== Titre: Standard_ExtCharacter.cxx
//==== Role : the methodes which raise the exceptions are implemented in 
//====        this file.
//============================================================================
#include <stdlib.h>
#include <Standard_ExtCharacter.hxx>
#include <Standard_Integer.hxx>
#include <Standard_RangeError.hxx>

#include <Standard_Type.hxx>
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

const Handle_Standard_Type& Standard_ExtCharacter_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_ExtCharacter",sizeof(Standard_ExtCharacter),
		      0,NULL);
  
  return _aType;
}


// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given ExtCharacter
// ------------------------------------------------------------------
Standard_Integer HashCode(const Standard_ExtCharacter me, 
			  const Standard_Integer Upper)
{
  if (Upper < 1){
    Standard_RangeError::
      Raise("Try to apply HashCode method with negative or null argument.");
  }
  Standard_Integer aCode = me; 
  return ( aCode % Upper) + 1;
}

// ------------------------------------------------------------------
// ShallowDump : Writes a character
// ------------------------------------------------------------------
Standard_EXPORT void ShallowDump (const Standard_ExtCharacter Value 
				  ,Standard_OStream& s)
{   
  s << hex << Value << " Standard_ExtCharacter" << endl;
}
