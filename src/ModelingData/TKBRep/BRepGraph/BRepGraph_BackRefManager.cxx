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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>

//=================================================================================================

void BRepGraph_BackRefManager::BindFaceToSurface(BRepGraph&       theGraph,
                                                  BRepGraph_NodeId theFaceDefId,
                                                  int              theSurfIdx)
{
  theGraph.myData->mySurfaces.Nodes.ChangeValue(theSurfIdx).FaceDefUsers.Append(theFaceDefId);
}

//=================================================================================================

void BRepGraph_BackRefManager::UnbindFaceFromSurface(BRepGraph&       theGraph,
                                                      BRepGraph_NodeId theFaceDefId,
                                                      int              theSurfIdx)
{
  NCollection_Vector<BRepGraph_NodeId>& anUsers =
    theGraph.myData->mySurfaces.Nodes.ChangeValue(theSurfIdx).FaceDefUsers;
  for (int anIdx = anUsers.Length() - 1; anIdx >= 0; --anIdx)
  {
    if (anUsers.Value(anIdx) == theFaceDefId)
    {
      if (anIdx < anUsers.Length() - 1)
        anUsers.ChangeValue(anIdx) = anUsers.Value(anUsers.Length() - 1);
      anUsers.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraph_BackRefManager::RewriteFaceSurface(BRepGraph&       theGraph,
                                                   BRepGraph_NodeId theFaceDefId,
                                                   int              theOldSurfIdx,
                                                   int              theNewSurfIdx)
{
  UnbindFaceFromSurface(theGraph, theFaceDefId, theOldSurfIdx);
  BindFaceToSurface(theGraph, theFaceDefId, theNewSurfIdx);
}

//=================================================================================================

void BRepGraph_BackRefManager::BindEdgeToCurve(BRepGraph&       theGraph,
                                                BRepGraph_NodeId theEdgeDefId,
                                                int              theCurveIdx)
{
  theGraph.myData->myCurves.Nodes.ChangeValue(theCurveIdx).EdgeDefUsers.Append(theEdgeDefId);
}

//=================================================================================================

void BRepGraph_BackRefManager::UnbindEdgeFromCurve(BRepGraph&       theGraph,
                                                    BRepGraph_NodeId theEdgeDefId,
                                                    int              theCurveIdx)
{
  NCollection_Vector<BRepGraph_NodeId>& anUsers =
    theGraph.myData->myCurves.Nodes.ChangeValue(theCurveIdx).EdgeDefUsers;
  for (int anIdx = anUsers.Length() - 1; anIdx >= 0; --anIdx)
  {
    if (anUsers.Value(anIdx) == theEdgeDefId)
    {
      if (anIdx < anUsers.Length() - 1)
        anUsers.ChangeValue(anIdx) = anUsers.Value(anUsers.Length() - 1);
      anUsers.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraph_BackRefManager::RewriteEdgeCurve(BRepGraph&       theGraph,
                                                 BRepGraph_NodeId theEdgeDefId,
                                                 int              theOldCurveIdx,
                                                 int              theNewCurveIdx)
{
  UnbindEdgeFromCurve(theGraph, theEdgeDefId, theOldCurveIdx);
  BindEdgeToCurve(theGraph, theEdgeDefId, theNewCurveIdx);
}

//=================================================================================================

void BRepGraph_BackRefManager::BindEdgeToWire(BRepGraph& theGraph,
                                               int        theEdgeDefIdx,
                                               int        theWireDefIdx)
{
  theGraph.myData->myEdgeToWires.TryBind(theEdgeDefIdx, NCollection_Vector<int>());
  theGraph.myData->myEdgeToWires.ChangeFind(theEdgeDefIdx).Append(theWireDefIdx);
}

//=================================================================================================

void BRepGraph_BackRefManager::UnbindEdgeFromWire(BRepGraph& theGraph,
                                                   int        theEdgeDefIdx,
                                                   int        theWireDefIdx)
{
  NCollection_Vector<int>* aWiresPtr = theGraph.myData->myEdgeToWires.ChangeSeek(theEdgeDefIdx);
  if (aWiresPtr == nullptr)
    return;

  NCollection_Vector<int>& aWires = *aWiresPtr;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    if (aWires.Value(aWIdx) == theWireDefIdx)
    {
      if (aWIdx < aWires.Length() - 1)
        aWires.ChangeValue(aWIdx) = aWires.Value(aWires.Length() - 1);
      aWires.EraseLast();
      break;
    }
  }
}

//=================================================================================================

void BRepGraph_BackRefManager::ReplaceEdgeInWireMap(BRepGraph& theGraph,
                                                     int        theOldEdgeIdx,
                                                     int        theNewEdgeIdx,
                                                     int        theWireDefIdx)
{
  UnbindEdgeFromWire(theGraph, theOldEdgeIdx, theWireDefIdx);
  BindEdgeToWire(theGraph, theNewEdgeIdx, theWireDefIdx);
}

//=================================================================================================

int BRepGraph_BackRefManager::AddRelEdge(BRepGraph&          theGraph,
                                         BRepGraph_NodeId    theFrom,
                                         BRepGraph_NodeId    theTo,
                                         BRepGraph_RelEdge::Kind theKind)
{
  BRepGraph_RelEdge anEdge;
  anEdge.RelKind = theKind;
  anEdge.Source  = theFrom;
  anEdge.Target  = theTo;

  theGraph.myData->myOutRelEdges.TryBind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  theGraph.myData->myInRelEdges.TryBind(theTo, NCollection_Vector<BRepGraph_RelEdge>());

  NCollection_Vector<BRepGraph_RelEdge>& anOutVec =
    theGraph.myData->myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  theGraph.myData->myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph_BackRefManager::RemoveRelEdges(BRepGraph&          theGraph,
                                              BRepGraph_NodeId    theFrom,
                                              BRepGraph_NodeId    theTo,
                                              BRepGraph_RelEdge::Kind theKind)
{
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec =
    theGraph.myData->myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Target == theTo)
      {
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  NCollection_Vector<BRepGraph_RelEdge>* anInVec =
    theGraph.myData->myInRelEdges.ChangeSeek(theTo);
  if (anInVec != nullptr)
  {
    for (int anIdx = anInVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anInVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Source == theFrom)
      {
        if (anIdx < anInVec->Length() - 1)
          anInVec->ChangeValue(anIdx) = anInVec->Value(anInVec->Length() - 1);
        anInVec->EraseLast();
      }
    }
  }
}

//=================================================================================================

void BRepGraph_BackRefManager::ClearRelEdges(BRepGraph&       theGraph,
                                             BRepGraph_NodeId theNode)
{
  theGraph.myData->myOutRelEdges.UnBind(theNode);
  theGraph.myData->myInRelEdges.UnBind(theNode);
}

//=================================================================================================

void BRepGraph_BackRefManager::ClearAll(BRepGraph& theGraph)
{
  // Clear geometry back-refs.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.myData->mySurfaces.Nodes.Length(); ++aSurfIdx)
    theGraph.myData->mySurfaces.Nodes.ChangeValue(aSurfIdx).FaceDefUsers.Clear();

  for (int aCurveIdx = 0; aCurveIdx < theGraph.myData->myCurves.Nodes.Length(); ++aCurveIdx)
    theGraph.myData->myCurves.Nodes.ChangeValue(aCurveIdx).EdgeDefUsers.Clear();

  // Clear edge-to-wire reverse index.
  theGraph.myData->myEdgeToWires.Clear();

  // Clear relation edge maps.
  theGraph.myData->myOutRelEdges.Clear();
  theGraph.myData->myInRelEdges.Clear();
}

//=================================================================================================

void BRepGraph_BackRefManager::RebuildAll(BRepGraph& theGraph)
{
  // Clear all existing back-references.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.myData->mySurfaces.Nodes.Length(); ++aSurfIdx)
    theGraph.myData->mySurfaces.Nodes.ChangeValue(aSurfIdx).FaceDefUsers.Clear();

  for (int aCurveIdx = 0; aCurveIdx < theGraph.myData->myCurves.Nodes.Length(); ++aCurveIdx)
    theGraph.myData->myCurves.Nodes.ChangeValue(aCurveIdx).EdgeDefUsers.Clear();

  theGraph.myData->myEdgeToWires.Clear();

  // Rebuild surface back-refs from FaceDefs.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.myData->myFaces.Defs.Length(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.myData->myFaces.Defs.Value(aFaceIdx);
    if (aFaceDef.SurfNodeId.IsValid())
      theGraph.myData->mySurfaces.Nodes.ChangeValue(aFaceDef.SurfNodeId.Index)
        .FaceDefUsers.Append(aFaceDef.Id);
  }

  // Rebuild curve back-refs from EdgeDefs.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myEdges.Defs.Length(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myEdges.Defs.Value(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
      theGraph.myData->myCurves.Nodes.ChangeValue(anEdgeDef.CurveNodeId.Index)
        .EdgeDefUsers.Append(anEdgeDef.Id);
  }

  // Rebuild edge-to-wire map from WireDefs.
  for (int aWireIdx = 0; aWireIdx < theGraph.myData->myWires.Defs.Length(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWires.Defs.Value(aWireIdx);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index;
      theGraph.myData->myEdgeToWires.TryBind(anEdgeDefIdx, NCollection_Vector<int>());
      theGraph.myData->myEdgeToWires.ChangeFind(anEdgeDefIdx).Append(aWireIdx);
    }
  }
}
