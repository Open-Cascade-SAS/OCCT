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
#include <TColgp_HSequenceOfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Standard_Mutex.hxx>

//! Functor for executing StdPrs_Isolines in parallel threads.
class StdPrs_WFShape_IsoFunctor
{
public:
  StdPrs_WFShape_IsoFunctor(Prs3d_NListOfSequenceOfPnt&     thePolylinesU,
                            Prs3d_NListOfSequenceOfPnt&     thePolylinesV,
                            const std::vector<TopoDS_Face>& theFaces,
                            const Handle(Prs3d_Drawer)&     theDrawer,
                            Standard_Real                   theShapeDeflection)
      : myPolylinesU(thePolylinesU),
        myPolylinesV(thePolylinesV),
        myFaces(theFaces),
        myDrawer(theDrawer),
        myShapeDeflection(theShapeDeflection)
  {
    //
  }

  void operator()(const Standard_Integer& theIndex) const
  {
    Prs3d_NListOfSequenceOfPnt aPolylinesU, aPolylinesV;
    const TopoDS_Face&         aFace = myFaces[theIndex];
    StdPrs_Isolines::Add(aFace, myDrawer, myShapeDeflection, aPolylinesU, aPolylinesV);
    {
      Standard_Mutex::Sentry aLock(myMutex);
      myPolylinesU.Append(aPolylinesU);
      myPolylinesV.Append(aPolylinesV);
    }
  }

private:
  StdPrs_WFShape_IsoFunctor operator=(StdPrs_WFShape_IsoFunctor&);

private:
  Prs3d_NListOfSequenceOfPnt&     myPolylinesU;
  Prs3d_NListOfSequenceOfPnt&     myPolylinesV;
  const std::vector<TopoDS_Face>& myFaces;
  const Handle(Prs3d_Drawer)&     myDrawer;
  mutable Standard_Mutex          myMutex;
  const Standard_Real             myShapeDeflection;
};

// =========================================================================
// function : Add
// purpose  :
// =========================================================================
void StdPrs_WFShape::Add(const Handle(Prs3d_Presentation)& thePresentation,
                         const TopoDS_Shape&               theShape,
                         const Handle(Prs3d_Drawer)&       theDrawer,
                         Standard_Boolean                  theIsParallel)
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
  if (Handle(Graphic3d_ArrayOfPrimitives) aTriFreeEdges =
        AddEdgesOnTriangulation(theShape, Standard_True))
  {
    Handle(Graphic3d_Group) aGroup = thePresentation->NewGroup();
    aGroup->SetPrimitivesAspect(theDrawer->FreeBoundaryAspect()->Aspect());
    aGroup->AddPrimitiveArray(aTriFreeEdges);
  }

  Prs3d_NListOfSequenceOfPnt      aCommonPolylines;
  const Handle(Prs3d_LineAspect)& aWireAspect = theDrawer->WireAspect();
  const Standard_Real             aShapeDeflection =
    StdPrs_ToolTriangulatedShape::GetDeflection(theShape, theDrawer);

  // Draw isolines
  {
    Prs3d_NListOfSequenceOfPnt  aUPolylines, aVPolylines;
    Prs3d_NListOfSequenceOfPnt* aUPolylinesPtr = &aUPolylines;
    Prs3d_NListOfSequenceOfPnt* aVPolylinesPtr = &aVPolylines;

    const Handle(Prs3d_LineAspect)& anIsoAspectU = theDrawer->UIsoAspect();
    const Handle(Prs3d_LineAspect)& anIsoAspectV = theDrawer->VIsoAspect();
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
      Standard_Integer aNbFaces = 0;
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
    Prs3d_NListOfSequenceOfPnt  anUnfree, aFree;
    Prs3d_NListOfSequenceOfPnt* anUnfreePtr = &anUnfree;
    Prs3d_NListOfSequenceOfPnt* aFreePtr    = &aFree;
    if (!theDrawer->UnFreeBoundaryDraw())
    {
      anUnfreePtr = NULL;
    }
    else if (theDrawer->UnFreeBoundaryAspect()->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      anUnfreePtr = &aCommonPolylines; // put unfree edges into single group with common edges
    }

    if (!theDrawer->FreeBoundaryDraw())
    {
      aFreePtr = NULL;
    }
    else if (theDrawer->FreeBoundaryAspect()->Aspect()->IsEqual(*aWireAspect->Aspect()))
    {
      aFreePtr = &aCommonPolylines; // put free edges into single group with common edges
    }

    addEdges(theShape,
             theDrawer,
             aShapeDeflection,
             theDrawer->WireDraw() ? &aCommonPolylines : NULL,
             aFreePtr,
             anUnfreePtr);
    Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->UnFreeBoundaryAspect(), anUnfree);
    Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->FreeBoundaryAspect(), aFree);
  }

  Prs3d::AddPrimitivesGroup(thePresentation, theDrawer->WireAspect(), aCommonPolylines);

  if (Handle(Graphic3d_ArrayOfPoints) aVertexArray =
        AddVertexes(theShape, theDrawer->VertexDrawMode()))
  {
    Handle(Graphic3d_Group) aGroup = thePresentation->NewGroup();
    aGroup->SetPrimitivesAspect(theDrawer->PointAspect()->Aspect());
    aGroup->AddPrimitiveArray(aVertexArray);
  }
}

