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

#ifndef _HLRBRep_FaceIterator_HeaderFile
#define _HLRBRep_FaceIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
class HLRAlgo_WiresBlock;
class HLRAlgo_EdgesBlock;
class HLRBRep_FaceData;

class HLRBRep_FaceIterator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_FaceIterator();

  //! Begin an exploration of the edges of the face <fd>
  Standard_EXPORT void InitEdge(HLRBRep_FaceData& fd);

  bool MoreEdge() const;

  Standard_EXPORT void NextEdge();

  //! Returns True if the current edge is the first of a
  //! wire.
  bool BeginningOfWire() const;

  //! Returns True if the current edge is the last of a
  //! wire.
  bool EndOfWire() const;

  //! Skip the current wire in the exploration.
  void SkipWire();

  //! Returns the edges of the current wire.
  occ::handle<HLRAlgo_EdgesBlock> Wire() const;

  int Edge() const;

  TopAbs_Orientation Orientation() const;

  bool OutLine() const;

  bool Internal() const;

  bool Double() const;

  bool IsoLine() const;

private:
  int           iWire;
  int           nbWires;
  int           iEdge;
  int           nbEdges;
  occ::handle<HLRAlgo_WiresBlock> myWires;
  occ::handle<HLRAlgo_EdgesBlock> myEdges;
};

#include <HLRBRep_FaceIterator.lxx>

#endif // _HLRBRep_FaceIterator_HeaderFile
