// Copyright (c) 1999 Matra Datavision
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

#include <ShapeFix_EdgeConnect.hxx>

#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace
{
//=================================================================================================

struct EdgeEndUsage
{
  TopoDS_Vertex First;
  TopoDS_Vertex Last;
  bool          UsesFirst = false;
  bool          UsesLast  = false;
};

//=================================================================================================

EdgeEndUsage edgeEndUsage(const TopoDS_Edge& theEdge, const TopoDS_Vertex& theVertex)
{
  TopoDS_Edge aForwardEdge = theEdge;
  aForwardEdge.Orientation(TopAbs_FORWARD);

  EdgeEndUsage aUsage;
  TopExp::Vertices(aForwardEdge, aUsage.First, aUsage.Last);
  aUsage.UsesFirst = theVertex.IsSame(aUsage.First);
  aUsage.UsesLast  = theVertex.IsSame(aUsage.Last);
  return aUsage;
}

//=================================================================================================

void appendCurveEndPositions(const TopoDS_Edge&                theEdge,
                             const EdgeEndUsage&               theUsage,
                             NCollection_LinearVector<gp_XYZ>& thePositions)
{
  const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(theEdge.TShape());
  if (aTEdge.IsNull())
  {
    return;
  }

  for (const occ::handle<BRep_CurveRepresentation>& aRepresentation : aTEdge->Curves())
  {
    const occ::handle<BRep_GCurve> aCurve = occ::down_cast<BRep_GCurve>(aRepresentation);
    if (aCurve.IsNull())
    {
      continue;
    }

    double aFirstParameter = 0.0;
    double aLastParameter  = 0.0;
    aCurve->Range(aFirstParameter, aLastParameter);
    gp_Pnt aPoint;
    if (theUsage.UsesFirst)
    {
      aCurve->D0(aFirstParameter, aPoint);
      thePositions.Append(aPoint.XYZ());
    }
    if (theUsage.UsesLast)
    {
      aCurve->D0(aLastParameter, aPoint);
      thePositions.Append(aPoint.XYZ());
    }
  }
}

//=================================================================================================

gp_XYZ positionAndTolerance(const NCollection_LinearVector<gp_XYZ>& thePositions,
                            double&                                 theTolerance)
{
  gp_XYZ aPosition(0.0, 0.0, 0.0);
  gp_XYZ aLowerBound(0.0, 0.0, 0.0);
  gp_XYZ anUpperBound(0.0, 0.0, 0.0);

  for (size_t anIndex = 0; anIndex < thePositions.Size(); ++anIndex)
  {
    const gp_XYZ& aCurrent = thePositions.Value(anIndex);
    if (anIndex == 0)
    {
      aLowerBound  = aCurrent;
      anUpperBound = aCurrent;
    }
    else
    {
      aLowerBound.SetX(std::min(aLowerBound.X(), aCurrent.X()));
      aLowerBound.SetY(std::min(aLowerBound.Y(), aCurrent.Y()));
      aLowerBound.SetZ(std::min(aLowerBound.Z(), aCurrent.Z()));
      anUpperBound.SetX(std::max(anUpperBound.X(), aCurrent.X()));
      anUpperBound.SetY(std::max(anUpperBound.Y(), aCurrent.Y()));
      anUpperBound.SetZ(std::max(anUpperBound.Z(), aCurrent.Z()));
    }
    aPosition = aCurrent;
  }

  if (thePositions.Size() > 1)
  {
    aPosition = (aLowerBound + anUpperBound) * 0.5;
  }

  double aMaximumDeviation = 0.0;
  for (size_t anIndex = 0; anIndex < thePositions.Size(); ++anIndex)
  {
    aMaximumDeviation =
      std::max(aMaximumDeviation, (aPosition - thePositions.Value(anIndex)).Modulus());
  }
  theTolerance = std::max(aMaximumDeviation * 1.0001, Precision::Confusion());
  return aPosition;
}

//=================================================================================================

void replaceVertex(BRep_Builder&        theBuilder,
                   TopoDS_Edge&         theEdge,
                   const TopoDS_Vertex& theVertex,
                   const TopoDS_Vertex& theSharedVertex)
{
  // BRep_Builder modifies the oriented vertex list of this stored edge copy.
  // Keep the legacy forward normalization before removing or adding vertices.
  theEdge.Orientation(TopAbs_FORWARD);
  const EdgeEndUsage       aUsage          = edgeEndUsage(theEdge, theVertex);
  const TopoDS_Vertex      anOldVertex     = aUsage.UsesFirst ? aUsage.First : aUsage.Last;
  const TopAbs_Orientation aNewOrientation = aUsage.UsesFirst ? TopAbs_FORWARD : TopAbs_REVERSED;
  const TopoDS_Vertex      aNewVertex = TopoDS::Vertex(theSharedVertex.Oriented(aNewOrientation));

  if (anOldVertex.IsSame(aNewVertex))
  {
    return;
  }

  const bool wasFree = theEdge.Free();
  theEdge.Free(true);
  theBuilder.Remove(theEdge, anOldVertex);
  theBuilder.Add(theEdge, aNewVertex);
  if (aUsage.UsesFirst && aUsage.UsesLast)
  {
    theBuilder.Remove(theEdge, anOldVertex.Oriented(TopAbs_REVERSED));
    theBuilder.Add(theEdge, aNewVertex.Oriented(TopAbs_REVERSED));
  }
  theEdge.Free(wasFree);
}
} // namespace

//=================================================================================================

