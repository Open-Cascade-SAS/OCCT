// Created on: 2014-10-14
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <StdPrs_WFShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <OSD_Parallel.hxx>
#include <StdPrs_Isolines.hxx>
#include <StdPrs_ShapeTool.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <mutex>

//! Functor for executing StdPrs_Isolines in parallel threads.
class StdPrs_WFShape_IsoFunctor
{
public:
  StdPrs_WFShape_IsoFunctor(
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePolylinesU,
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePolylinesV,
    const std::vector<TopoDS_Face>&                               theFaces,
    const occ::handle<Prs3d_Drawer>&                              theDrawer,
    double                                                        theShapeDeflection)
      : myPolylinesU(thePolylinesU),
        myPolylinesV(thePolylinesV),
        myFaces(theFaces),
        myDrawer(theDrawer),
        myShapeDeflection(theShapeDeflection)
  {
    //
  }

  void operator()(const int& theIndex) const
  {
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> aPolylinesU, aPolylinesV;
    const TopoDS_Face&                                           aFace = myFaces[theIndex];
    StdPrs_Isolines::Add(aFace, myDrawer, myShapeDeflection, aPolylinesU, aPolylinesV);
    {
      std::lock_guard<std::mutex> aLock(myMutex);
      myPolylinesU.Append(aPolylinesU);
      myPolylinesV.Append(aPolylinesV);
    }
  }

private:
  StdPrs_WFShape_IsoFunctor operator=(StdPrs_WFShape_IsoFunctor&) = delete;

private:
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& myPolylinesU;
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& myPolylinesV;
  const std::vector<TopoDS_Face>&                               myFaces;
  const occ::handle<Prs3d_Drawer>&                              myDrawer;
  mutable std::mutex                                            myMutex;
  const double                                                  myShapeDeflection;
};

//=================================================================================================

