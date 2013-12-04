// Created on: 1997-12-10
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <PDataStd_Variable.ixx>

//=======================================================================
//function : PDataStd_Variable
//purpose  : 
//=======================================================================

PDataStd_Variable::PDataStd_Variable() : isConstant(Standard_False)
{
}

//=======================================================================
//function : PDataStd_Variable
//purpose  : 
//=======================================================================

PDataStd_Variable::PDataStd_Variable(const Standard_Boolean constant)
{
  Constant(constant);
}

//=======================================================================
//function : Constant
//purpose  : 
//=======================================================================

void PDataStd_Variable::Constant(const Standard_Boolean status) 
{
  isConstant = status;
}
//=======================================================================
//function : Constant
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_Variable::Constant() const
{
  return isConstant;
}
//=======================================================================
//function : Unit
//purpose  : 
//=======================================================================

void PDataStd_Variable::Unit(const Handle(PCollection_HAsciiString)& unit) 
{
  myUnit = unit;
}
//=======================================================================
//function : Unit
//purpose  : 
//=======================================================================
Handle(PCollection_HAsciiString) PDataStd_Variable::Unit() const
{
  return myUnit;
}
