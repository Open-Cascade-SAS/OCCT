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
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <OSD_Parallel.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <StdPrs_Isolines.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Prs3d_ShapeTool.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d.hxx>
#include <Poly_Connect.hxx>
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
#include <TopTools_ListIteratorOfListOfShape.hxx>

//! Functor for executing StdPrs_Isolines in parallel threads.
class StdPrs_WFShape_IsoFunctor
{
public:
  StdPrs_WFShape_IsoFunctor (Prs3d_NListOfSequenceOfPnt& thePolylinesU,
                             Prs3d_NListOfSequenceOfPnt& thePolylinesV,
                             const std::vector<TopoDS_Face>& theFaces,
                             const Handle(Prs3d_Drawer)& theDrawer,
                             Standard_Real theShapeDeflection)
  : myPolylinesU (thePolylinesU),
    myPolylinesV (thePolylinesV),
    myFaces (theFaces),
    myDrawer (theDrawer),
    myShapeDeflection (theShapeDeflection)
  {
    //
  }

  void operator()(const Standard_Integer& theIndex) const
  {
    Prs3d_NListOfSequenceOfPnt aPolylinesU, aPolylinesV;
    const TopoDS_Face& aFace = myFaces[theIndex];
    StdPrs_Isolines::Add (aFace, myDrawer, myShapeDeflection, aPolylinesU, aPolylinesV);
    {
      Standard_Mutex::Sentry aLock (myMutex);
      myPolylinesU.Append (aPolylinesU);
      myPolylinesV.Append (aPolylinesV);
    }
  }

private:
  StdPrs_WFShape_IsoFunctor operator= (StdPrs_WFShape_IsoFunctor& );
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
void StdPrs_WFShape::Add (const Handle(Prs3d_Presentation)& thePresentation,
                          const TopoDS_Shape& theShape,
                          const Handle(Prs3d_Drawer)& theDrawer,
                          Standard_Boolean theIsParallel)
{
  if (theShape.IsNull())
  {
    return;
  }

  Prs3d_ShapeTool aTool (theShape, theDrawer->VertexDrawMode() == Prs3d_VDM_All);

  // Explore shape elements.
  TopTools_ListOfShape aLFree, aLUnFree, aLWire;
  for (aTool.InitCurve(); aTool.MoreCurve(); aTool.NextCurve())
  {
    const TopoDS_Edge& anEdge = aTool.GetCurve();
    switch (aTool.Neighbours())
    {
      case 0:  aLWire.Append (anEdge);   break;
      case 1:  aLFree.Append (anEdge);   break;
      default: aLUnFree.Append (anEdge); break;
    }
  }

  Standard_Real aShapeDeflection = Prs3d::GetDeflection (theShape, theDrawer);

  // Draw shape elements
  {
    Handle(Graphic3d_ArrayOfPrimitives) aTriFreeEdges = AddEdgesOnTriangulation (theShape, Standard_True);
    if (!aTriFreeEdges.IsNull())
    {
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
      aGroup->SetPrimitivesAspect (theDrawer->FreeBoundaryAspect()->Aspect());
      aGroup->AddPrimitiveArray (aTriFreeEdges);
    }
  }

  Prs3d_NListOfSequenceOfPnt aCommonPolylines;
  const Handle(Prs3d_LineAspect)& aWireAspect = theDrawer->WireAspect();

  // Draw isolines
  {
    Prs3d_NListOfSequenceOfPnt  aUPolylines, aVPolylines;
    Prs3d_NListOfSequenceOfPnt* aUPolylinesPtr = &aUPolylines;
    Prs3d_NListOfSequenceOfPnt* aVPolylinesPtr = &aVPolylines;

    const Handle(Prs3d_LineAspect)& anIsoAspectU = theDrawer->UIsoAspect();
    const Handle(Prs3d_LineAspect)& anIsoAspectV = theDrawer->VIsoAspect();
    if (anIsoAspectV->Aspect()->IsEqual (*anIsoAspectU->Aspect()))
    {
      aVPolylinesPtr = aUPolylinesPtr;
    }
    if (anIsoAspectU->Aspect()->IsEqual (*aWireAspect->Aspect()))
    {
      aUPolylinesPtr = &aCommonPolylines;
    }
    if (anIsoAspectV->Aspect()->IsEqual (*aWireAspect->Aspect()))
    {
      aVPolylinesPtr = &aCommonPolylines;
    }

    bool isParallelIso = false;
    if (theIsParallel)
    {
      Standard_Integer aNbFaces = 0;
      for (TopExp_Explorer aFaceExplorer (theShape, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
      {
        ++aNbFaces;
      }
      if (aNbFaces > 1)
      {
        isParallelIso = true;
        std::vector<TopoDS_Face> aFaces (aNbFaces);
        aNbFaces = 0;
        for (TopExp_Explorer aFaceExplorer (theShape, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
        {
          const TopoDS_Face& aFace = TopoDS::Face (aFaceExplorer.Current());
          if (theDrawer->IsoOnPlane() || !Prs3d_ShapeTool::IsPlanarFace (aFace))
          {
            aFaces[aNbFaces++] = aFace;
          }
        }

        StdPrs_WFShape_IsoFunctor anIsoFunctor (*aUPolylinesPtr, *aVPolylinesPtr, aFaces, theDrawer, aShapeDeflection);
        OSD_Parallel::For (0, aNbFaces, anIsoFunctor, aNbFaces < 2);
      }
    }

    if (!isParallelIso)
    {
      for (TopExp_Explorer aFaceExplorer (theShape, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next())
      {
        const TopoDS_Face& aFace = TopoDS::Face (aFaceExplorer.Current());
        if (theDrawer->IsoOnPlane() || !Prs3d_ShapeTool::IsPlanarFace (aFace))
        {
          StdPrs_Isolines::Add (aFace, theDrawer, aShapeDeflection, *aUPolylinesPtr, *aVPolylinesPtr);
        }
      }
    }

    Prs3d::AddPrimitivesGroup (thePresentation, anIsoAspectU, aUPolylines);
    Prs3d::AddPrimitivesGroup (thePresentation, anIsoAspectV, aVPolylines);
  }

  if (!aLWire.IsEmpty() && theDrawer->WireDraw())
  {
    addEdges (aLWire, theDrawer, aShapeDeflection, aCommonPolylines);
  }

  if (!aLUnFree.IsEmpty() && theDrawer->UnFreeBoundaryDraw())
  {
    const Handle(Prs3d_LineAspect)& aLineAspect = theDrawer->UnFreeBoundaryAspect();
    if (!aLineAspect->Aspect()->IsEqual (*aWireAspect->Aspect()))
    {
      Prs3d_NListOfSequenceOfPnt aPolylines;
      addEdges (aLUnFree, theDrawer, aShapeDeflection, aPolylines);
      Prs3d::AddPrimitivesGroup (thePresentation, aLineAspect, aPolylines);
    }
    else
    {
      addEdges (aLUnFree, theDrawer, aShapeDeflection, aCommonPolylines);
    }
  }

  if (!aLFree.IsEmpty() && theDrawer->FreeBoundaryDraw())
  {
    const Handle(Prs3d_LineAspect)& aLineAspect = theDrawer->FreeBoundaryAspect();
    if (!aLineAspect->Aspect()->IsEqual (*aWireAspect->Aspect()))
    {
      Prs3d_NListOfSequenceOfPnt aPolylines;
      addEdges (aLFree, theDrawer, aShapeDeflection, aPolylines);
      Prs3d::AddPrimitivesGroup (thePresentation, aLineAspect, aPolylines);
    }
    else
    {
      addEdges (aLFree, theDrawer, aShapeDeflection, aCommonPolylines);
    }
  }

  Prs3d::AddPrimitivesGroup (thePresentation, theDrawer->WireAspect(), aCommonPolylines);

  {
    TColgp_SequenceOfPnt aShapeVertices;
    for (aTool.InitVertex(); aTool.MoreVertex(); aTool.NextVertex())
    {
      aShapeVertices.Append (BRep_Tool::Pnt (aTool.GetVertex()));
    }
    if (!aShapeVertices.IsEmpty())
    {
      addVertices (thePresentation, aShapeVertices, theDrawer->PointAspect());
    }
  }
}

// =========================================================================
// function : AddEdges
// purpose  :
// =========================================================================
void StdPrs_WFShape::addEdges (const TopTools_ListOfShape&  theEdges,
                               const Handle (Prs3d_Drawer)& theDrawer,
                               const Standard_Real          theShapeDeflection,
                               Prs3d_NListOfSequenceOfPnt&  thePolylines)
{
  TopTools_ListIteratorOfListOfShape anEdgesIter;
  for (anEdgesIter.Initialize (theEdges); anEdgesIter.More(); anEdgesIter.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgesIter.Value());
    if (BRep_Tool::Degenerated (anEdge))
    {
      continue;
    }

    Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt;

    TopLoc_Location aLocation;
    Handle(Poly_Triangulation)          aTriangulation;
    Handle(Poly_PolygonOnTriangulation) anEdgeIndicies;
    BRep_Tool::PolygonOnTriangulation (anEdge, anEdgeIndicies, aTriangulation, aLocation);
    Handle(Poly_Polygon3D) aPolygon;

    if (!anEdgeIndicies.IsNull())
    {
      // Presentation based on triangulation of a face.
      const TColStd_Array1OfInteger& anIndices = anEdgeIndicies->Nodes();
      const TColgp_Array1OfPnt&      aNodes    = aTriangulation->Nodes();

      Standard_Integer anIndex = anIndices.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          aPoints->Append (aNodes (anIndices (anIndex)));
        }
      }
      else
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          aPoints->Append (aNodes (anIndices (anIndex)).Transformed (aLocation));
        }
      }
    }
    else if (!(aPolygon = BRep_Tool::Polygon3D (anEdge, aLocation)).IsNull())
    {
      // Presentation based on triangulation of the free edge on a surface.
      const TColgp_Array1OfPnt& aNodes = aPolygon->Nodes();
      Standard_Integer anIndex = aNodes.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= aNodes.Upper(); ++anIndex)
        {
          aPoints->Append (aNodes.Value (anIndex));
        }
      }
      else
      {
        for (; anIndex <= aNodes.Upper(); ++anIndex)
        {
          aPoints->Append (aNodes.Value (anIndex).Transformed (aLocation));
        }
      }
    }
    else if (BRep_Tool::IsGeometric (anEdge))
    {
      // Default presentation for edges without triangulation.
      BRepAdaptor_Curve aCurve (anEdge);
      StdPrs_DeflectionCurve::Add (Handle(Prs3d_Presentation)(),
                                   aCurve,
                                   theShapeDeflection,
                                   theDrawer,
                                   aPoints->ChangeSequence(),
                                   Standard_False);
    }