void StdPrs_WFShape::Add(const occ::handle<Prs3d_Presentation>& thePresentation,
                         const TopoDS_Shape&                    theShape,
                         const occ::handle<Prs3d_Drawer>&       theDrawer,
                         bool                                   theIsParallel)
{
  if (theShape.IsNull())
  {
    return;
  }

  if (theDrawer->IsAutoTriangulation())
  {
    StdPrs_ToolTriangulatedShape::Tessellate(theShape, theDrawer);
  }

  // draw triangulation-only edges
  if (occ::handle<Graphic3d_ArrayOfPrimitives> aTriFreeEdges =
        AddEdgesOnTriangulation(theShape, true))
  {
    occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
    aGroup->SetPrimitivesAspect(theDrawer->FreeBoundaryAspect()->Aspect());
    aGroup->AddPrimitiveArray(aTriFreeEdges);
  }

  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> aCommonPolylines;
  const occ::handle<Prs3d_LineAspect>& aWireAspect = theDrawer->WireAspect();
  const double aShapeDeflection = StdPrs_ToolTriangulatedShape::GetDeflection(theShape, theDrawer);

  // Draw isolines
  {
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>  aUPolylines, aVPolylines;
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* aUPolylinesPtr = &aUPolylines;
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* aVPolylinesPtr = &aVPolylines;

    const occ::handle<Prs3d_LineAspect>& anIsoAspectU = theDrawer->UIsoAspect();
    const occ::handle<Prs3d_LineAspect>& anIsoAspectV = theDrawer->VIsoAspect();
    if (anIsoAspectV->Aspect()->IsEqual(*anIsoAspectU->Aspect()))
    {
      aVPolylinesPtr = aUPolylinesPtr; // put both U and V isolines into single group
    }
    if (anIsoAspectU->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      aUPolylinesPtr = &aCommonPolylines; // put U isolines into single group with common edges
    }
    if (anIsoAspectV->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      aVPolylinesPtr = &aCommonPolylines; // put V isolines into single group with common edges
    }

    bool isParallelIso = false;
    if (theIsParallel)
    {
      int aNbFaces = 0;
      for (TopExp_Explorer aFaceExplorer(theShape, TopAbs_FACE); aFaceExplorer.More();
           aFaceExplorer.Next())
      {
        ++aNbFaces;
      }
      if (aNbFaces > 1)
      {
        isParallelIso = true;
        std::vector<TopoDS_Face> aFaces(aNbFaces);
        aNbFaces = 0;
        for (TopExp_Explorer aFaceExplorer(theShape, TopAbs_FACE); aFaceExplorer.More();
             aFaceExplorer.Next())
        {
          const TopoDS_Face& aFace = TopoDS::Face(aFaceExplorer.Current());
          if (theDrawer->IsoOnPlane() || !StdPrs_ShapeTool::IsPlanarFace(aFace))
          {
            aFaces[aNbFaces++] = aFace;
          }
        }

        StdPrs_WFShape_IsoFunctor anIsoFunctor(*aUPolylinesPtr,
                                               *aVPolylinesPtr,
                                               aFaces,
                                               theDrawer,
                                               aShapeDeflection);
        OSD_Parallel::For(0, aNbFaces, anIsoFunctor, aNbFaces < 2);
      }
    }

    if (!isParallelIso)
    {
      for (TopExp_Explorer aFaceExplorer(theShape, TopAbs_FACE); aFaceExplorer.More();
           aFaceExplorer.Next())
      {
        const TopoDS_Face& aFace = TopoDS::Face(aFaceExplorer.Current());
        if (theDrawer->IsoOnPlane() || !StdPrs_ShapeTool::IsPlanarFace(aFace))
        {
          StdPrs_Isolines::Add(aFace,
                               theDrawer,
                               aShapeDeflection,
                               *aUPolylinesPtr,
                               *aVPolylinesPtr);
        }
      }
    }

    Prs3d::AddPrimitivesGroup(thePresentation, anIsoAspectU, aUPolylines);
    Prs3d::AddPrimitivesGroup(thePresentation, anIsoAspectV, aVPolylines);
  }

  {
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>  anUnfree, aFree;
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* anUnfreePtr = &anUnfree;
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* aFreePtr    = &aFree;
    if (!theDrawer->UnFreeBoundaryDraw())
    {
      anUnfreePtr = nullptr;
    }
    else if (theDrawer->UnFreeBoundaryAspect()->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      anUnfreePtr = &aCommonPolylines; // put unfree edges into single group with common edges
    }

    if (!theDrawer->FreeBoundaryDraw())
    {
      aFreePtr = nullptr;
    }
    else if (theDrawer->FreeBoundaryAspect()->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      aFreePtr = &aCommonPolylines; // put free edges into single group with common edges
    }

    addEdges(theShape,
             theDrawer,
             aShapeDeflection,
             theDrawer->WireDraw() ? &aCommonPolylines : nullptr,
             aFreePtr,
             anUnfreePtr);
    Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->UnFreeBoundaryAspect(), anUnfree);
    Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->FreeBoundaryAspect(), aFree);
  }

  Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->WireAspect(), aCommonPolylines);

  if (occ::handle<Graphic3d_ArrayOfPoints> aVertexArray =
        AddVertexes(theShape, theDrawer->VertexDrawMode()))
  {
    occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
    aGroup->SetPrimitivesAspect(theDrawer->PointAspect()->Aspect());
    aGroup->AddPrimitiveArray(aVertexArray);
  }
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfPrimitives> StdPrs_WFShape::AddAllEdges(
  const TopoDS_Shape&              theShape,
  const occ::handle<Prs3d_Drawer>& theDrawer)
{
  const double aShapeDeflection = StdPrs_ToolTriangulatedShape::GetDeflection(theShape, theDrawer);
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> aPolylines;
  addEdges(theShape, theDrawer, aShapeDeflection, &aPolylines, &aPolylines, &aPolylines);
  return Prs3d::PrimitivesFromPolylines(aPolylines);
}

//=================================================================================================

