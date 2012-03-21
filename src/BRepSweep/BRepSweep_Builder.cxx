// Created on: 1993-02-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepSweep_Builder.ixx>
#include <TopoDS.hxx>

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


