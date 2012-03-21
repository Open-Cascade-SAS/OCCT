// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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


#include <HLRBRep_ShapeBounds.ixx>

//=======================================================================
//function : HLRBRep_ShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_ShapeBounds::
HLRBRep_ShapeBounds (const Handle(HLRTopoBRep_OutLiner)& S,
		     const Handle(MMgt_TShared)& SData,
		     const Standard_Integer nbIso,
		     const Standard_Integer V1,
		     const Standard_Integer V2,
		     const Standard_Integer E1,
		     const Standard_Integer E2,
		     const Standard_Integer F1,
		     const Standard_Integer F2) :
		     myShape(S),
		     myShapeData(SData),
		     myNbIso(nbIso),
		     myVertStart(V1),myVertEnd(V2),
		     myEdgeStart(E1),myEdgeEnd(E2),
		     myFaceStart(F1),myFaceEnd(F2)
{}

//=======================================================================
//function : HLRBRep_ShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_ShapeBounds::
HLRBRep_ShapeBounds (const Handle(HLRTopoBRep_OutLiner)& S,
		     const Standard_Integer nbIso,
		     const Standard_Integer V1,
		     const Standard_Integer V2,
		     const Standard_Integer E1,
		     const Standard_Integer E2,
		     const Standard_Integer F1,
		     const Standard_Integer F2) :
		     myShape(S),
		     myNbIso(nbIso),
		     myVertStart(V1),myVertEnd(V2),
		     myEdgeStart(E1),myEdgeEnd(E2),
		     myFaceStart(F1),myFaceEnd(F2)
{}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Translate (const Standard_Integer NV,
				     const Standard_Integer NE,
				     const Standard_Integer NF)
{
  myVertStart += NV;
  myVertEnd   += NV;
  myEdgeStart += NE;
  myEdgeEnd   += NE;
  myFaceStart += NF;
  myFaceEnd   += NF;
}

//=======================================================================
//function : Sizes
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Sizes (Standard_Integer& NV,
				 Standard_Integer& NE,
				 Standard_Integer& NF) const
{
  NV = myVertEnd + 1 - myVertStart;
  NE = myEdgeEnd + 1 - myEdgeStart;
  NF = myFaceEnd + 1 - myFaceStart;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Bounds (Standard_Integer& V1,
				  Standard_Integer& V2,
				  Standard_Integer& E1,
				  Standard_Integer& E2,
				  Standard_Integer& F1,
				  Standard_Integer& F2) const
{
  V1 = myVertStart;
  V2 = myVertEnd;
  E1 = myEdgeStart;
  E2 = myEdgeEnd;
  F1 = myFaceStart;
  F2 = myFaceEnd;
}

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::UpdateMinMax (const Standard_Address TotMinMax)
{
  for (Standard_Integer i = 0; i <= 15; i++)
    myMinMax[i] = ((Standard_Integer*)TotMinMax)[i];
}