void StdPrs_WFShape::addEdges(
  const TopoDS_Shape&                                           theShape,
  const occ::handle<Prs3d_Drawer>&                              theDrawer,
  double                                                        theShapeDeflection,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* theWire,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* theFree,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>* theUnFree)
{
  if (theShape.IsNull())
  {
    return;
  }

  NCollection_List<TopoDS_Shape> aLWire, aLFree, aLUnFree;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgeMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeMap);
  for (NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>::Iterator anEdgeIter(anEdgeMap);
       anEdgeIter.More();
       anEdgeIter.Next())
  {
    const TopoDS_Edge& anEdge        = TopoDS::Edge(anEdgeIter.Key());
    const int          aNbNeighbours = anEdgeIter.Value().Extent();
    switch (aNbNeighbours)
    {
      case 0: {
        if (theWire != nullptr)
        {
          aLWire.Append(anEdge);
        }
        break;
      }
      case 1: {
        if (theFree != nullptr)
        {
          aLFree.Append(anEdge);
        }
        break;
      }
      default: {
        if (theUnFree)
        {
          aLUnFree.Append(anEdge);
        }
        break;
      }
    }
  }

  if (!aLWire.IsEmpty())
  {
    addEdges(aLWire, theDrawer, theShapeDeflection, *theWire);
  }
  if (!aLFree.IsEmpty())
  {
    addEdges(aLFree, theDrawer, theShapeDeflection, *theFree);
  }
  if (!aLUnFree.IsEmpty())
  {
    addEdges(aLUnFree, theDrawer, theShapeDeflection, *theUnFree);
  }
}

//=================================================================================================

void StdPrs_WFShape::addEdges(
  const NCollection_List<TopoDS_Shape>&                         theEdges,
  const occ::handle<Prs3d_Drawer>&                              theDrawer,
  const double                                                  theShapeDeflection,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePolylines)
{
  NCollection_List<TopoDS_Shape>::Iterator anEdgesIter;
  for (anEdgesIter.Initialize(theEdges); anEdgesIter.More(); anEdgesIter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgesIter.Value());
    if (BRep_Tool::Degenerated(anEdge))
    {
      continue;
    }

    occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>;

    TopLoc_Location                          aLocation;
    occ::handle<Poly_Triangulation>          aTriangulation;
    occ::handle<Poly_PolygonOnTriangulation> anEdgeIndicies;
    BRep_Tool::PolygonOnTriangulation(anEdge, anEdgeIndicies, aTriangulation, aLocation);
    occ::handle<Poly_Polygon3D> aPolygon;

    if (!anEdgeIndicies.IsNull())
    {
      // Presentation based on triangulation of a face.
      const NCollection_Array1<int>& anIndices = anEdgeIndicies->Nodes();

      int anIndex = anIndices.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          aPoints->Append(aTriangulation->Node(anIndices[anIndex]));
        }
      }
      else
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          aPoints->Append(aTriangulation->Node(anIndices[anIndex]).Transformed(aLocation));
        }
      }
    }
    else if (!(aPolygon = BRep_Tool::Polygon3D(anEdge, aLocation)).IsNull())
    {
      // Presentation based on triangulation of the free edge on a surface.
      const NCollection_Array1<gp_Pnt>& aNodes  = aPolygon->Nodes();
      int                               anIndex = aNodes.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= aNodes.Upper(); ++anIndex)
        {
          aPoints->Append(aNodes.Value(anIndex));
        }
      }
      else
      {
        for (; anIndex <= aNodes.Upper(); ++anIndex)
        {
          aPoints->Append(aNodes.Value(anIndex).Transformed(aLocation));
        }
      }
    }
    else if (BRep_Tool::IsGeometric(anEdge))
    {
      // Default presentation for edges without triangulation.
      BRepAdaptor_Curve aCurve(anEdge);
      StdPrs_DeflectionCurve::Add(occ::handle<Prs3d_Presentation>(),
                                  aCurve,
                                  theShapeDeflection,
                                  theDrawer,
                                  aPoints->ChangeSequence(),
                                  false);
    }

    if (!aPoints->IsEmpty())
    {
      thePolylines.Append(aPoints);
    }
  }
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfPrimitives> StdPrs_WFShape::AddEdgesOnTriangulation(
  const TopoDS_Shape& theShape,
  const bool          theToExcludeGeometric)
{
  NCollection_Sequence<gp_Pnt> aSeqPnts;
  AddEdgesOnTriangulation(aSeqPnts, theShape, theToExcludeGeometric);
  if (aSeqPnts.Size() < 2)
  {
    return occ::handle<Graphic3d_ArrayOfSegments>();
  }

  int                                    aNbVertices = aSeqPnts.Size();
  occ::handle<Graphic3d_ArrayOfSegments> aSurfArray  = new Graphic3d_ArrayOfSegments(aNbVertices);
  for (int anI = 1; anI <= aNbVertices; anI += 2)
  {
    aSurfArray->AddVertex(aSeqPnts.Value(anI));
    aSurfArray->AddVertex(aSeqPnts.Value(anI + 1));
  }
  return aSurfArray;
}

