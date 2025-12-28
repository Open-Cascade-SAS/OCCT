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

#include <BRep_Tool.hxx>
#include <HLRAlgo_WiresBlock.hxx>
#include <HLRBRep_FaceData.hxx>
#include <HLRBRep_Surface.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================

HLRBRep_FaceData::HLRBRep_FaceData()
    : myFlags(0),
      mySize(0)
{
  Selected(true);
}

//=================================================================================================

void HLRBRep_FaceData::Set(const TopoDS_Face&       FG,
                           const TopAbs_Orientation Or,
                           const bool               Cl,
                           const int                NW)
{
  Closed(Cl);
  Geometry().Surface(FG);
  myTolerance = (float)(BRep_Tool::Tolerance(FG));
  Orientation(Or);
  Wires() = new HLRAlgo_WiresBlock(NW);
}

//=================================================================================================

void HLRBRep_FaceData::SetWire(const int WI, const int NE)
{
  Wires()->Set(WI, new HLRAlgo_EdgesBlock(NE));
}

//=================================================================================================

void HLRBRep_FaceData::SetWEdge(const int                WI,
                                const int                EWI,
                                const int                EI,
                                const TopAbs_Orientation Or,
                                const bool               OutL,
                                const bool               Inte,
                                const bool               Dble,
                                const bool               IsoL)
{
  Wires()->Wire(WI)->Edge(EWI, EI);
  Wires()->Wire(WI)->Orientation(EWI, Or);
  Wires()->Wire(WI)->OutLine(EWI, OutL);
  Wires()->Wire(WI)->Internal(EWI, Inte);
  Wires()->Wire(WI)->Double(EWI, Dble);
  Wires()->Wire(WI)->IsoLine(EWI, IsoL);
}
