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

bool BRepGraph::UIDsView::IsEnabled() const { return myGraph->myData->myUIDEnabled; }

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(BRepGraph_NodeId theNode) const
{
  if (!myGraph->myData->myUIDEnabled)
    return BRepGraph_UID();
  const BRepGraph_UID* aPtr = myGraph->myData->myNodeToUID.Seek(theNode);
  return aPtr != nullptr ? *aPtr : BRepGraph_UID();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{
  const BRepGraph_NodeId* aPtr = myGraph->myData->myUIDToNodeId.Seek(theUID);
  return aPtr != nullptr ? *aPtr : BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  if (!myGraph->myData->myUIDEnabled || !theUID.IsValid())
    return false;
  if (theUID.Generation() != myGraph->myData->myGeneration)
    return false;
  return myGraph->myData->myUIDToNodeId.IsBound(theUID);
}

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const { return myGraph->myData->myGeneration; }