// =========================================================================
// function : AddAllEdges
// purpose  :
// =========================================================================
Handle(Graphic3d_ArrayOfPrimitives) StdPrs_WFShape::AddAllEdges(
  const TopoDS_Shape&         theShape,
  const Handle(Prs3d_Drawer)& theDrawer)
{
  const Standard_Real aShapeDeflection =
    StdPrs_ToolTriangulatedShape::GetDeflection(theShape, theDrawer);
  Prs3d_NListOfSequenceOfPnt aPolylines;
  addEdges(theShape, theDrawer, aShapeDeflection, &aPolylines, &aPolylines, &aPolylines);
  return Prs3d::PrimitivesFromPolylines(aPolylines);
}

// =========================================================================
// function : addEdges
// purpose  :
// =========================================================================
void StdPrs_WFShape::addEdges(const TopoDS_Shape&         theShape,
                              const Handle(Prs3d_Drawer)& theDrawer,
                              Standard_Real               theShapeDeflection,
                              Prs3d_NListOfSequenceOfPnt* theWire,
                              Prs3d_NListOfSequenceOfPnt* theFree,
                              Prs3d_NListOfSequenceOfPnt* theUnFree)
{
  if (theShape.IsNull())
  {
    return;
  }

  TopTools_ListOfShape                      aLWire, aLFree, aLUnFree;
  TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, anEdgeMap);
  for (TopTools_IndexedDataMapOfShapeListOfShape::Iterator anEdgeIter(anEdgeMap); anEdgeIter.More();
       anEdgeIter.Next())
  {
    const TopoDS_Edge&     anEdge        = TopoDS::Edge(anEdgeIter.Key());
    const Standard_Integer aNbNeighbours = anEdgeIter.Value().Extent();
    switch (aNbNeighbours)
    {
      case 0: {
        if (theWire != NULL)
        {
          aLWire.Append(anEdge);
        }
        break;
      }
      case 1: {
        if (theFree != NULL)
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

// =========================================================================
// function : AddEdges
// purpose  :
// =========================================================================
void StdPrs_WFShape::addEdges(const TopTools_ListOfShape& theEdges,
                              const Handle(Prs3d_Drawer)& theDrawer,
                              const Standard_Real         theShapeDeflection,
                              Prs3d_NListOfSequenceOfPnt& thePolylines)
{
  TopTools_ListIteratorOfListOfShape anEdgesIter;
  for (anEdgesIter.Initialize(theEdges); anEdgesIter.More(); anEdgesIter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgesIter.Value());
    if (BRep_Tool::Degenerated(anEdge))
    {
      continue;
    }

    Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt;

    TopLoc_Location                     aLocation;
    Handle(Poly_Triangulation)          aTriangulation;
    Handle(Poly_PolygonOnTriangulation) anEdgeIndicies;
    BRep_Tool::PolygonOnTriangulation(anEdge, anEdgeIndicies, aTriangulation, aLocation);
    Handle(Poly_Polygon3D) aPolygon;

    if (!anEdgeIndicies.IsNull())
    {
      // Presentation based on triangulation of a face.
      const TColStd_Array1OfInteger& anIndices = anEdgeIndicies->Nodes();

      Standard_Integer anIndex = anIndices.Lower();
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
      const TColgp_Array1OfPnt& aNodes  = aPolygon->Nodes();
      Standard_Integer          anIndex = aNodes.Lower();
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
      StdPrs_DeflectionCurve::Add(Handle(Prs3d_Presentation)(),
                                  aCurve,
                                  theShapeDeflection,
                                  theDrawer,
                                  aPoints->ChangeSequence(),
                                  Standard_False);
    }

    if (!aPoints->IsEmpty())
    {
      thePolylines.Append(aPoints);
    }
  }
}

// =========================================================================
// function : AddEdgesOnTriangulation
// purpose  :
// =========================================================================
Handle(Graphic3d_ArrayOfPrimitives) StdPrs_WFShape::AddEdgesOnTriangulation(
  const TopoDS_Shape&    theShape,
  const Standard_Boolean theToExcludeGeometric)
{
  TColgp_SequenceOfPnt aSeqPnts;
  AddEdgesOnTriangulation(aSeqPnts, theShape, theToExcludeGeometric);
  if (aSeqPnts.Size() < 2)
  {
    return Handle(Graphic3d_ArrayOfSegments)();
  }

  Standard_Integer                  aNbVertices = aSeqPnts.Size();
  Handle(Graphic3d_ArrayOfSegments) aSurfArray  = new Graphic3d_ArrayOfSegments(aNbVertices);
  for (Standard_Integer anI = 1; anI <= aNbVertices; anI += 2)
  {
    aSurfArray->AddVertex(aSeqPnts.Value(anI));
    aSurfArray->AddVertex(aSeqPnts.Value(anI + 1));
  }
  return aSurfArray;
}

// =========================================================================
// function : AddEdgesOnTriangulation
// purpose  :
// =========================================================================
void StdPrs_WFShape::AddEdgesOnTriangulation(TColgp_SequenceOfPnt&  theSegments,
                                             const TopoDS_Shape&    theShape,
                                             const Standard_Boolean theToExcludeGeometric)
{
  TopLoc_Location aLocation, aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIter.Current());
    if (theToExcludeGeometric)
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aDummyLoc);
      if (!aSurf.IsNull())
      {
        continue;
      }
    }
    if (const Handle(Poly_Triangulation)& aPolyTri = BRep_Tool::Triangulation(aFace, aLocation))
    {
      Prs3d::AddFreeEdges(theSegments, aPolyTri, aLocation);
    }
  }
}

// =========================================================================
// function : AddVertexes
// purpose  :
// =========================================================================
Handle(Graphic3d_ArrayOfPoints) StdPrs_WFShape::AddVertexes(const TopoDS_Shape&  theShape,
                                                            Prs3d_VertexDrawMode theVertexMode)
{
  TColgp_SequenceOfPnt aShapeVertices;
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
      for (TopoDS_Iterator aVertIter(anEdgeIter.Current(), Standard_False, Standard_True);
           aVertIter.More();
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
    return Handle(Graphic3d_ArrayOfPoints)();
  }

  const Standard_Integer          aNbVertices  = aShapeVertices.Length();
  Handle(Graphic3d_ArrayOfPoints) aVertexArray = new Graphic3d_ArrayOfPoints(aNbVertices);
  for (Standard_Integer aVertIter = 1; aVertIter <= aNbVertices; ++aVertIter)
  {
    aVertexArray->AddVertex(aShapeVertices.Value(aVertIter));
  }
  return aVertexArray;
}
