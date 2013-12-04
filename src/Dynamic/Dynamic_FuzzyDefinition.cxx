// Created on: 1993-01-22
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

#include <Dynamic_FuzzyDefinition.ixx>
#include <TCollection_AsciiString.hxx>


//=======================================================================
//function : Dynamic_FuzzyDefinition
//purpose  : 
//=======================================================================

Dynamic_FuzzyDefinition::Dynamic_FuzzyDefinition(const Standard_CString aname)
{
  thename = new TCollection_HAsciiString(aname);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_FuzzyDefinition::Type() const
{
  return thename->String();
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

//void Dynamic_FuzzyDefinition::Dump(Standard_OStream& astream) const
void Dynamic_FuzzyDefinition::Dump(Standard_OStream& ) const
{}
