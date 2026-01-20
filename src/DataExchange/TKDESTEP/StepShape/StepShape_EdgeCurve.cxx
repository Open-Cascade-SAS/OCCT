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

#include <StepGeom_Curve.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_Vertex.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_EdgeCurve, StepShape_Edge)

StepShape_EdgeCurve::StepShape_EdgeCurve() {}

void StepShape_EdgeCurve::Init(const occ::handle<TCollection_HAsciiString>& aName,
                               const occ::handle<StepShape_Vertex>&         aEdgeStart,
                               const occ::handle<StepShape_Vertex>&         aEdgeEnd,
                               const occ::handle<StepGeom_Curve>&           aEdgeGeometry,
                               const bool                  aSameSense)
{
  // --- classe own fields ---
  edgeGeometry = aEdgeGeometry;
  sameSense    = aSameSense;
  // --- classe inherited fields ---
  StepShape_Edge::Init(aName, aEdgeStart, aEdgeEnd);
}

void StepShape_EdgeCurve::SetEdgeGeometry(const occ::handle<StepGeom_Curve>& aEdgeGeometry)
{
  edgeGeometry = aEdgeGeometry;
}

occ::handle<StepGeom_Curve> StepShape_EdgeCurve::EdgeGeometry() const
{
  return edgeGeometry;
}

void StepShape_EdgeCurve::SetSameSense(const bool aSameSense)
{
  sameSense = aSameSense;
}

bool StepShape_EdgeCurve::SameSense() const
{
  return sameSense;
}
