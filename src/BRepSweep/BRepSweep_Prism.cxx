// Created on: 1993-06-25
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


#include <BRepSweep_Prism.hxx>
#include <BRepSweep_Translation.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Sweep_NumShape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

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

//=======================================================================
//function : IsUsed
//purpose  : 
//=======================================================================
Standard_Boolean BRepSweep_Prism::IsUsed(const TopoDS_Shape& aGenS) const
{
  return myTranslation.IsUsed(aGenS);
}

//=======================================================================
//function : GenIsUsed
//purpose  : 
//=======================================================================
Standard_Boolean BRepSweep_Prism::GenIsUsed(const TopoDS_Shape& aGenS) const
{
  return myTranslation.GenIsUsed(aGenS);
}
