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

#include <BRepGraphSupInc_CopyContext.hxx>

#include <BRepGraphSupInc_Storage.hxx>

//=================================================================================================

BRepGraphSupInc_CopyContext::BRepGraphSupInc_CopyContext(const BRepGraphSupInc_Storage& theSource,
                                                         BRepGraphSupInc_Storage&       theTarget,
                                                         const Mode                     theMode)
    : mySource(theSource),
      myTarget(theTarget),
      myMode(theMode)
{
}

//=================================================================================================

bool BRepGraphSupInc_CopyContext::AddMapping(const BRepGraphSupInc_ItemUID& theSourceUID,
                                             const BRepGraphSupInc_ItemUID& theTargetUID)
{
  if (!theSourceUID.IsValid() || !theTargetUID.IsValid())
  {
    return false;
  }

  const BRepGraphSupInc_ItemUID* anExisting = myMappings.Seek(theSourceUID);
  if (anExisting != nullptr)
  {
    return *anExisting == theTargetUID;
  }
  myMappings.Bind(theSourceUID, theTargetUID);
  return true;
}

//=================================================================================================

BRepGraphSupInc_ItemUID BRepGraphSupInc_CopyContext::TargetUID(
  const BRepGraphSupInc_ItemUID& theSourceUID) const
{
  const BRepGraphSupInc_ItemUID* aTargetUID = myMappings.Seek(theSourceUID);
  if (aTargetUID != nullptr)
  {
    return *aTargetUID;
  }
  return myMode == Mode::Compact && myTarget.Has(theSourceUID) ? theSourceUID
                                                               : BRepGraphSupInc_ItemUID();
}

//=================================================================================================

bool BRepGraphSupInc_CopyContext::HasTargetUID(const BRepGraphSupInc_ItemUID& theSourceUID) const
{
  return myMappings.Seek(theSourceUID) != nullptr
         || (myMode == Mode::Compact && myTarget.Has(theSourceUID));
}

//=================================================================================================

bool BRepGraphSupInc_CopyContext::AddNodeMapping(const BRepGraph_NodeId theSourceNode,
                                                 const BRepGraph_NodeId theTargetNode)
{
  if (!theSourceNode.IsValid() || !theTargetNode.IsValid())
  {
    return false;
  }
  const BRepGraph_NodeId* aMapped = myNodeMappings.Seek(theSourceNode);
  return aMapped == nullptr ? myNodeMappings.Bind(theSourceNode, theTargetNode)
                            : *aMapped == theTargetNode;
}

//=================================================================================================

BRepGraph_NodeId BRepGraphSupInc_CopyContext::TargetNode(const BRepGraph_NodeId theSourceNode) const
{
  const BRepGraph_NodeId* aMapped = myNodeMappings.Seek(theSourceNode);
  if (aMapped != nullptr)
  {
    return *aMapped;
  }
  return myMode == Mode::Copy && &mySource != &myTarget ? theSourceNode : BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraphSupInc_CopyContext::HasTargetNode(const BRepGraph_NodeId theSourceNode) const
{
  return myNodeMappings.Seek(theSourceNode) != nullptr;
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_CopyContext::TargetStore(
  const Standard_GUID& theID) const
{
  return myTarget.Registry().FindStore(theID);
}

//=================================================================================================

bool BRepGraphSupInc_CopyContext::addPrevalidatedTargetStore(
  const Standard_GUID&                      theID,
  const occ::handle<BRepGraphSupInc_Store>& theStore)
{
  if (theStore.IsNull())
  {
    return false;
  }

  const occ::handle<BRepGraphSupInc_Store>* anExisting = myPrevalidatedStores.Seek(theID);
  return anExisting == nullptr ? myPrevalidatedStores.Bind(theID, theStore)
                               : *anExisting == theStore;
}

//=================================================================================================

occ::handle<BRepGraphSupInc_Store> BRepGraphSupInc_CopyContext::prevalidatedTargetStore(
  const Standard_GUID& theID) const
{
  const occ::handle<BRepGraphSupInc_Store>* aStore = myPrevalidatedStores.Seek(theID);
  return aStore == nullptr ? occ::handle<BRepGraphSupInc_Store>() : *aStore;
}
