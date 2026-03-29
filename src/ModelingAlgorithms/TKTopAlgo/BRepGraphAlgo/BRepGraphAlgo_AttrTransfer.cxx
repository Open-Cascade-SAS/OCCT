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

#include <BRepGraphAlgo_AttrTransfer.hxx>

#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>

#include <NCollection_Vector.hxx>

//=================================================================================================

BRepGraphAlgo_AttrTransfer::Result BRepGraphAlgo_AttrTransfer::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_AttrTransfer::Result BRepGraphAlgo_AttrTransfer::Perform(BRepGraph&     theGraph,
                                                                       const Options& theOptions)
{
  Result aResult;

  const BRepGraph_History& aHistory   = theGraph.History();
  const int                aNbRecords = aHistory.NbRecords();
  if (aNbRecords == 0)
    return aResult;

  for (int aRecIdx = 0; aRecIdx < aNbRecords; ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRecord = aHistory.Record(aRecIdx);

    for (NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>>::Iterator
           aMapIter(aRecord.Mapping);
         aMapIter.More();
         aMapIter.Next())
    {
      const BRepGraph_NodeId& anOriginal = aMapIter.Key();

      const NCollection_Vector<BRepGraph_NodeId>& aReplacements = aMapIter.Value();
      if (aReplacements.IsEmpty())
        continue;

      // Get attribute kinds on the original node.
      const NCollection_Vector<occ::handle<BRepGraph_CacheKind>> aKinds =
        theGraph.Cache().CacheKinds(anOriginal);
      if (aKinds.IsEmpty())
        continue;

      // Copy each attribute to each replacement node.
      for (int aReplIdx = 0; aReplIdx < aReplacements.Length(); ++aReplIdx)
      {
        const BRepGraph_NodeId& aTarget = aReplacements.Value(aReplIdx);

        // Skip self-references.
        if (aTarget == anOriginal)
          continue;

        for (int aKindIdx = 0; aKindIdx < aKinds.Length(); ++aKindIdx)
        {
          const occ::handle<BRepGraph_CacheKind>& aKind = aKinds.Value(aKindIdx);

          // Check if target already has this attribute.
          if (!theOptions.OverwriteExisting)
          {
            occ::handle<BRepGraph_CacheValue> anExisting = theGraph.Cache().Get(aTarget, aKind);
            if (!anExisting.IsNull())
            {
              ++aResult.NbSkipped;
              continue;
            }
          }

          occ::handle<BRepGraph_CacheValue> anAttr = theGraph.Cache().Get(anOriginal, aKind);
          if (!anAttr.IsNull())
          {
            theGraph.Cache().Set(aTarget, aKind, anAttr);
            ++aResult.NbTransferred;
          }
        }
      }
    }
  }

  return aResult;
}
