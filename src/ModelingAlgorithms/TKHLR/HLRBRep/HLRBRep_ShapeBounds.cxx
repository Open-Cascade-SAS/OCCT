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

//=================================================================================================

HLRBRep_ShapeBounds::HLRBRep_ShapeBounds(const occ::handle<HLRTopoBRep_OutLiner>& S,
                                         const occ::handle<Standard_Transient>&   SData,
                                         const int                                nbIso,
                                         const int                                V1,
                                         const int                                V2,
                                         const int                                E1,
                                         const int                                E2,
                                         const int                                F1,
                                         const int                                F2)
    : myShape(S),
      myShapeData(SData),
      myNbIso(nbIso),
      myVertStart(V1),
      myVertEnd(V2),
      myEdgeStart(E1),
      myEdgeEnd(E2),
      myFaceStart(F1),
      myFaceEnd(F2)
{
}

//=================================================================================================

HLRBRep_ShapeBounds::HLRBRep_ShapeBounds(const occ::handle<HLRTopoBRep_OutLiner>& S,
                                         const int                                nbIso,
                                         const int                                V1,
                                         const int                                V2,
                                         const int                                E1,
                                         const int                                E2,
                                         const int                                F1,
                                         const int                                F2)
    : myShape(S),
      myNbIso(nbIso),
      myVertStart(V1),
      myVertEnd(V2),
      myEdgeStart(E1),
      myEdgeEnd(E2),
      myFaceStart(F1),
      myFaceEnd(F2)
{
}

//=================================================================================================

void HLRBRep_ShapeBounds::Translate(const int NV, const int NE, const int NF)
{
  myVertStart += NV;
  myVertEnd += NV;
  myEdgeStart += NE;
  myEdgeEnd += NE;
  myFaceStart += NF;
  myFaceEnd += NF;
}

//=================================================================================================

void HLRBRep_ShapeBounds::Sizes(int& NV, int& NE, int& NF) const
{
  NV = myVertEnd + 1 - myVertStart;
  NE = myEdgeEnd + 1 - myEdgeStart;
  NF = myFaceEnd + 1 - myFaceStart;
}

//=================================================================================================

void HLRBRep_ShapeBounds::Bounds(int& V1, int& V2, int& E1, int& E2, int& F1, int& F2) const
{
  V1 = myVertStart;
  V2 = myVertEnd;
  E1 = myEdgeStart;
  E2 = myEdgeEnd;
  F1 = myFaceStart;
  F2 = myFaceEnd;
}