ShapeFix_EdgeConnect::ShapeFix_EdgeConnect() = default;

//=================================================================================================

void ShapeFix_EdgeConnect::Add(const TopoDS_Edge& theFirstEdge, const TopoDS_Edge& theSecondEdge)
{
  const TopoDS_Vertex aFirstVertex  = TopExp::LastVertex(theFirstEdge, true);
  const TopoDS_Vertex aSecondVertex = TopExp::FirstVertex(theSecondEdge, true);
  const TopoDS_Shape* aFirstShared  = myVertices.Seek(aFirstVertex);
  const TopoDS_Shape* aSecondShared = myVertices.Seek(aSecondVertex);

  if (aFirstShared == nullptr && aSecondShared == nullptr)
  {
    myVertices.TryBind(aFirstVertex, aFirstVertex);
    myVertices.TryBind(aSecondVertex, aFirstVertex);
    NCollection_List<TopoDS_Shape> aNewList;
    aNewList.Append(aFirstVertex);
    aNewList.Append(theFirstEdge);
    aNewList.Append(aSecondVertex);
    aNewList.Append(theSecondEdge);
    myLists.TryBind(aFirstVertex, std::move(aNewList));
    return;
  }

  if (aFirstShared != nullptr && aSecondShared == nullptr)
  {
    const TopoDS_Shape aSharedVertex = *aFirstShared;
    myVertices.TryBind(aSecondVertex, aSharedVertex);
    NCollection_List<TopoDS_Shape>& aList = myLists(aSharedVertex);
    aList.Append(aSecondVertex);
    aList.Append(theSecondEdge);
    return;
  }

  if (aFirstShared == nullptr)
  {
    const TopoDS_Shape aSharedVertex = *aSecondShared;
    myVertices.TryBind(aFirstVertex, aSharedVertex);
    NCollection_List<TopoDS_Shape>& aList = myLists(aSharedVertex);
    aList.Append(aFirstVertex);
    aList.Append(theFirstEdge);
    return;
  }

  const TopoDS_Shape aFirstSharedVertex  = *aFirstShared;
  const TopoDS_Shape aSecondSharedVertex = *aSecondShared;
  if (aFirstSharedVertex.IsSame(aSecondSharedVertex))
  {
    return;
  }

  NCollection_List<TopoDS_Shape>& aFirstList  = myLists(aFirstSharedVertex);
  NCollection_List<TopoDS_Shape>& aSecondList = myLists(aSecondSharedVertex);
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aSecondList); anIterator.More();)
  {
    myVertices(anIterator.Value()) = aFirstSharedVertex;
    anIterator.Next(); // edge paired with the vertex
    anIterator.Next();
  }
  aFirstList.Append(aSecondList);
  myLists.UnBind(aSecondSharedVertex);
}

//=================================================================================================

void ShapeFix_EdgeConnect::Add(const TopoDS_Shape& theShape)
{
  for (TopExp_Explorer aWireExplorer(theShape, TopAbs_WIRE); aWireExplorer.More();
       aWireExplorer.Next())
  {
    const TopoDS_Wire aWire = TopoDS::Wire(aWireExplorer.Current());
    TopExp_Explorer   anEdgeExplorer(aWire, TopAbs_EDGE);
    if (!anEdgeExplorer.More())
    {
      continue;
    }

    TopoDS_Edge       aPreviousEdge = TopoDS::Edge(anEdgeExplorer.Current());
    const TopoDS_Edge aFirstEdge    = aPreviousEdge;
    for (anEdgeExplorer.Next(); anEdgeExplorer.More(); anEdgeExplorer.Next())
    {
      const TopoDS_Edge aCurrentEdge = TopoDS::Edge(anEdgeExplorer.Current());
      Add(aPreviousEdge, aCurrentEdge);
      aPreviousEdge = aCurrentEdge;
    }
    if (aWire.Closed())
    {
      Add(aPreviousEdge, aFirstEdge);
    }
  }
}

//=================================================================================================

void ShapeFix_EdgeConnect::Build()
{
  NCollection_LinearVector<gp_XYZ> aPositions;
  BRep_Builder                     aBuilder;

  for (auto [aSharedShape, aList] : myLists.Items())
  {
    TopoDS_Vertex aSharedVertex = TopoDS::Vertex(aSharedShape);
    aPositions.Clear();

    for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aList); anIterator.More();)
    {
      const TopoDS_Vertex aVertex = TopoDS::Vertex(anIterator.Value());
      anIterator.Next();
      const TopoDS_Edge& anEdge = TopoDS::Edge(anIterator.Value());
      anIterator.Next();
      appendCurveEndPositions(anEdge, edgeEndUsage(anEdge, aVertex), aPositions);
    }

    double       aTolerance = 0.0;
    const gp_XYZ aPosition  = positionAndTolerance(aPositions, aTolerance);
    aBuilder.UpdateVertex(aSharedVertex, gp_Pnt(aPosition), aTolerance);

    for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aList); anIterator.More();)
    {
      const TopoDS_Vertex aVertex = TopoDS::Vertex(anIterator.Value());
      anIterator.Next();
      TopoDS_Edge& anEdge = TopoDS::Edge(anIterator.ChangeValue());
      anIterator.Next();
      replaceVertex(aBuilder, anEdge, aVertex, aSharedVertex);
    }
  }

  Clear();
}

//=================================================================================================

void ShapeFix_EdgeConnect::Clear()
{
  myVertices.Clear();
  myLists.Clear();
}