    if (!aPoints->IsEmpty())
    {
      thePolylines.Append (aPoints);
    }
  }
}

// =========================================================================
// function : AddEdgesOnTriangulation
// purpose  :
// =========================================================================
Handle(Graphic3d_ArrayOfPrimitives) StdPrs_WFShape::AddEdgesOnTriangulation (const TopoDS_Shape& theShape,
                                                                             const Standard_Boolean theToExcludeGeometric)
{
  TColgp_SequenceOfPnt aSeqPnts;
  AddEdgesOnTriangulation (aSeqPnts, theShape, theToExcludeGeometric);
  if (aSeqPnts.Size() < 2)
  {
    return Handle(Graphic3d_ArrayOfSegments)();
  }

  Standard_Integer aNbVertices = aSeqPnts.Size();
  Handle(Graphic3d_ArrayOfSegments) aSurfArray = new Graphic3d_ArrayOfSegments (aNbVertices);
  for (Standard_Integer anI = 1; anI <= aNbVertices; anI += 2)
  {
    aSurfArray->AddVertex (aSeqPnts.Value (anI));
    aSurfArray->AddVertex (aSeqPnts.Value (anI + 1));
  }
  return aSurfArray;
}

// =========================================================================
// function : AddEdgesOnTriangulation
// purpose  :
// =========================================================================
void StdPrs_WFShape::AddEdgesOnTriangulation (TColgp_SequenceOfPnt& theSegments,
                                              const TopoDS_Shape& theShape,
                                              const Standard_Boolean theToExcludeGeometric)
{
  TopLoc_Location aLocation, aDummyLoc;
  for (TopExp_Explorer aFaceIter (theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face (aFaceIter.Current());
    if (theToExcludeGeometric)
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface (aFace, aDummyLoc);
      if (!aSurf.IsNull())
      {
        continue;
      }
    }
    const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation (aFace, aLocation);
    if (T.IsNull())
    {
      continue;
    }

    const TColgp_Array1OfPnt& aNodes = T->Nodes();

    // Build the connect tool.
    Poly_Connect aPolyConnect (T);

    Standard_Integer aNbTriangles = T->NbTriangles();
    Standard_Integer aT[3];
    Standard_Integer aN[3];

    // Count the free edges.
    Standard_Integer aNbFree = 0;
    for (Standard_Integer anI = 1; anI <= aNbTriangles; ++anI)
    {
      aPolyConnect.Triangles (anI, aT[0], aT[1], aT[2]);
      for (Standard_Integer aJ = 0; aJ < 3; ++aJ)
      {
        if (aT[aJ] == 0)
        {
          ++aNbFree;
        }
      }
    }

    if (aNbFree == 0)
    {
      continue;
    }

    // Allocate the arrays.
    TColStd_Array1OfInteger aFree (1, 2 * aNbFree);
    Standard_Integer aNbInternal = (3 * aNbTriangles - aNbFree) / 2;
    TColStd_Array1OfInteger anInternal (0, 2 * aNbInternal);

    Standard_Integer aFreeIndex = 1, anIntIndex = 1;
    const Poly_Array1OfTriangle& aTriangles = T->Triangles();
    for (Standard_Integer anI = 1; anI <= aNbTriangles; ++anI)
    {
      aPolyConnect.Triangles (anI, aT[0], aT[1], aT[2]);
      aTriangles (anI).Get (aN[0], aN[1], aN[2]);
      for (Standard_Integer aJ = 0; aJ < 3; aJ++)
      {
        Standard_Integer k = (aJ + 1) % 3;
        if (aT[aJ] == 0)
        {
          aFree (aFreeIndex)     = aN[aJ];
          aFree (aFreeIndex + 1) = aN[k];
          aFreeIndex += 2;
        }
        // internal edge if this triangle has a lower index than the adjacent.
        else if (anI < aT[aJ])
        {
          anInternal (anIntIndex)     = aN[aJ];
          anInternal (anIntIndex + 1) = aN[k];
          anIntIndex += 2;
        }
      }
    }

    // free edges
    Standard_Integer aFreeHalfNb = aFree.Length() / 2;
    for (Standard_Integer anI = 1; anI <= aFreeHalfNb; ++anI)
    {
      gp_Pnt aPoint1 = aNodes (aFree (2 * anI - 1)).Transformed (aLocation);
      gp_Pnt aPoint2 = aNodes (aFree (2 * anI    )).Transformed (aLocation);
      theSegments.Append (aPoint1);
      theSegments.Append (aPoint2);
    }
  }
}

// =========================================================================
// function : AddPoints
// purpose  :
// =========================================================================
void StdPrs_WFShape::addVertices (const Handle (Prs3d_Presentation)& thePresentation,
                                  const TColgp_SequenceOfPnt&        theVertices,
                                  const Handle (Prs3d_PointAspect)&  theAspect)
{
  Standard_Integer aNbVertices = theVertices.Length();
  if (aNbVertices < 1)
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPoints) aVertexArray = new Graphic3d_ArrayOfPoints (aNbVertices);
  for (Standard_Integer anI = 1; anI <= aNbVertices; ++anI)
  {
    aVertexArray->AddVertex (theVertices.Value (anI));
  }

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
  aGroup->SetPrimitivesAspect (theAspect->Aspect());
  aGroup->AddPrimitiveArray (aVertexArray);
}
