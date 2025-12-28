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

#include <StepShape_Path.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Path, StepShape_TopologicalRepresentationItem)

StepShape_Path::StepShape_Path() {}

void StepShape_Path::Init(
  const occ::handle<TCollection_HAsciiString>&                                 aName,
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& aEdgeList)
{
  // --- classe own fields ---
  edgeList = aEdgeList;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Path::SetEdgeList(
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& aEdgeList)
{
  edgeList = aEdgeList;
}

occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> StepShape_Path::EdgeList()
  const
{
  return edgeList;
}

occ::handle<StepShape_OrientedEdge> StepShape_Path::EdgeListValue(const int num) const
{
  return edgeList->Value(num);
}

int StepShape_Path::NbEdgeList() const
{
  if (edgeList.IsNull())
    return 0;
  return edgeList->Length();
}
