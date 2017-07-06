// Created on: 1994-08-04
// Created by: Christophe MARION
// Copyright (c) 1994-1999 Matra Datavision
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

#define No_Standard_OutOfRange


#include <HLRBRep_Algo.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_Algo,HLRBRep_InternalAlgo)

//=======================================================================
//function : HLRBRep_Algo
//purpose  : 
//=======================================================================
HLRBRep_Algo::HLRBRep_Algo ()
{}

//=======================================================================
//function : HLRBRep_Algo
//purpose  : 
//=======================================================================

HLRBRep_Algo::HLRBRep_Algo (const Handle(HLRBRep_Algo)& A) :
HLRBRep_InternalAlgo(A)
{}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  HLRBRep_Algo::Add (const TopoDS_Shape& S,
			 const Handle(Standard_Transient)& SData,
			 const Standard_Integer nbIso)
{
  Load(new HLRTopoBRep_OutLiner(S),SData,nbIso);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  HLRBRep_Algo::Add (const TopoDS_Shape& S,
			 const Standard_Integer nbIso)
{
  Load(new HLRTopoBRep_OutLiner(S),nbIso);
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_Algo::Index (const TopoDS_Shape& S)
{
  Standard_Integer n = NbShapes();

  for (Standard_Integer i = 1; i <= n; i++) {
    if (ShapeBounds(i).Shape()->OriginalShape() == S) return i;
    if (ShapeBounds(i).Shape()->OutLinedShape() == S) return i;
  }

  return 0;
}

//=======================================================================
//function : OutLinedShapeNullify
//purpose  : 
//=======================================================================

void HLRBRep_Algo::OutLinedShapeNullify ()
{
  Standard_Integer n = NbShapes();

  for (Standard_Integer i = 1; i <= n; i++) {
    ShapeBounds(i).Shape()->OutLinedShape(TopoDS_Shape());
    ShapeBounds(i).Shape()->DataStructure().Clear();
  }
}
