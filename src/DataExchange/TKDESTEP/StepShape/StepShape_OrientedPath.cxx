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

#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_OrientedPath.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedPath, StepShape_Path)

StepShape_OrientedPath::StepShape_OrientedPath() {}

void StepShape_OrientedPath::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                  const occ::handle<StepShape_EdgeLoop>&       aPathElement,
                                  const bool                                   aOrientation)
{
  // --- classe own fields ---
  pathElement = aPathElement;
  orientation = aOrientation;
  // --- classe inherited fields ---
  occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> aEdgeList;
  aEdgeList.Nullify();
  StepShape_Path::Init(aName, aEdgeList);
}

void StepShape_OrientedPath::SetPathElement(const occ::handle<StepShape_EdgeLoop>& aPathElement)
{
  pathElement = aPathElement;
}

occ::handle<StepShape_EdgeLoop> StepShape_OrientedPath::PathElement() const
{
  return pathElement;
}

void StepShape_OrientedPath::SetOrientation(const bool aOrientation)
{
  orientation = aOrientation;
}

bool StepShape_OrientedPath::Orientation() const
{
  return orientation;
}

void StepShape_OrientedPath::SetEdgeList(
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>>& /*aEdgeList*/)
{
  // WARNING : the field is redefined.
  // field set up forbidden.
  std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> StepShape_OrientedPath::
  EdgeList() const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  if (orientation)
    return pathElement->EdgeList();
  else
  {
    // on devrait creer un nouveau tableau d'oriented edge classe dans
    // l'ordre inverse - on fera plus tard
    return pathElement->EdgeList();
  }
}

occ::handle<StepShape_OrientedEdge> StepShape_OrientedPath::EdgeListValue(const int num) const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  if (orientation)
    return pathElement->EdgeListValue(num);
  else
  {
    int nbEdges = pathElement->NbEdgeList();
    return pathElement->EdgeListValue(nbEdges - num + 1);
  }
}

int StepShape_OrientedPath::NbEdgeList() const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  return pathElement->NbEdgeList();
}
