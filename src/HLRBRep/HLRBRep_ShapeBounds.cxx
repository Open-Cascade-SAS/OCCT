// Created on: 1997-04-17
// Created by: Christophe MARION
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


#include <HLRBRep_ShapeBounds.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <Standard_Transient.hxx>

//=======================================================================
//function : HLRBRep_ShapeBounds
//purpose  : 
//=======================================================================
HLRBRep_ShapeBounds::
HLRBRep_ShapeBounds (const Handle(HLRTopoBRep_OutLiner)& S,
		     const Handle(Standard_Transient)& SData,
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
