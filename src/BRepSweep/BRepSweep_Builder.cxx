// Created on: 1993-02-04
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


#include <BRep_Builder.hxx>
#include <BRepSweep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepSweep_Builder
//purpose  : 
//=======================================================================
BRepSweep_Builder::BRepSweep_Builder(const BRep_Builder& aBuilder) :
       myBuilder(aBuilder)
{
}

//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeCompound(TopoDS_Shape& aCompound)const 
{
  myBuilder.MakeCompound(TopoDS::Compound(aCompound));
}


//=======================================================================
//function : MakeCompSolid
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeCompSolid(TopoDS_Shape& aCompSolid)const 
{
  myBuilder.MakeCompSolid(TopoDS::CompSolid(aCompSolid));
}


//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeSolid(TopoDS_Shape& aSolid)const 
{
  myBuilder.MakeSolid(TopoDS::Solid(aSolid));
}


//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeShell(TopoDS_Shape& aShell)const 
{
  myBuilder.MakeShell(TopoDS::Shell(aShell));
}


//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeWire(TopoDS_Shape& aWire)const 
{
  myBuilder.MakeWire(TopoDS::Wire(aWire));
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::Add(TopoDS_Shape& aShape1, 
			     const TopoDS_Shape& aShape2, 
			     const TopAbs_Orientation Orient)const 
{
  TopoDS_Shape aComp = aShape2;
  aComp.Orientation(Orient);
  myBuilder.Add(aShape1,aComp);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::Add(TopoDS_Shape& aShape1, 
			     const TopoDS_Shape& aShape2)const 
{
  myBuilder.Add(aShape1,aShape2);
}


