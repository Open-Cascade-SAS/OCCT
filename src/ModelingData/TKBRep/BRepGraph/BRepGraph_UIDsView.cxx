// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Data.hxx>

namespace
{

//! Return the per-kind forward UID vector for a given Kind.
const NCollection_Vector<BRepGraph_UID>* uidVecForKind(const BRepGraph_Data&  theData,
                                                       BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return &theData.mySolids.UIDs;
    case BRepGraph_NodeId::Kind::Shell:     return &theData.myShells.UIDs;
    case BRepGraph_NodeId::Kind::Face:      return &theData.myFaces.UIDs;
    case BRepGraph_NodeId::Kind::Wire:      return &theData.myWires.UIDs;
    case BRepGraph_NodeId::Kind::Edge:      return &theData.myEdges.UIDs;
    case BRepGraph_NodeId::Kind::Vertex:    return &theData.myVertices.UIDs;
    case BRepGraph_NodeId::Kind::Compound:  return &theData.myCompounds.UIDs;
    case BRepGraph_NodeId::Kind::CompSolid: return &theData.myCompSolids.UIDs;
    case BRepGraph_NodeId::Kind::Surface:   return &theData.mySurfaces.UIDs;
    case BRepGraph_NodeId::Kind::Curve:     return &theData.myCurves.UIDs;
    default: return nullptr;
  }
}

} // namespace

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_UID();

  const NCollection_Vector<BRepGraph_UID>* aVec = uidVecForKind(*myGraph->myData, theNode.NodeKind);
  if (aVec == nullptr || theNode.Index >= aVec->Length())
    return BRepGraph_UID();
  return aVec->Value(theNode.Index);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_NodeId();

  // Linear scan of the matching per-kind forward vector.
  const NCollection_Vector<BRepGraph_UID>* aVec = uidVecForKind(*myGraph->myData, theUID.Kind());
  if (aVec == nullptr)
    return BRepGraph_NodeId();

  for (int i = 0; i < aVec->Length(); ++i)
  {
    if (aVec->Value(i) == theUID)
      return BRepGraph_NodeId(theUID.Kind(), i);
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration)
    return false;

  const NCollection_Vector<BRepGraph_UID>* aVec = uidVecForKind(*myGraph->myData, theUID.Kind());
  if (aVec == nullptr)
    return false;

  for (int i = 0; i < aVec->Length(); ++i)
  {
    if (aVec->Value(i) == theUID)
      return true;
  }
  return false;
}

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const { return myGraph->myData->myGeneration; }
