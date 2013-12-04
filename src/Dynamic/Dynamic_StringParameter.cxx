// Created on: 1993-02-03
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Dynamic_StringParameter.ixx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_StringParameter
//purpose  : 
//=======================================================================

Dynamic_StringParameter::Dynamic_StringParameter(const Standard_CString aparameter)
: Dynamic_Parameter(aparameter)
{}

//=======================================================================
//function : Dynamic_StringParameter
//purpose  : 
//=======================================================================

Dynamic_StringParameter::Dynamic_StringParameter(const Standard_CString aparameter,
						 const Standard_CString avalue)
: Dynamic_Parameter(aparameter)
{
  thestring = new TCollection_HAsciiString(avalue);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_StringParameter::Value() const
{
  return thestring->String();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Dynamic_StringParameter::Value(const Standard_CString avalue)
{
  thestring = new TCollection_HAsciiString(avalue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_StringParameter::Dump(Standard_OStream& astream) const
{
  Dynamic_Parameter::Dump(astream);
  TCollection_AsciiString string = thestring->String();
  astream<<" "<<string.ToCString();
}
