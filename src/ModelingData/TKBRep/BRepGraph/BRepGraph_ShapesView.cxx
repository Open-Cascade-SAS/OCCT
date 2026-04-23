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

#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraphInc_Reconstruct.hxx>

#include <BRep_Builder.hxx>
#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>
#include <TopoDS_Compound.hxx>

#include <shared_mutex>

namespace
{
struct BRepGraph_ReconstructionContext
{
  const BRepGraph*                     Graph        = nullptr;
  const BRepGraphInc_Storage*          Storage      = nullptr;
  const BRepGraph_LayerParam*          Params       = nullptr;
  const BRepGraph_LayerRegularity*     Regularities = nullptr;
  BRepGraphInc_Reconstruct::Cache      Cache;
  NCollection_Map<BRepGraph_ProductId> ActiveProducts;
};

static TopoDS_Shape reconstructProductLocal(
  BRepGraph_ReconstructionContext& theContext,
  const BRepGraph_ProductId        theProduct,
  const BRepGraph_OccurrenceId     theParentOccurrence         = BRepGraph_OccurrenceId(),
  const bool                       theFilterByParentOccurrence = false);

static TopoDS_Shape reconstructOccurrenceLocal(BRepGraph_ReconstructionContext& theContext,
                                               const BRepGraph_OccurrenceId     theOccurrence,
                                               const TopLoc_Location&           theLocalLocation)
{
  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
    return TopoDS_Shape();

  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (anOccurrence.IsRemoved || !anOccurrence.ChildDefId.IsValid())
    return TopoDS_Shape();

  TopoDS_Shape aShape;
  if (anOccurrence.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    aShape = reconstructProductLocal(theContext,
                                     BRepGraph_ProductId(anOccurrence.ChildDefId),
                                     theOccurrence,
                                     true);
  }
  else
  {
    aShape = BRepGraphInc_Reconstruct::Node(aStorage,
                                            anOccurrence.ChildDefId,
                                            theContext.Cache,
                                            theContext.Params,
                                            theContext.Regularities);
  }
  if (!aShape.IsNull() && !theLocalLocation.IsIdentity())
    aShape.Move(theLocalLocation);
  return aShape;
}

static TopoDS_Shape reconstructProductLocal(BRepGraph_ReconstructionContext& theContext,
                                            const BRepGraph_ProductId        theProduct,
                                            const BRepGraph_OccurrenceId     theParentOccurrence,
                                            const bool theFilterByParentOccurrence)
{
  (void)theParentOccurrence; // Reserved for future parent-occurrence filtering.
  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
    return TopoDS_Shape();

  const BRepGraph_NodeId aProductNode = theProduct;
  if (!theFilterByParentOccurrence)
  {
    if (const TopoDS_Shape* aCached = theContext.Cache.Seek(aProductNode))
      return *aCached;
  }

  const BRepGraphInc::ProductDef& aProduct = aStorage.Product(theProduct);
  if (aProduct.IsRemoved || theContext.ActiveProducts.Contains(theProduct))
    return TopoDS_Shape();

  theContext.ActiveProducts.Add(theProduct);

  // Find the shape root: scan occurrence refs for a topology (non-product) child.
  TopoDS_Shape     aResult;
  BRepGraph_NodeId aShapeRootNode;
  TopLoc_Location  aRootLocation;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*theContext.Graph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    const BRepGraphInc::OccurrenceDef& aDef = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (aDef.ChildDefId.IsValid() && aDef.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
    {
      aShapeRootNode = aDef.ChildDefId;
      aRootLocation  = aRef.LocalLocation;
      break;
    }
  }
  if (aShapeRootNode.IsValid())
  {
    aResult = BRepGraphInc_Reconstruct::Node(aStorage,
                                             aShapeRootNode,
                                             theContext.Cache,
                                             theContext.Params,
                                             theContext.Regularities);
    if (!aResult.IsNull() && !aRootLocation.IsIdentity())
      aResult.Move(aRootLocation);
  }
  else
  {
    BRep_Builder    aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);

    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*theContext.Graph, theProduct); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& anOccurrenceRef =
        aStorage.OccurrenceRef(anOccIt.CurrentId());
      if (!anOccurrenceRef.OccurrenceDefId.IsValid(aStorage.NbOccurrences()))
      {
        continue;
      }

      const BRepGraphInc::OccurrenceDef& anOccurrence =
        aStorage.Occurrence(anOccurrenceRef.OccurrenceDefId);
      if (anOccurrence.IsRemoved)
      {
        continue;
      }

      TopoDS_Shape aChild = reconstructOccurrenceLocal(theContext,
                                                       anOccurrenceRef.OccurrenceDefId,
                                                       anOccurrenceRef.LocalLocation);
      if (!aChild.IsNull())
        aBuilder.Add(aCompound, aChild);
    }

    aResult = aCompound;
  }

  theContext.ActiveProducts.Remove(theProduct);
  if (!theFilterByParentOccurrence && !aResult.IsNull())
    theContext.Cache.Bind(aProductNode, aResult);
  return aResult;
}

