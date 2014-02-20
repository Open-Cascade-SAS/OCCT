// Created on: 1993-01-28
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Dynamic_CompiledMethod.ixx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_CompiledMethod
//purpose  : 
//=======================================================================

Dynamic_CompiledMethod::Dynamic_CompiledMethod(const Standard_CString aname,
					       const Standard_CString afunction)
     : Dynamic_MethodDefinition(aname)
{
  thefunction = new TCollection_HAsciiString(afunction);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

void Dynamic_CompiledMethod::Function(const Standard_CString afunction)
{
  thefunction = new TCollection_HAsciiString(afunction);
}

//=======================================================================
//function : Function
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_CompiledMethod::Function() const
{
  return thefunction->String();
}
