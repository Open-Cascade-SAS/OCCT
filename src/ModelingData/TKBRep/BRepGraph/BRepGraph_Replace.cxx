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

#include <BRepGraph_Replace.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraphInc_Load.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraphSupInc_CopyContext.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
template <typename TheId>
void bindNodeKind(const BRepGraph&              theSource,
                  const BRepGraph&              theTarget,
                  const uint32_t                theCount,
                  BRepGraph_CopyRemap::ItemMap& theRemap,
                  uint32_t&                     theMappedCount)
{
  for (uint32_t anIndex = 0; anIndex < theCount; ++anIndex)
  {
    const TheId         anOldId(anIndex);
    const BRepGraph_UID aUID = theSource.UIDs().Of(anOldId);
    if (!aUID.IsValid())
    {
      continue;
    }
    const BRepGraph_NodeId aNewId = theTarget.UIDs().NodeIdFrom(aUID);
    if (aNewId.IsValid() && aNewId.NodeKind == BRepGraph_NodeId(anOldId).NodeKind)
    {
      theRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
      ++theMappedCount;
    }
  }
}

template <typename TheId>
void bindRefKind(const BRepGraph&              theSource,
                 const BRepGraph&              theTarget,
                 const uint32_t                theCount,
                 BRepGraph_CopyRemap::ItemMap& theRemap,
                 uint32_t&                     theMappedCount)
{
  for (uint32_t anIndex = 0; anIndex < theCount; ++anIndex)
  {
    const TheId            anOldId(anIndex);
    const BRepGraph_RefUID aUID = theSource.UIDs().Of(anOldId);
    if (!aUID.IsValid())
    {
      continue;
    }
    const BRepGraph_RefId aNewId = theTarget.UIDs().RefIdFrom(aUID);
    if (aNewId.IsValid() && aNewId.RefKind == BRepGraph_RefId(anOldId).RefKind)
    {
      theRemap.Bind(BRepGraph_ItemId(anOldId), BRepGraph_ItemId(aNewId));
      ++theMappedCount;
    }
  }
}