static TopoDS_Shape reconstructShape(BRepGraph_ReconstructionContext& theContext,
                                     const BRepGraph_NodeId           theNode)
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  const BRepGraphInc_Storage& aStorage = *theContext.Storage;
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Product:
      return reconstructProductLocal(theContext, BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anOccurrence(theNode);
      if (!anOccurrence.IsValid(aStorage.NbOccurrences()))
        return TopoDS_Shape();

      const BRepGraphInc::OccurrenceDef& anOccurrenceDef = aStorage.Occurrence(anOccurrence);
      if (anOccurrenceDef.IsRemoved || !anOccurrenceDef.ChildDefId.IsValid())
        return TopoDS_Shape();

      TopoDS_Shape aShape;
      if (anOccurrenceDef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        aShape = reconstructProductLocal(theContext,
                                         BRepGraph_ProductId(anOccurrenceDef.ChildDefId),
                                         anOccurrence,
                                         true);
      }
      else
      {
        aShape = BRepGraphInc_Reconstruct::Node(aStorage,
                                                anOccurrenceDef.ChildDefId,
                                                theContext.Cache,
                                                theContext.Params,
                                                theContext.Regularities);
      }
      if (!aShape.IsNull())
      {
        const TopLoc_Location aGlobalLocation =
          theContext.Graph->Topo().Occurrences().OccurrenceLocation(anOccurrence);
        if (!aGlobalLocation.IsIdentity())
          aShape.Move(aGlobalLocation);
      }
      return aShape;
    }
    default:
      return BRepGraphInc_Reconstruct::Node(aStorage,
                                            theNode,
                                            theContext.Cache,
                                            theContext.Params,
                                            theContext.Regularities);
  }
}

static BRepGraph_ReconstructionContext makeReconstructionContext(
  const BRepGraph*            theGraph,
  const BRepGraphInc_Storage& theStorage)
{
  BRepGraph_ReconstructionContext aContext;
  aContext.Graph   = theGraph;
  aContext.Storage = &theStorage;

  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph->LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph->LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  aContext.Params       = aParamLayer.get();
  aContext.Regularities = aRegularityLayer.get();
  return aContext;
}
} // namespace

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Shape(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: if entity was never mutated, return the original shape.
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef != nullptr && aDef->IsRemoved)
    return TopoDS_Shape();
  if (aDef != nullptr && aDef->SubtreeGen == 0)
  {
    const TopoDS_Shape* anOrig = FindOriginal(theNode);
    if (anOrig != nullptr)
      return *anOrig;
  }

  // Check mutable cache under shared lock with SubtreeGen validation.
  {
    std::shared_lock<std::shared_mutex> aReadLock(myGraph->myData->myCurrentShapesMutex);
    const BRepGraph_Data::CachedShape*  aCached = myGraph->myData->myCurrentShapes.Seek(theNode);
    if (aCached != nullptr && aDef != nullptr && aCached->StoredSubtreeGen == aDef->SubtreeGen)
      return aCached->Shape;
  }

  // Reconstruct from incidence storage / assembly facade.
  BRepGraph_ReconstructionContext aContext =
    makeReconstructionContext(myGraph, myGraph->myData->myIncStorage);
  TopoDS_Shape aReconstructed = reconstructShape(aContext, theNode);

  // Store under exclusive lock with double-check to avoid redundant writes
  // when multiple threads reconstruct the same parent node concurrently.
  if (!aReconstructed.IsNull() && aDef != nullptr)
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    const BRepGraph_Data::CachedShape*  aExisting = myGraph->myData->myCurrentShapes.Seek(theNode);
    if (aExisting != nullptr && aExisting->StoredSubtreeGen == aDef->SubtreeGen)
    {
      return aExisting->Shape;
    }
    BRepGraph_Data::CachedShape anEntry;
    anEntry.Shape            = aReconstructed;
    anEntry.StoredSubtreeGen = aDef->SubtreeGen;
    myGraph->myData->myCurrentShapes.Bind(theNode, anEntry);
    if (theNode.NodeKind != BRepGraph_NodeId::Kind::Product
        && theNode.NodeKind != BRepGraph_NodeId::Kind::Occurrence)
    {
      myGraph->myData->myIncStorage.BindTShapeToNode(aReconstructed.TShape().get(), theNode);
    }
  }
  return aReconstructed;
}

//=================================================================================================

bool BRepGraph::ShapesView::HasOriginal(const BRepGraph_NodeId theNode) const
{
  return FindOriginal(theNode) != nullptr;
}

//=================================================================================================

const TopoDS_Shape* BRepGraph::ShapesView::FindOriginal(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return nullptr;

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef != nullptr && aDef->IsRemoved)
    return nullptr;

  return myGraph->myData->myIncStorage.FindOriginal(theNode);
}

//=================================================================================================

const TopoDS_Shape& BRepGraph::ShapesView::OriginalOf(const BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = FindOriginal(theNode);
  if (aShape == nullptr)
    throw Standard_ProgramError("BRepGraph::ShapesView::OriginalOf() - no original shape.");
  return *aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::ShapesView::Reconstruct(const BRepGraph_NodeId theRoot) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theRoot);
  if (aDef != nullptr && aDef->IsRemoved)
    return TopoDS_Shape();
  BRepGraph_ReconstructionContext aContext =
    makeReconstructionContext(myGraph, myGraph->myData->myIncStorage);
  return reconstructShape(aContext, theRoot);
}

//=================================================================================================

//=================================================================================================

BRepGraph_NodeId BRepGraph::ShapesView::FindNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return BRepGraph_NodeId();

  const BRepGraph_NodeId* aNodeId =
    myGraph->myData->myIncStorage.FindNodeByTShape(theShape.TShape().get());
  if (aNodeId != nullptr)
  {
    const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(*aNodeId);
    if (aDef == nullptr || aDef->IsRemoved)
      return BRepGraph_NodeId();
    return *aNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraph::ShapesView::HasNode(const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
    return false;

  return FindNode(theShape).IsValid();
}
