// Created on: 1998-05-20
// Created by: Didier PIFFAULT
// Copyright (c) 1998-1999 Matra Datavision
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeWire.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib_MakeVertex.hxx>

//=======================================================================
// function : Add
// purpose  : Add the list of edges to the current wire
//=======================================================================
void BRepLib_MakeWire::Add(const NCollection_List<TopoDS_Shape>& L)
{
  myError                   = BRepLib_WireDone;
  int aLSize   = 0;
  int aRefSize = L.Size();
  if (!L.IsEmpty())
  {
    ///
    NCollection_List<NCollection_List<TopoDS_Vertex>> aGrVL;

    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aMapVE;

    CollectCoincidentVertices(L, aGrVL);

    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> anO2NV;

    CreateNewVertices(aGrVL, anO2NV);

    NCollection_List<TopoDS_Shape> aNewEList;

    CreateNewListOfEdges(L, anO2NV, aNewEList);
    ///

    TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);

    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aProcessedEdges;
    TopExp_Explorer     anExp;

    NCollection_List<TopoDS_Shape> anActEdges, aNeighEdges;

    if (myEdge.IsNull())
    {
      // take the first edge from the list and add it
      const TopoDS_Edge& aFE = TopoDS::Edge(aNewEList.First());
      Add(aFE);
      aProcessedEdges.Add(aFE);
      anActEdges.Append(aFE);
      aLSize++;
    }
    else
    {
      // existing edges are already connected
      for (anExp.Init(myShape, TopAbs_EDGE); anExp.More(); anExp.Next())
      {
        const TopoDS_Shape& aCSh = anExp.Current();
        aProcessedEdges.Add(aCSh);
        anActEdges.Append(aCSh);
      }
    }

    NCollection_List<TopoDS_Shape>::Iterator anItL1, anItL2;

    for (anItL1.Initialize(aNewEList); anItL1.More(); anItL1.Next())
      TopExp::MapShapesAndAncestors(anItL1.Value(), TopAbs_VERTEX, TopAbs_EDGE, aMapVE);

    while (!anActEdges.IsEmpty())
    {
      anItL2.Initialize(anActEdges);
      for (; anItL2.More(); anItL2.Next())
      {
        const TopoDS_Shape& aCE = anItL2.Value();
        anExp.Init(aCE, TopAbs_VERTEX);
        for (; anExp.More(); anExp.Next())
        {
          const TopoDS_Shape& aCV = anExp.Current();
          for (anItL1.Initialize(aMapVE.FindFromKey(aCV)); anItL1.More(); anItL1.Next())
          {
            const TopoDS_Shape& aNE = anItL1.Value(); // neighbor edge
            if (!aProcessedEdges.Contains(aNE))
            {
              Add(TopoDS::Edge(aNE), false);
              aNeighEdges.Append(aNE);
              aProcessedEdges.Add(aNE);
              aLSize++;
            }
          }
        }
      }
      anActEdges.Clear();
      anActEdges.Append(aNeighEdges);
    }
  }
  if (aLSize == aRefSize)
    Done();
  else
  {
    NotDone();
    myError = BRepLib_DisconnectedWire;
  }
}

//=================================================================================================

bool BRepLib_MakeWire::BRepLib_BndBoxVertexSelector::Accept(
  const int& theObj)
{
  if (theObj > myMapOfShape.Extent())
    return false;

  const TopoDS_Vertex& aV = TopoDS::Vertex(myMapOfShape(theObj));

  if (theObj == myVInd)
    return false;

  gp_Pnt aVPnt = BRep_Tool::Pnt(aV);

  double aTolV = BRep_Tool::Tolerance(aV);

  double aL    = myP.SquareDistance(aVPnt);
  double aSTol = aTolV + myTolP;
  aSTol *= aSTol;

  if (aL <= aSTol)
  {
    myResultInd.Append(theObj);
    return true;
  }

  return false;
}

//=================================================================================================

void BRepLib_MakeWire::BRepLib_BndBoxVertexSelector::SetCurrentVertex(const gp_Pnt&    theP,
                                                                      double    theTol,
                                                                      int theVInd)
{
  myP = theP;
  myVBox.Add(myP);
  myVBox.Enlarge(theTol);
  myTolP = theTol;
  myVInd = theVInd;
}

