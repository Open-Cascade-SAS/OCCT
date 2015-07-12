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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_EdgesBlock.hxx>
#include <HLRAlgo_WiresBlock.hxx>
#include <HLRBRep_FaceData.hxx>
#include <HLRBRep_FaceIterator.hxx>

//=======================================================================
//function : FaceIterator
//purpose  : 
//=======================================================================
HLRBRep_FaceIterator::HLRBRep_FaceIterator()
{}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::InitEdge(HLRBRep_FaceData& fd)
{
  iWire = 0;
  myWires = fd.Wires();
  nbWires = myWires->NbWires();

  iEdge = 0;
  nbEdges = 0;
  NextEdge();
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceIterator::NextEdge()
{
  iEdge++;
  if (iEdge > nbEdges) {
    iWire++;
    if (iWire <= nbWires) {
      iEdge = 1;
      myEdges = myWires->Wire(iWire);
      nbEdges = myEdges->NbEdges();
    }
  }
}
