// Created on: 1993-06-25
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


#include <BRepSweep_Prism.ixx>
#include <BRepSweep_Translation.hxx>
#include <Sweep_NumShape.hxx>

#include <gp_Trsf.hxx>
#include <Precision.hxx>
//=======================================================================
//function : BRepSweep_Prism
//purpose  : 
//=======================================================================

BRepSweep_Prism::BRepSweep_Prism
  (const TopoDS_Shape& S, 
   const gp_Vec& V,
   const Standard_Boolean C,
   const Standard_Boolean Canonize):
  myTranslation(S,
		NumShape(),
		Location(V),
		V,
		C,
		Canonize)
{
  Standard_ConstructionError_Raise_if
    (V.Magnitude()<=Precision::Confusion(),"BRepSweep_Prism::Constructor");
}

//=======================================================================
//function : BRepSweep_Prism
//purpose  : 
//=======================================================================

BRepSweep_Prism::BRepSweep_Prism
  (const TopoDS_Shape& S, 
   const gp_Dir& D, 
   const Standard_Boolean Inf,
   const Standard_Boolean C,
   const Standard_Boolean Canonize):
  myTranslation(S,
		NumShape(Inf),
		Location(D),
		D,
		C,
		Canonize)
{
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::Shape()
{
  return myTranslation.Shape();
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::Shape(const TopoDS_Shape& aGenS)
{
  return myTranslation.Shape(aGenS);
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::FirstShape()
{
  return myTranslation.FirstShape();
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::FirstShape(const TopoDS_Shape& aGenS)
{
  return myTranslation.FirstShape(aGenS);
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::LastShape()
{
  return myTranslation.LastShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Prism::LastShape(const TopoDS_Shape& aGenS)
{
  return myTranslation.LastShape(aGenS);
}


//=======================================================================
//function : Vec
//purpose  : 
//=======================================================================

gp_Vec BRepSweep_Prism::Vec()const
{
  return myTranslation.Vec();
}


//=======================================================================
//function : NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape  BRepSweep_Prism::NumShape()const 
{
  return Sweep_NumShape(2,TopAbs_EDGE);
}


//=======================================================================
//function : NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape  BRepSweep_Prism::NumShape(const Standard_Boolean Inf)const 
{
  Sweep_NumShape N;
  if(Inf){
    N.Init(0,TopAbs_EDGE,Standard_False,Standard_True,Standard_True);
  }
  else{
    N.Init(1,TopAbs_EDGE,Standard_False,Standard_False,Standard_True);
  }
  return N;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

TopLoc_Location  BRepSweep_Prism::Location(const gp_Vec& V)const 
{
  gp_Trsf gpt;
  gpt.SetTranslation(V);
  TopLoc_Location L(gpt);
  return L;
}