//=================================================================================================

void BRepLib_MakeWire::CollectCoincidentVertices(
  const NCollection_List<TopoDS_Shape>&                        theL,
  NCollection_List<NCollection_List<TopoDS_Vertex>>& theGrVL)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                anAllV;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aMV2EL;

  TopExp::MapShapes(myShape, TopAbs_VERTEX, anAllV);

  NCollection_List<TopoDS_Shape>::Iterator anItL(theL);
  for (; anItL.More(); anItL.Next())
    TopExp::MapShapes(anItL.Value(), TopAbs_VERTEX, anAllV);

  // aV2CV : vertex <-> its coincident vertices
  NCollection_DataMap<TopoDS_Vertex, NCollection_Map<TopoDS_Vertex>> aV2CV;
  NCollection_UBTree<int, Bnd_Box>                      aTree;
  NCollection_UBTreeFiller<int, Bnd_Box>                aTreeFiller(aTree);
  NCollection_Map<TopoDS_Vertex>                                     aNonGroupedV;

  /// add vertices from anAllV to treefiller
  for (int i = 1; i <= anAllV.Extent(); i++)
  {
    const TopoDS_Shape& aSh = anAllV(i);
    Bnd_Box             aBB;
    BRepBndLib::Add(aSh, aBB);
    aTreeFiller.Add(i, aBB);
  }

  aTreeFiller.Fill();
  BRepLib_BndBoxVertexSelector aSelector(anAllV);

  int                             aNbColl = 0;
  NCollection_List<int>::Iterator itI;
  for (int i = 1; i <= anAllV.Extent(); i++)
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(anAllV(i));
    if (myVertices.Contains(aV))
      continue;
    aSelector.SetCurrentVertex(BRep_Tool::Pnt(aV), BRep_Tool::Tolerance(aV), i);
    aNbColl = aTree.Select(aSelector);
    if (aNbColl > 0)
    {
      const NCollection_List<int>& aResInds = aSelector.GetResultInds();
      NCollection_Map<TopoDS_Vertex>* aVM = aV2CV.Bound(aV, NCollection_Map<TopoDS_Vertex>());
      for (itI.Initialize(aResInds); itI.More(); itI.Next())
      {
        const TopoDS_Vertex& aCV = TopoDS::Vertex(anAllV(itI.Value()));
        aVM->Add(aCV);
        if (myVertices.Contains(aCV))
        {
          if (aV2CV.IsBound(aCV))
            aV2CV(aCV).Add(aV);
          else
          {
            aV2CV.Bound(aCV, NCollection_Map<TopoDS_Vertex>())->Add(aV);
            aNonGroupedV.Add(aCV);
          }
        }
      }
      if (!aVM->IsEmpty())
        aNonGroupedV.Add(aV); // vertexes to be grouped; store only coincident vertices
    }
    aSelector.ClearResInds();
  }

  /// group the coincident vertices
  NCollection_Map<TopoDS_Vertex>::Iterator  itMV;
  NCollection_List<TopoDS_Vertex>           aStartV, aCurrentV, anOneGrV;
  NCollection_List<TopoDS_Vertex>::Iterator itLV;
  bool                          IsStartNewGroup = true;
  while (!aNonGroupedV.IsEmpty() || !IsStartNewGroup)
  // exit only if there are no nongrouped vertices
  // and the last group are fully are constructed
  {
    if (IsStartNewGroup)
    {
      // start list of vertices is empty => append one from aNonGroupedV
      //  and remove it from it (i.e. mark as grouped)
      itMV.Initialize(aNonGroupedV);
      const TopoDS_Vertex& aCurV = itMV.Value();
      aStartV.Append(aCurV);
      aNonGroupedV.Remove(aCurV);
    }
    itLV.Init(aStartV);
    for (; itLV.More(); itLV.Next())
    {
      const TopoDS_Vertex& aSV = itLV.Value();
      anOneGrV.Append(aSV);
      itMV.Initialize(aV2CV(aSV));
      for (; itMV.More(); itMV.Next())
      {
        const TopoDS_Vertex& aCV = itMV.Value();
        if (aNonGroupedV.Contains(aCV))
        {
          aCurrentV.Append(aCV);
          aNonGroupedV.Remove(aCV);
        }
      }
    }
    aStartV.Clear();
    aStartV.Append(aCurrentV);
    IsStartNewGroup = aStartV.IsEmpty();
    if (IsStartNewGroup && !anOneGrV.IsEmpty())
    {
      theGrVL.Append(anOneGrV);
      anOneGrV.Clear();
    }
  }
}

