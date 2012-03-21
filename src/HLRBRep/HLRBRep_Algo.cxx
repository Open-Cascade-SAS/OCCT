// Created on: 1994-08-04
// Created by: Christophe MARION
// Copyright (c) 1994-1999 Matra Datavision
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


#define No_Standard_OutOfRange

#include <HLRBRep_Algo.ixx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRTopoBRep_OutLiner.hxx>

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
			 const Handle(MMgt_TShared)& SData,
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
