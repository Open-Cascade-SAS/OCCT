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

#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_TransientCache.hxx>

//=================================================================================================

void BRepGraph::CacheView::Set(const BRepGraph_NodeId                   theNode,
                               const occ::handle<BRepGraph_CacheKind>&  theKind,
                               const occ::handle<BRepGraph_CacheValue>& theValue)
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return;
  }
  myGraph->transientCache().Set(theNode, theKind, theValue, aDef->SubtreeGen);
}

//=================================================================================================

void BRepGraph::CacheView::Set(const BRepGraph_NodeId                   theNode,
                               const int                                theKindSlot,
                               const occ::handle<BRepGraph_CacheValue>& theValue)
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return;
  }
  myGraph->transientCache().Set(theNode, theKindSlot, theValue, aDef->SubtreeGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph::CacheView::Get(
  const BRepGraph_NodeId                  theNode,
  const occ::handle<BRepGraph_CacheKind>& theKind) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return myGraph->transientCache().Get(theNode, theKind, aDef->SubtreeGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph::CacheView::Get(const BRepGraph_NodeId theNode,
                                                            const int theKindSlot) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return myGraph->transientCache().Get(theNode, theKindSlot, aDef->SubtreeGen);
}

//=================================================================================================

bool BRepGraph::CacheView::Has(const BRepGraph_NodeId                  theNode,
                               const occ::handle<BRepGraph_CacheKind>& theKind) const
{
  return !Get(theNode, theKind).IsNull();
}

//=================================================================================================

bool BRepGraph::CacheView::Has(const BRepGraph_NodeId theNode, const int theKindSlot) const
{
  return !Get(theNode, theKindSlot).IsNull();
}

//=================================================================================================

// Remove() intentionally skips IsRemoved checks — stale cache entries
// for removed nodes must still be removable during cleanup.
bool BRepGraph::CacheView::Remove(const BRepGraph_NodeId                  theNode,
                                  const occ::handle<BRepGraph_CacheKind>& theKind)
{
  return myGraph->transientCache().Remove(theNode, theKind);
}

//=================================================================================================

bool BRepGraph::CacheView::Remove(const BRepGraph_NodeId theNode, const int theKindSlot)
{
  return myGraph->transientCache().Remove(theNode, theKindSlot);
}

//=================================================================================================

void BRepGraph::CacheView::Invalidate(const BRepGraph_NodeId                  theNode,
                                      const occ::handle<BRepGraph_CacheKind>& theKind)
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
  {
    return;
  }

  occ::handle<BRepGraph_CacheValue> aValue =
    myGraph->transientCache().Get(theNode, theKind, aDef->SubtreeGen);
  if (!aValue.IsNull())
  {
    aValue->Invalidate();
  }
}

//=================================================================================================

void BRepGraph::CacheView::Invalidate(const BRepGraph_NodeId theNode, const int theKindSlot)
{
  occ::handle<BRepGraph_CacheValue> aValue = Get(theNode, theKindSlot);
  if (!aValue.IsNull())
  {
    aValue->Invalidate();
  }
}

//=================================================================================================

BRepGraph_CacheKindIterator<BRepGraph_NodeId> BRepGraph::CacheView::CacheKindIter(
  const BRepGraph_NodeId theNode) const
{
  BRepGraph_CacheKindIterator<BRepGraph_NodeId> anIt;
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr || aDef->IsRemoved)
  {
    return anIt;
  }

  anIt.myCount =
    myGraph->transientCache().CollectCacheKindSlots(theNode, aDef->SubtreeGen, anIt.mySlots);
  return anIt;
}

//=================================================================================================

void BRepGraph::CacheView::Set(const BRepGraph_RefId                    theRef,
                               const occ::handle<BRepGraph_CacheKind>&  theKind,
                               const occ::handle<BRepGraph_CacheValue>& theValue)
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return;
  }
  myGraph->refTransientCache().Set(theRef, theKind, theValue, aRef->OwnGen);
}

//=================================================================================================

void BRepGraph::CacheView::Set(const BRepGraph_RefId                    theRef,
                               const int                                theKindSlot,
                               const occ::handle<BRepGraph_CacheValue>& theValue)
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return;
  }
  myGraph->refTransientCache().Set(theRef, theKindSlot, theValue, aRef->OwnGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph::CacheView::Get(
  const BRepGraph_RefId                   theRef,
  const occ::handle<BRepGraph_CacheKind>& theKind) const
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return myGraph->refTransientCache().Get(theRef, theKind, aRef->OwnGen);
}

//=================================================================================================

occ::handle<BRepGraph_CacheValue> BRepGraph::CacheView::Get(const BRepGraph_RefId theRef,
                                                            const int theKindSlot) const
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return occ::handle<BRepGraph_CacheValue>();
  }
  return myGraph->refTransientCache().Get(theRef, theKindSlot, aRef->OwnGen);
}

//=================================================================================================

bool BRepGraph::CacheView::Has(const BRepGraph_RefId                   theRef,
                               const occ::handle<BRepGraph_CacheKind>& theKind) const
{
  return !Get(theRef, theKind).IsNull();
}

//=================================================================================================

bool BRepGraph::CacheView::Has(const BRepGraph_RefId theRef, const int theKindSlot) const
{
  return !Get(theRef, theKindSlot).IsNull();
}

//=================================================================================================

// Remove() intentionally skips IsRemoved checks — stale cache entries
// for removed refs must still be removable during cleanup.
bool BRepGraph::CacheView::Remove(const BRepGraph_RefId                   theRef,
                                  const occ::handle<BRepGraph_CacheKind>& theKind)
{
  return myGraph->refTransientCache().Remove(theRef, theKind);
}

//=================================================================================================

bool BRepGraph::CacheView::Remove(const BRepGraph_RefId theRef, const int theKindSlot)
{
  return myGraph->refTransientCache().Remove(theRef, theKindSlot);
}

//=================================================================================================

void BRepGraph::CacheView::Invalidate(const BRepGraph_RefId                   theRef,
                                      const occ::handle<BRepGraph_CacheKind>& theKind)
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return;
  }

  occ::handle<BRepGraph_CacheValue> aValue =
    myGraph->refTransientCache().Get(theRef, theKind, aRef->OwnGen);
  if (!aValue.IsNull())
  {
    aValue->Invalidate();
  }
}

//=================================================================================================

void BRepGraph::CacheView::Invalidate(const BRepGraph_RefId theRef, const int theKindSlot)
{
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return;
  }

  occ::handle<BRepGraph_CacheValue> aValue =
    myGraph->refTransientCache().Get(theRef, theKindSlot, aRef->OwnGen);
  if (!aValue.IsNull())
  {
    aValue->Invalidate();
  }
}

//=================================================================================================

BRepGraph_CacheKindIterator<BRepGraph_RefId> BRepGraph::CacheView::CacheKindIter(
  const BRepGraph_RefId theRef) const
{
  BRepGraph_CacheKindIterator<BRepGraph_RefId> anIt;
  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRef);
  if (aRef == nullptr || aRef->IsRemoved)
  {
    return anIt;
  }

  anIt.myCount = myGraph->refTransientCache().CollectCacheKindSlots(theRef,
                                                                    aRef->OwnGen,
                                                                    anIt.mySlots);
  return anIt;
}