//=================================================================================================

void BRepLib_MakeWire::CreateNewVertices(
  const NCollection_List<NCollection_List<TopoDS_Vertex>>& theGrVL,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&                            theO2NV)
{
  // map [old vertex => new vertex]
  // note that already existing shape (i.e. the original ones)
  // shouldn't be modified on the topological level
  NCollection_List<NCollection_List<TopoDS_Vertex>>::Iterator itLLV;
  NCollection_List<TopoDS_Vertex>::Iterator                   itLV;
  BRep_Builder                                                aBB;
  itLLV.Init(theGrVL);
  for (; itLLV.More(); itLLV.Next())
  {
    TopoDS_Vertex                          aNewV;
    NCollection_List<TopoDS_Shape>         aValList;
    const NCollection_List<TopoDS_Vertex>& aVal = itLLV.Value();
    itLV.Initialize(aVal);
    double aNewTol = 0;
    gp_Pnt        aNewC;
    for (; itLV.More(); itLV.Next())
    {
      const TopoDS_Vertex& aVV = itLV.Value();
      aValList.Append(aVV);
      if (myVertices.Contains(aVV))
        aNewV = aVV;
    }
    BRepLib::BoundingVertex(aValList, aNewC, aNewTol);

    if (aNewV.IsNull())
    {
      // vertices from the original shape isn't found in this group
      // create the new vertex
      aNewV = BRepLib_MakeVertex(aNewC);
      aBB.UpdateVertex(aNewV, aNewTol);
    }
    else
      // update already existing vertex
      aBB.UpdateVertex(aNewV, gp_Pnt(aNewC), aNewTol);

    // fill the map of old->new vertices
    itLV.Initialize(aVal);
    for (; itLV.More(); itLV.Next())
    {
      const TopoDS_Vertex& aVV = itLV.Value();
      theO2NV.Bind(aVV, aNewV);
    }
  }
}

//=================================================================================================

void BRepLib_MakeWire::CreateNewListOfEdges(const NCollection_List<TopoDS_Shape>&         theL,
                                            const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theO2NV,
                                            NCollection_List<TopoDS_Shape>&               theNewEList)
{
  /// create the new list (theNewEList) from the input list L
  bool                   IsNewEdge;
  NCollection_List<TopoDS_Vertex>    aVList;
  TopExp_Explorer                    exp;
  BRep_Builder                       aBB;
  NCollection_List<TopoDS_Shape>::Iterator anItL;
  for (anItL.Initialize(theL); anItL.More(); anItL.Next())
  {
    IsNewEdge = false;
    aVList.Clear();
    const TopoDS_Edge& aCE = TopoDS::Edge(anItL.Value());
    exp.Init(aCE, TopAbs_VERTEX);
    for (; exp.More(); exp.Next())
    {
      const TopoDS_Vertex& aVal = TopoDS::Vertex(exp.Current());
      if (theO2NV.IsBound(aVal))
      {
        IsNewEdge = true;
        // append the new vertex
        aVList.Append(TopoDS::Vertex(theO2NV(aVal).Oriented(aVal.Orientation())));
      }
      else
        aVList.Append(aVal);
    }
    if (IsNewEdge)
    {
      TopoDS_Shape                              NewE = aCE.EmptyCopied();
      NCollection_List<TopoDS_Vertex>::Iterator it(aVList);
      for (; it.More(); it.Next())
        aBB.Add(NewE, it.Value());
      theNewEList.Append(TopoDS::Edge(NewE));
    }
    else
      theNewEList.Append(aCE);
  }
}