//=================================================================================================

void StdPrs_WFShape::AddEdgesOnTriangulation(NCollection_Sequence<gp_Pnt>& theSegments,
                                             const TopoDS_Shape&           theShape,
                                             const bool                    theToExcludeGeometric)
{
  TopLoc_Location aLocation, aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIter.Current());
    if (theToExcludeGeometric)
    {
      const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(aFace, aDummyLoc);
      if (!aSurf.IsNull())
      {
        continue;
      }
    }
    if (const occ::handle<Poly_Triangulation>& aPolyTri =
          BRep_Tool::Triangulation(aFace, aLocation))
    {
      Prs3d::AddFreeEdges(theSegments, aPolyTri, aLocation);
    }
  }
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfPoints> StdPrs_WFShape::AddVertexes(const TopoDS_Shape&  theShape,
                                                                 Prs3d_VertexDrawMode theVertexMode)
{
  NCollection_Sequence<gp_Pnt> aShapeVertices;
  if (theVertexMode == Prs3d_VDM_All)
  {
    for (TopExp_Explorer aVertIter(theShape, TopAbs_VERTEX); aVertIter.More(); aVertIter.Next())
    {
      const TopoDS_Vertex& aVert = TopoDS::Vertex(aVertIter.Current());
      aShapeVertices.Append(BRep_Tool::Pnt(aVert));
    }
  }
  else
  {
    // isolated vertices
    for (TopExp_Explorer aVertIter(theShape, TopAbs_VERTEX, TopAbs_EDGE); aVertIter.More();
         aVertIter.Next())
    {
      const TopoDS_Vertex& aVert = TopoDS::Vertex(aVertIter.Current());
      aShapeVertices.Append(BRep_Tool::Pnt(aVert));
    }

    // internal vertices
    for (TopExp_Explorer anEdgeIter(theShape, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
    {
      for (TopoDS_Iterator aVertIter(anEdgeIter.Current(), false, true); aVertIter.More();
           aVertIter.Next())
      {
        const TopoDS_Shape& aVertSh = aVertIter.Value();
        if (aVertSh.Orientation() == TopAbs_INTERNAL && aVertSh.ShapeType() == TopAbs_VERTEX)
        {
          const TopoDS_Vertex& aVert = TopoDS::Vertex(aVertSh);
          aShapeVertices.Append(BRep_Tool::Pnt(aVert));
        }
      }
    }
  }

  if (aShapeVertices.IsEmpty())
  {
    return occ::handle<Graphic3d_ArrayOfPoints>();
  }

  const int                            aNbVertices  = aShapeVertices.Length();
  occ::handle<Graphic3d_ArrayOfPoints> aVertexArray = new Graphic3d_ArrayOfPoints(aNbVertices);
  for (int aVertIter = 1; aVertIter <= aNbVertices; ++aVertIter)
  {
    aVertexArray->AddVertex(aShapeVertices.Value(aVertIter));
  }
  return aVertexArray;
}
