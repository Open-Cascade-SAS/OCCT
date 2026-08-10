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

#include <BRepGraph_Layer.hxx>

#include <BRepGraph.hxx>
#include <Standard_ProgramError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_Layer, Standard_Transient)

//=================================================================================================

BRepGraph_Layer::BRepGraph_Layer() = default;

//=================================================================================================

const BRepGraph& BRepGraph_Layer::Graph() const
{
  if (myGraph == nullptr)
  {
    throw Standard_ProgramError("BRepGraph_Layer: layer is detached from graph");
  }
  return *myGraph;
}

//=================================================================================================

void BRepGraph_Layer::OnAttached() noexcept {}

//=================================================================================================

void BRepGraph_Layer::OnDetached() noexcept {}

//=================================================================================================

void BRepGraph_Layer::attachGraph(BRepGraph* theGraph) noexcept
{
  if (myGraph != nullptr)
  {
    detachContext();
  }
  myGraph = theGraph;
  if (myGraph != nullptr)
  {
    OnAttached();
  }
}

//=================================================================================================

void BRepGraph_Layer::detachContext() noexcept
{
  if (myGraph == nullptr)
  {
    return;
  }
  OnDetached();
  myGraph = nullptr;
}

//=================================================================================================

int BRepGraph_Layer::SubscribedKinds() const
{
  return 0;
}

//=================================================================================================

void BRepGraph_Layer::OnNodeRemoved(const BRepGraph_NodeId /*theNode*/) noexcept {}

//=================================================================================================

void BRepGraph_Layer::OnItemRemoved(const BRepGraph_ItemId theItem) noexcept
{
  if (!theItem.IsValid())
  {
    return;
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      OnNodeRemoved(theItem.NodeId());
      return;
    case BRepGraph_ItemId::Domain::Reference:
      OnRefRemoved(theItem.RefId());
      return;
    case BRepGraph_ItemId::Domain::None:
      return;
  }
}

//=================================================================================================

void BRepGraph_Layer::OnNodeModified(const BRepGraph_NodeId /*theNode*/) noexcept {}

//=================================================================================================

void BRepGraph_Layer::OnItemModified(const BRepGraph_ItemId theItem) noexcept
{
  if (!theItem.IsValid())
  {
    return;
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      OnNodeModified(theItem.NodeId());
      return;
    case BRepGraph_ItemId::Domain::Reference:
      OnRefModified(theItem.RefId());
      return;
    case BRepGraph_ItemId::Domain::None:
      return;
  }
}

//=================================================================================================

void BRepGraph_Layer::OnNodeReplaced(const BRepGraph_NodeId /*theOldNode*/,
                                     const BRepGraph_NodeId /*theNewNode*/) noexcept
{
}

//=================================================================================================

void BRepGraph_Layer::OnNodesModified(
  const NCollection_Array1<BRepGraph_NodeId>& /*theModifiedNodes*/) noexcept
{
}

//=================================================================================================

int BRepGraph_Layer::SubscribedRefKinds() const
{
  return 0;
}

//=================================================================================================

void BRepGraph_Layer::OnRefRemoved(const BRepGraph_RefId /*theRef*/) noexcept {}

//=================================================================================================

void BRepGraph_Layer::OnRefModified(const BRepGraph_RefId /*theRef*/) noexcept {}

//=================================================================================================

void BRepGraph_Layer::OnRefsModified(
  const NCollection_Array1<BRepGraph_RefId>& /*theModifiedRefs*/) noexcept
{
}
