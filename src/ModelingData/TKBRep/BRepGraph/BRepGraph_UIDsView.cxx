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

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_UID();

  const NCollection_Vector<BRepGraph_UID>& aVec =
    myGraph->myData->myIncStorage.UIDs(theNode.NodeKind);
  if (theNode.Index >= aVec.Length())
    return BRepGraph_UID();
  return aVec.Value(theNode.Index);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return BRepGraph_NodeId();

  const NCollection_Vector<BRepGraph_UID>& aVec = myGraph->myData->myIncStorage.UIDs(theUID.Kind());

  for (int i = 0; i < aVec.Length(); ++i)
  {
    if (aVec.Value(i) == theUID)
      return BRepGraph_NodeId(theUID.Kind(), i);
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration.load())
    return false;

  const NCollection_Vector<BRepGraph_UID>& aVec = myGraph->myData->myIncStorage.UIDs(theUID.Kind());

  for (int i = 0; i < aVec.Length(); ++i)
  {
    if (aVec.Value(i) == theUID)
      return true;
  }
  return false;
}

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const
{
  return myGraph->myData->myGeneration.load();
}

//=================================================================================================

const Standard_GUID& BRepGraph::UIDsView::GraphGUID() const
{
  return myGraph->myData->myGraphGUID;
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return BRepGraph_VersionStamp();

  const NCollection_Vector<BRepGraph_UID>& aVec =
    myGraph->myData->myIncStorage.UIDs(theNode.NodeKind);
  if (theNode.Index >= aVec.Length())
    return BRepGraph_VersionStamp();

  const BRepGraph_UID                aUID = aVec.Value(theNode.Index);
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr || aDef->IsRemoved)
    return BRepGraph_VersionStamp();

  return BRepGraph_VersionStamp(aUID, aDef->MutationGen, myGraph->myData->myGeneration.load());
}

//=================================================================================================

bool BRepGraph::UIDsView::IsStale(const BRepGraph_VersionStamp& theStamp) const
{
  if (!theStamp.IsValid())
    return true;

  if (theStamp.myGeneration != myGraph->myData->myGeneration.load())
    return true;

  const BRepGraph_NodeId aNodeId = NodeIdFrom(theStamp.myUID);
  if (!aNodeId.IsValid())
    return true;

  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(aNodeId);
  if (aDef == nullptr || aDef->IsRemoved)
    return true;

  return aDef->MutationGen != theStamp.myMutationGen;
}
