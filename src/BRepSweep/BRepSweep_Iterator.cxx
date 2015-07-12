// Created on: 1993-06-09
// Created by: Laurent BOURESCHE
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


#include <BRepSweep_Iterator.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepSweep_Iterator
//purpose  : 
//=======================================================================
BRepSweep_Iterator::BRepSweep_Iterator()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepSweep_Iterator::Init(const TopoDS_Shape& aShape)
{
  myIterator.Initialize(aShape);
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  BRepSweep_Iterator::Next()
{
  myIterator.Next();
}


