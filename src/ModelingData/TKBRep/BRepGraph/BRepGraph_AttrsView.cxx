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

#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_Data.hxx>
#include <NCollection_Vector.hxx>

//=================================================================================================

void BRepGraph::AttrsView::Set(const BRepGraph_NodeId                       theNode,
                               const int                                    theKey,
                               const occ::handle<BRepGraph_UserAttribute>& theAttr)
{
  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache != nullptr)
    aCache->SetUserAttribute(theKey, theAttr);
}

//=================================================================================================

occ::handle<BRepGraph_UserAttribute> BRepGraph::AttrsView::Get(const BRepGraph_NodeId theNode, const int theKey) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return occ::handle<BRepGraph_UserAttribute>();
  return aDef->Cache.GetUserAttribute(theKey);
}

//=================================================================================================

bool BRepGraph::AttrsView::Remove(const BRepGraph_NodeId theNode, const int theKey)
{
  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache == nullptr)
    return false;
  return aCache->RemoveUserAttribute(theKey);
}

//=================================================================================================

void BRepGraph::AttrsView::Invalidate(const BRepGraph_NodeId theNode, const int theKey)
{
  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateUserAttribute(theKey);
}

//=================================================================================================

NCollection_Vector<int> BRepGraph::AttrsView::AttributeKeys(const BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return NCollection_Vector<int>();
  return aDef->Cache.UserAttributeKeys();
}
