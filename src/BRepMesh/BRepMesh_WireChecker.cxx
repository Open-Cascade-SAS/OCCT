// Created on: 2014-06-03
// Created by: Oleg AGASHIN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BRepMesh_WireChecker.hxx>

#include <Precision.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Iterator.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Classifier.hxx>
#include <BRepMesh_WireInterferenceChecker.hxx>
#include <OSD_Parallel.hxx>


//=======================================================================
//function : Selector::Constructor
//purpose  : 
//=======================================================================
BRepMesh_WireChecker::BndBox2dTreeSelector::BndBox2dTreeSelector(
  const Standard_Integer theReservedSize)
  : mySkippedIndex(-1),
    myIndices(0, theReservedSize - 1),
    myIndicesNb(0)
{
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_WireChecker::BndBox2dTreeSelector::Reject(
  const Bnd_Box2d& theBox2D) const
{
  return myBox2D.IsOut(theBox2D);
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_WireChecker::BndBox2dTreeSelector::Accept(
  const Standard_Integer& theIndex)
{
  if (theIndex <= mySkippedIndex)
    return Standard_False;

  myIndices(myIndicesNb++) = theIndex;
  return Standard_True;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepMesh_WireChecker::BndBox2dTreeSelector::Clear()
{
  mySkippedIndex = -1;
  myIndicesNb    = 0;
}

//=======================================================================
//function : SetBox
//purpose  : 
//=======================================================================
void BRepMesh_WireChecker::BndBox2dTreeSelector::SetBox(
  const Bnd_Box2d& theBox2D)
{
  myBox2D = theBox2D;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepMesh_WireChecker::BndBox2dTreeSelector::SetSkippedIndex(
  const Standard_Integer theIndex)
{
  mySkippedIndex = theIndex;
}

//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
const BRepMesh::Array1OfInteger& 
  BRepMesh_WireChecker::BndBox2dTreeSelector::Indices() const
{
  return myIndices;
}

//=======================================================================
//function : IndicesNb
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_WireChecker::BndBox2dTreeSelector::IndicesNb() const
{
  return myIndicesNb;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_WireChecker::BRepMesh_WireChecker(
  const TopoDS_Face&                            theFace,
  const Standard_Real                           theTolUV,
  const BRepMesh::HDMapOfShapePairOfPolygon&    theEdges,
  const BRepMesh::HIMapOfInteger&               theVertexMap,
  const Handle(BRepMesh_DataStructureOfDelaun)& theStructure,
  const Standard_Real                           theUmin,
  const Standard_Real                           theUmax,
  const Standard_Real                           theVmin,
  const Standard_Real                           theVmax,
  const Standard_Boolean                        isInParallel)
  : myTolUV(theTolUV),
    myEdges(theEdges),
    myVertexMap(theVertexMap),
    myStructure(theStructure),
    myUmin(theUmin),
    myUmax(theUmax),
    myVmin(theVmin),
    myVmax(theVmax),
    myStatus(BRepMesh_NoError),
    myIsInParallel(isInParallel)
{
  TopoDS_Face aFace = theFace;
  aFace.Orientation(TopAbs_FORWARD);

  for (TopoDS_Iterator aFaceIt(aFace); aFaceIt.More(); aFaceIt.Next())
  {
    if (aFaceIt.Value().IsNull() || aFaceIt.Value().ShapeType() != TopAbs_WIRE) // may be inner vertex
      continue;
    const TopoDS_Wire& aWire = TopoDS::Wire(aFaceIt.Value());

    myWiresEdges.Append(ListOfEdges());
    ListOfEdges& aEdges = myWiresEdges.ChangeLast();

    // Start traversing the wires
    BRepTools_WireExplorer aWireExplorer(aWire, aFace);
    for (; aWireExplorer.More(); aWireExplorer.Next())
    {
      const TopoDS_Edge& aEdge   = aWireExplorer.Current();
      TopAbs_Orientation aOrient = aEdge.Orientation();
      if (aOrient != TopAbs_FORWARD && aOrient != TopAbs_REVERSED)
        continue;

      aEdges.Append(aEdge);
    }

    if (aEdges.IsEmpty())
      myWiresEdges.Remove(myWiresEdges.Size());
  }
}

//=======================================================================
//function : ReCompute
//purpose  : 
//=======================================================================
void BRepMesh_WireChecker::ReCompute(BRepMesh::HClassifier& theClassifier)
{
  if (theClassifier.IsNull())
    return;

  theClassifier->Destroy();
  myStatus = BRepMesh_NoError;
  
  SeqOfDWires aDWires;
  if (!collectDiscretizedWires(aDWires))
    return;

  const Standard_Integer aNbWires = aDWires.Size();
  BRepMesh::Array1OfSegmentsTree aWiresBiPoints(1, aNbWires);
  fillSegmentsTree(aDWires, aWiresBiPoints);

  if (myIsInParallel && aNbWires > 1)
  {
    // Check wires in parallel threads.
    Standard_Mutex aWireMutex;
    BRepMesh_WireInterferenceChecker aIntChecker(aWiresBiPoints, &myStatus, &aWireMutex);
    OSD_Parallel::For(1, aNbWires + 1, aIntChecker);
  }
  else
  {
    BRepMesh_WireInterferenceChecker aIntChecker(aWiresBiPoints, &myStatus);
    OSD_Parallel::For(1, aNbWires + 1, aIntChecker, Standard_True);
  }

  if (myStatus == BRepMesh_SelfIntersectingWire)
    return;

  // Find holes
  SeqOfDWires::Iterator aDWiresIt(aDWires);
  for (; aDWiresIt.More(); aDWiresIt.Next())
  {
    const SeqOfPnt2d& aDWire = aDWiresIt.Value();
    theClassifier->RegisterWire(aDWire, myTolUV, myUmin, myUmax, myVmin, myVmax);
  }
}

//=======================================================================
//function : collectDiscretizedWires
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_WireChecker::collectDiscretizedWires(
  SeqOfDWires& theDWires)
{
  SeqOfWireEdges::Iterator aWireIt(myWiresEdges);
  for(; aWireIt.More(); aWireIt.Next())
  {
    const ListOfEdges& aEdges = aWireIt.Value();
    // For each wire we create a data map, linking vertices (only
    // the ends of edges) with their positions in the sequence of
    // all 2d points from this wire.
    // When we meet some vertex for the second time - the piece
    // of sequence is treated for a HOLE and quits the sequence.
    // Actually, we must unbind the vertices belonging to the
    // loop from the map, but since they can't appear twice on the
    // valid wire, leave them for a little speed up.

    SeqOfPnt2d                    aSeqPnt2d;
    BRepMesh::MapOfIntegerInteger aNodeInSeq;
    Standard_Integer aFirstIndex = 0, aLastIndex = 0;

    // Start traversing the wire
    ListOfEdges::Iterator aEdgeIt(aEdges);
    for (; aEdgeIt.More(); aEdgeIt.Next())
    {
      const TopoDS_Edge& aEdge   = aEdgeIt.Value();
      TopAbs_Orientation aOrient = aEdge.Orientation();
      if (!myEdges->IsBound(aEdge))
        continue;

      // Retrieve polygon
      // Define the direction for adding points to aSeqPnt2d
      Standard_Integer aStartId, aEndId, aIncrement;
      const BRepMesh_PairOfPolygon& aPair = myEdges->Find(aEdge);
      Handle(Poly_PolygonOnTriangulation) aNOD;
      if (aOrient == TopAbs_FORWARD)
      {
        aNOD       = aPair.First();
        aStartId   = 1;
        aEndId     = aNOD->NbNodes();
        aIncrement = 1;
      }
      else
      {
        aNOD       = aPair.Last();
        aStartId   = aNOD->NbNodes();
        aEndId     = 1;
        aIncrement = -1;
      }

      const TColStd_Array1OfInteger& aIndices = aNOD->Nodes();
      const Standard_Integer aFirstVertexId = myVertexMap->FindKey(aIndices(aStartId));
      const Standard_Integer aLastVertexId  = myVertexMap->FindKey(aIndices(aEndId)  );

      if (aFirstVertexId == aLastVertexId && (aEndId - aStartId) == aIncrement)
      {
        // case of continuous set of degenerated edges
        aLastIndex = aLastVertexId;
        continue;
      }

      if (aFirstIndex != 0)
      {
        if (aFirstVertexId != aLastIndex)
        {
          // there's a gap between edges
          myStatus = BRepMesh_OpenWire;
          return Standard_False;
        }
      }
      else
        aFirstIndex = aFirstVertexId;

      aLastIndex = aLastVertexId;

      // Record first vertex (to detect loops)
      aNodeInSeq.Bind(aFirstVertexId, (aSeqPnt2d.Length() + 1));

      // Add vertices in sequence
      for (Standard_Integer i = aStartId; i != aEndId; i += aIncrement)
      {
        Standard_Integer aIndex = ((i == aStartId) ? 
          aFirstVertexId : 
          myVertexMap->FindKey(aIndices(i)));

        aSeqPnt2d.Append(gp_Pnt2d(myStructure->GetNode(aIndex).Coord()));
      }

      // Now, is there a loop?
      if (aNodeInSeq.IsBound(aLastVertexId))
      {
        // Yes, treat it separately as a hole
        // Divide points into main wire and a loop
        const Standard_Integer aIdxWireStart = aNodeInSeq(aLastVertexId);
        if(aIdxWireStart < aSeqPnt2d.Length())
        {
          theDWires.Append(SeqOfPnt2d());
          SeqOfPnt2d& aWire = theDWires.ChangeLast();
          aSeqPnt2d.Split(aIdxWireStart, aWire);
        }
      }
    }

    if (aFirstIndex == 0)
      continue;

    // Isn't wire open?
    if (aFirstIndex != aLastIndex || aSeqPnt2d.Length() > 1)
    {
      myStatus = BRepMesh_OpenWire;
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : fillSegmentsTree
//purpose  :
//=======================================================================
void BRepMesh_WireChecker::fillSegmentsTree(
  const SeqOfDWires&              theDWires,
  BRepMesh::Array1OfSegmentsTree& theWiresSegmentsTree)
{
  const Standard_Integer aNbWires = theDWires.Size();
  for (Standard_Integer aWireIt = 1; aWireIt <= aNbWires; ++aWireIt)
  {
    const SeqOfPnt2d&      aWire    = theDWires(aWireIt);
    const Standard_Integer aWireLen = aWire.Size();

    BRepMesh::HArray1OfSegments  aWireSegments = 
      new BRepMesh::Array1OfSegments(1, aWireLen);

    BRepMesh::HBndBox2dTree      aBndBoxTree   = 
      new BRepMesh::BndBox2dTree;

    BRepMesh::BndBox2dTreeFiller aBndBoxTreeFiller(*aBndBoxTree);

    Standard_Real x1 = 0., y1 = 0., aXstart = 0., aYstart = 0.;
    for (Standard_Integer aPntIt = 0; aPntIt <= aWireLen; ++aPntIt)
    {
      Standard_Real x2, y2;
      // Obtain last point of the segment
      if (aPntIt == aWireLen)
      {
        x2 = aXstart;
        y2 = aYstart;
      }
      else
      {
        const gp_Pnt2d& aPnt = aWire(aPntIt + 1);
        x2 = aPnt.X();
        y2 = aPnt.Y();
      }

      // Build segment (bi-point)
      if (aPntIt == 0)
      {
        aXstart = x2;
        aYstart = y2;
      }
      else
      {
        gp_Pnt2d aStartPnt(x1, y1);
        gp_Pnt2d   aEndPnt(x2, y2);
       
        BRepMesh::Segment& aSegment = aWireSegments->ChangeValue(aPntIt);
        aSegment.StartPnt = aStartPnt.XY();
        aSegment.EndPnt   = aEndPnt.XY();

        Bnd_Box2d aBox;
        aBox.Add(aStartPnt);
        aBox.Add(  aEndPnt);
        aBndBoxTreeFiller.Add(aPntIt, aBox);
      }
      x1 = x2;
      y1 = y2;
    }
    aBndBoxTreeFiller.Fill();

    BRepMesh::SegmentsTree& aSegmentsTree = theWiresSegmentsTree(aWireIt);
    aSegmentsTree.first  = aWireSegments;
    aSegmentsTree.second = aBndBoxTree;
  }
}
