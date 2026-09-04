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

#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>
#include <Standard_NotImplemented.hxx>
#include <TCollection_HAsciiString.hxx>

#include <NCollection_FlatMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedEdge, StepShape_Edge)

namespace
{
//! Resolve an oriented-edge endpoint iteratively and reject cyclic edge references.
occ::handle<StepShape_Vertex> resolveVertex(const occ::handle<StepShape_Edge>& theEdgeElement,
                                            const bool                         theToStart)
{
  NCollection_FlatMap<occ::handle<StepShape_Edge>> aVisitedEdges;
  occ::handle<StepShape_Edge>                      anEdge  = theEdgeElement;
  bool                                             isStart = theToStart;
  while (!anEdge.IsNull())
  {
    const occ::handle<StepShape_OrientedEdge> anOrientedEdge =
      occ::down_cast<StepShape_OrientedEdge>(anEdge);
    if (anOrientedEdge.IsNull())
    {
      return isStart ? anEdge->EdgeStart() : anEdge->EdgeEnd();
    }
    if (!aVisitedEdges.Add(anEdge))
    {
      return nullptr;
    }
    if (!anOrientedEdge->Orientation())
    {
      isStart = !isStart;
    }
    anEdge = anOrientedEdge->EdgeElement();
  }
  return nullptr;
}
} // namespace

StepShape_OrientedEdge::StepShape_OrientedEdge() = default;

void StepShape_OrientedEdge::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                  const occ::handle<StepShape_Edge>&           aEdgeElement,
                                  const bool                                   aOrientation)
{
  // --- classe own fields ---
  edgeElement = aEdgeElement;
  orientation = aOrientation;
  // --- classe inherited fields ---
  occ::handle<StepShape_Vertex> aEdgeStart;
  aEdgeStart.Nullify();
  occ::handle<StepShape_Vertex> aEdgeEnd;
  aEdgeEnd.Nullify();
  StepShape_Edge::Init(aName, aEdgeStart, aEdgeEnd);
}

void StepShape_OrientedEdge::SetEdgeElement(const occ::handle<StepShape_Edge>& aEdgeElement)
{
  edgeElement = aEdgeElement;
}

occ::handle<StepShape_Edge> StepShape_OrientedEdge::EdgeElement() const
{
  return edgeElement;
}

void StepShape_OrientedEdge::SetOrientation(const bool aOrientation)
{
  orientation = aOrientation;
}

bool StepShape_OrientedEdge::Orientation() const
{
  return orientation;
}

void StepShape_OrientedEdge::SetEdgeStart(const occ::handle<StepShape_Vertex>& /*aEdgeStart*/)
{
  throw Standard_NotImplemented(
    "StepShape_OrientedEdge::SetEdgeStart: redefined field cannot be set");
}

occ::handle<StepShape_Vertex> StepShape_OrientedEdge::EdgeStart() const
{
  return resolveVertex(edgeElement, Orientation());
}

void StepShape_OrientedEdge::SetEdgeEnd(const occ::handle<StepShape_Vertex>& /*aEdgeEnd*/)
{
  throw Standard_NotImplemented(
    "StepShape_OrientedEdge::SetEdgeEnd: redefined field cannot be set");
}

occ::handle<StepShape_Vertex> StepShape_OrientedEdge::EdgeEnd() const
{
  return resolveVertex(edgeElement, !Orientation());
}
