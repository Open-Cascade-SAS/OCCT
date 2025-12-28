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

#include <Standard_Type.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_LoopAndPath.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Path.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_LoopAndPath, StepShape_TopologicalRepresentationItem)

StepShape_LoopAndPath::StepShape_LoopAndPath() {}

void StepShape_LoopAndPath::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                 const occ::handle<StepShape_Loop>&           aLoop,
                                 const occ::handle<StepShape_Path>&           aPath)
{
  // --- classe own fields ---
  loop = aLoop;
  path = aPath;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_LoopAndPath::Init(const occ::handle<TCollection_HAsciiString>&        aName,
                                 const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& aEdgeList)
{
  // --- classe inherited fields ---

  StepRepr_RepresentationItem::Init(aName);

  // --- ANDOR component fields ---

  loop = new StepShape_Loop();
  loop->Init(aName);

  // --- ANDOR component fields ---

  path = new StepShape_Path();
  path->Init(aName, aEdgeList);
}

void StepShape_LoopAndPath::SetLoop(const occ::handle<StepShape_Loop>& aLoop)
{
  loop = aLoop;
}

occ::handle<StepShape_Loop> StepShape_LoopAndPath::Loop() const
{
  return loop;
}

void StepShape_LoopAndPath::SetPath(const occ::handle<StepShape_Path>& aPath)
{
  path = aPath;
}

occ::handle<StepShape_Path> StepShape_LoopAndPath::Path() const
{
  return path;
}

//--- Specific Methods for AND classe field access ---

//--- Specific Methods for AND classe field access ---

void StepShape_LoopAndPath::SetEdgeList(const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& aEdgeList)
{
  path->SetEdgeList(aEdgeList);
}

occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> StepShape_LoopAndPath::EdgeList() const
{
  return path->EdgeList();
}

occ::handle<StepShape_OrientedEdge> StepShape_LoopAndPath::EdgeListValue(
  const int num) const
{
  return path->EdgeListValue(num);
}

int StepShape_LoopAndPath::NbEdgeList() const
{
  return path->NbEdgeList();
}
