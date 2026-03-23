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

#include <BRepGraph_AttrsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_NodeId.hxx>

#include <NCollection_Vector.hxx>

namespace
{

//! Check if a NodeId refers to a geometry node (Surface, Curve, PCurve).
//! Geometry nodes have no Cache and thus no user attributes.
bool isGeometryKind(BRepGraph_NodeId::Kind theKind)
{
  return theKind == BRepGraph_NodeId::Kind::Surface
      || theKind == BRepGraph_NodeId::Kind::Curve
      || theKind == BRepGraph_NodeId::Kind::PCurve;
}

} // namespace

//=================================================================================================

BRepGraphAlgo_AttrTransfer::Result BRepGraphAlgo_AttrTransfer::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_AttrTransfer::Result BRepGraphAlgo_AttrTransfer::Perform(
  BRepGraph&     theGraph,
  const Options& theOptions)
{
  Result aResult;

  const BRepGraph_History& aHistory = theGraph.History();
  const int aNbRecords = aHistory.NbRecords();
  if (aNbRecords == 0)
    return aResult;

  for (int aRecIdx = 0; aRecIdx < aNbRecords; ++aRecIdx)
  {
    const BRepGraph_HistoryRecord& aRecord = aHistory.Record(aRecIdx);

    for (NCollection_DataMap<BRepGraph_NodeId,
                             NCollection_Vector<BRepGraph_NodeId>>::Iterator aMapIter(aRecord.Mapping);
         aMapIter.More(); aMapIter.Next())
    {
      const BRepGraph_NodeId& anOriginal = aMapIter.Key();

      // Skip geometry nodes -- they have no Cache / user attributes.
      if (isGeometryKind(anOriginal.NodeKind))
        continue;

      const NCollection_Vector<BRepGraph_NodeId>& aReplacements = aMapIter.Value();
      if (aReplacements.IsEmpty())
        continue;

      // Get attribute keys on the original node.
      const NCollection_Vector<int> aKeys = theGraph.Attrs().AttributeKeys(anOriginal);
      if (aKeys.IsEmpty())
        continue;

      // Copy each attribute to each replacement node.
      for (int aReplIdx = 0; aReplIdx < aReplacements.Length(); ++aReplIdx)
      {
        const BRepGraph_NodeId& aTarget = aReplacements.Value(aReplIdx);

        // Skip geometry replacement nodes and self-references.
        if (isGeometryKind(aTarget.NodeKind) || aTarget == anOriginal)
          continue;

        for (int aKeyIdx = 0; aKeyIdx < aKeys.Length(); ++aKeyIdx)
        {
          const int aKey = aKeys.Value(aKeyIdx);

          // Check if target already has this attribute.
          if (!theOptions.OverwriteExisting)
          {
            BRepGraph_UserAttrPtr anExisting = theGraph.Attrs().Get(aTarget, aKey);
            if (anExisting != nullptr)
            {
              ++aResult.NbSkipped;
              continue;
            }
          }

          BRepGraph_UserAttrPtr anAttr = theGraph.Attrs().Get(anOriginal, aKey);
          if (anAttr != nullptr)
          {
            theGraph.Attrs().Set(aTarget, aKey, anAttr);
            ++aResult.NbTransferred;
          }
        }
      }
    }
  }

  return aResult;
}
