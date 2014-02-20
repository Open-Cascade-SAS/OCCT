// Created on: 1997-12-10
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <PDataStd_Expression.ixx>

//=======================================================================
//function : PDataStd_Expression
//purpose  : 
//=======================================================================

PDataStd_Expression::PDataStd_Expression()
{
}

//=======================================================================
//function : PDataStd_Expression
//purpose  : 
//=======================================================================

PDataStd_Expression::PDataStd_Expression(const Handle(PCollection_HExtendedString)& name)
{
  SetName(name);
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

 Handle(PCollection_HExtendedString) PDataStd_Expression::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

 void PDataStd_Expression::SetName(const Handle(PCollection_HExtendedString)& name) 
{
  myName = name;
}


//=======================================================================
//function : GetVariables
//purpose  : 
//=======================================================================

Handle(PDF_HAttributeArray1) PDataStd_Expression::GetVariables() const
{ 
  return myVariables; 
}


//=======================================================================
//function : SetVariables
//purpose  : 
//=======================================================================

void PDataStd_Expression::SetVariables(const Handle(PDF_HAttributeArray1)& vars) 
{ 
  myVariables = vars; 
}