void buildDurableRemap(const BRepGraph&                 theSource,
                       const BRepGraph&                 theTarget,
                       const BRepGraphInc_Load::Counts& theCounts,
                       BRepGraph_CopyRemap::ItemMap&    theRemap,
                       BRepGraph_Replace::Result&       theResult)
{
  bindNodeKind<BRepGraph_VertexId>(theSource,
                                   theTarget,
                                   theCounts.NbVertices,
                                   theRemap,
                                   theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_EdgeId>(theSource,
                                 theTarget,
                                 theCounts.NbEdges,
                                 theRemap,
                                 theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_CoEdgeId>(theSource,
                                   theTarget,
                                   theCounts.NbCoEdges,
                                   theRemap,
                                   theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_WireId>(theSource,
                                 theTarget,
                                 theCounts.NbWires,
                                 theRemap,
                                 theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_FaceId>(theSource,
                                 theTarget,
                                 theCounts.NbFaces,
                                 theRemap,
                                 theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_ShellId>(theSource,
                                  theTarget,
                                  theCounts.NbShells,
                                  theRemap,
                                  theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_SolidId>(theSource,
                                  theTarget,
                                  theCounts.NbSolids,
                                  theRemap,
                                  theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_CompoundId>(theSource,
                                     theTarget,
                                     theCounts.NbCompounds,
                                     theRemap,
                                     theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_CompSolidId>(theSource,
                                      theTarget,
                                      theCounts.NbCompSolids,
                                      theRemap,
                                      theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_ProductId>(theSource,
                                    theTarget,
                                    theCounts.NbProducts,
                                    theRemap,
                                    theResult.NbMappedNodes);
  bindNodeKind<BRepGraph_OccurrenceId>(theSource,
                                       theTarget,
                                       theCounts.NbOccurrences,
                                       theRemap,
                                       theResult.NbMappedNodes);

  bindRefKind<BRepGraph_VertexRefId>(theSource,
                                     theTarget,
                                     theCounts.NbVertexRefs,
                                     theRemap,
                                     theResult.NbMappedRefs);
  bindRefKind<BRepGraph_WireRefId>(theSource,
                                   theTarget,
                                   theCounts.NbWireRefs,
                                   theRemap,
                                   theResult.NbMappedRefs);
  bindRefKind<BRepGraph_FaceRefId>(theSource,
                                   theTarget,
                                   theCounts.NbFaceRefs,
                                   theRemap,
                                   theResult.NbMappedRefs);
  bindRefKind<BRepGraph_ShellRefId>(theSource,
                                    theTarget,
                                    theCounts.NbShellRefs,
                                    theRemap,
                                    theResult.NbMappedRefs);
  bindRefKind<BRepGraph_SolidRefId>(theSource,
                                    theTarget,
                                    theCounts.NbSolidRefs,
                                    theRemap,
                                    theResult.NbMappedRefs);
  bindRefKind<BRepGraph_ChildRefId>(theSource,
                                    theTarget,
                                    theCounts.NbChildRefs,
                                    theRemap,
                                    theResult.NbMappedRefs);
  bindRefKind<BRepGraph_OccurrenceRefId>(theSource,
                                         theTarget,
                                         theCounts.NbOccurrenceRefs,
                                         theRemap,
                                         theResult.NbMappedRefs);
}

BRepGraph_NodeId remapNode(const BRepGraph_CopyRemap::ItemMap& theItemRemap,
                           const BRepGraph_NodeId              theNode)
{
  if (!theNode.IsValid())
  {
    return BRepGraph_NodeId();
  }
  const BRepGraph_ItemId* aMapped = theItemRemap.Seek(BRepGraph_ItemId(theNode));
  return aMapped != nullptr && aMapped->IsNode() ? aMapped->NodeId() : BRepGraph_NodeId();
}
} // namespace

//=================================================================================================

BRepGraph_Replace::Result BRepGraph_Replace::Perform(BRepGraph&  theGraph,
                                                     BRepGraph&& theReplacement)
{
  Result aResult;
  if (!theGraph.IsValid() || !theReplacement.IsValid())
  {
    return aResult;
  }
  if (theGraph.UIDs().GraphGUID() != theReplacement.UIDs().GraphGUID()
      || theGraph.UIDs().Generation() != theReplacement.UIDs().Generation())
  {
    aResult.StatusCode = Status::DifferentGraphIdentity;
    return aResult;
  }

  try
  {
    BRepGraph_CopyRemap::ItemMap anItemRemap;
    buildDurableRemap(theGraph,
                      theReplacement,
                      theGraph.incStorage().Counts(),
                      anItemRemap,
                      aResult);

    for (BRepGraph_CacheRegistry* aRegistry : theGraph.data()->myExternalCacheRegistries)
    {
      theReplacement.registerExternalCacheRegistry(aRegistry);
    }

    // Cache callbacks validate source generations and publish only entries that
    // are still valid for an unchanged durable item in the replacement graph.
    theGraph.CacheRegistry().CopyFreshCachesTo(theReplacement,
                                               anItemRemap,
                                               BRepGraph_CopyRemap::Mode::Compact,
                                               BRepGraph_CopyRemap::FreshnessPolicy::MatchTarget);

    // Prepare every fallible migration against the detached replacement. Moving
    // the completed graph rebinds its registries to the stable wrapper without
    // allocation, so publication is the only operation after this point.
    theGraph.LayerRegistry().CopyTransientLayersTo(theReplacement, anItemRemap);
    theGraph = std::move(theReplacement);
    theGraph.clearExternalCacheRegistries();
    aResult.StatusCode = Status::Done;
  }
  catch (const Standard_Failure&)
  {
    aResult.StatusCode = Status::MigrationFailed;
  }
  catch (...)
  {
    aResult.StatusCode = Status::MigrationFailed;
  }
  return aResult;
}

//=================================================================================================

BRepGraph_Replace::Result BRepGraph_Replace::PerformCompaction(
  BRepGraph&                          theGraph,
  BRepGraph&&                         theCompactedCore,
  const BRepGraph_CopyRemap::ItemMap& theItemRemap)
{
  Result aResult;
  if (!theGraph.IsValid() || !theCompactedCore.IsValid())
  {
    return aResult;
  }
  if (theGraph.UIDs().GraphGUID() != theCompactedCore.UIDs().GraphGUID()
      || theGraph.UIDs().Generation() != theCompactedCore.UIDs().Generation())
  {
    aResult.StatusCode = Status::DifferentGraphIdentity;
    return aResult;
  }

  try
  {
    NCollection_LinearVector<std::pair<TopoDS_Shape, BRepGraph_NodeId>> aShapeBindings;
    NCollection_LinearVector<std::pair<BRepGraph_NodeId, TopoDS_Shape>> anOriginalBindings;
    theGraph.incStorage().ForEachShapeBinding(
      [&](const TopoDS_Shape& theShape, const BRepGraph_NodeId theNode) {
        aShapeBindings.Append({theShape, theNode});
      });
    theGraph.incStorage().ForEachOriginalBinding(
      [&](const BRepGraph_NodeId theNode, const TopoDS_Shape& theShape) {
        anOriginalBindings.Append({theNode, theShape});
      });

    NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId> anOwnerMap;
    for (const auto& [anOldItem, aNewItem] : theItemRemap.Items())
    {
      if (anOldItem.IsNode() && aNewItem.IsNode())
      {
        anOwnerMap.Bind(anOldItem.NodeId(), aNewItem.NodeId());
        ++aResult.NbMappedNodes;
      }
      else if (anOldItem.IsReference() && aNewItem.IsReference())
      {
        ++aResult.NbMappedRefs;
      }
    }

    for (BRepGraph_CacheRegistry* aRegistry : theGraph.data()->myExternalCacheRegistries)
    {
      theCompactedCore.registerExternalCacheRegistry(aRegistry);
    }

    if (!theGraph.supplementStorage().CloneCompactedTo(theCompactedCore.supplementStorage(),
                                                       anOwnerMap))
    {
      aResult.StatusCode = Status::MigrationFailed;
      return aResult;
    }
    BRepGraphSupInc_CopyContext aSupplementCopy(theGraph.supplementStorage(),
                                                theCompactedCore.supplementStorage(),
                                                BRepGraphSupInc_CopyContext::Mode::Compact);
    for (NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIt(anOwnerMap);
         anIt.More();
         anIt.Next())
    {
      (void)aSupplementCopy.AddNodeMapping(anIt.Key(), anIt.Value());
    }

    for (const auto& [aShape, anOldNode] : aShapeBindings)
    {
      const BRepGraph_NodeId aNewNode = remapNode(theItemRemap, anOldNode);
      if (aNewNode.IsValid())
      {
        theCompactedCore.incStorage().SetDefinitionShapeBinding(aShape, aNewNode);
      }
    }
    for (const auto& [anOldNode, aShape] : anOriginalBindings)
    {
      const BRepGraph_NodeId aNewNode = remapNode(theItemRemap, anOldNode);
      if (aNewNode.IsValid())
      {
        theCompactedCore.incStorage().BindOriginal(aNewNode, aShape);
      }
    }

    theGraph.CacheRegistry().CopyFreshCachesTo(theCompactedCore,
                                               theItemRemap,
                                               BRepGraph_CopyRemap::Mode::Compact);
    theGraph.LayerRegistry().CopyLayersTo(theCompactedCore,
                                          theItemRemap,
                                          BRepGraph_CopyRemap::Mode::Compact);
    theGraph.LayerSupplementRegistry().CopyLayersTo(theCompactedCore,
                                                    theItemRemap,
                                                    BRepGraph_CopyRemap::Mode::Compact,
                                                    aSupplementCopy);

    theCompactedCore.Editor().CommitMutation();
    theGraph = std::move(theCompactedCore);
    theGraph.clearExternalCacheRegistries();
    aResult.StatusCode = Status::Done;
  }
  catch (const Standard_Failure&)
  {
    aResult.StatusCode = Status::MigrationFailed;
  }
  catch (...)
  {
    aResult.StatusCode = Status::MigrationFailed;
  }
  return aResult;
}
