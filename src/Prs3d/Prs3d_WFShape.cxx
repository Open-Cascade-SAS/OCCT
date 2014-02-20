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

#include <Prs3d_WFShape.hxx>

#include <Bnd_Box.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Tool.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <gp_Pnt.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_NListIteratorOfListOfSequenceOfPnt.hxx>
#include <Prs3d_ShapeTool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

namespace
{

  //! Compare two aspects
  inline Standard_Boolean IsSame (const Handle(Graphic3d_AspectLine3d)& theUAspect,
                                  const Handle(Graphic3d_AspectLine3d)& theVAspect)
  {
    Quantity_Color aCU, aCV;
    Aspect_TypeOfLine aTlU, aTlV;
    Standard_Real aWU, aWV;
    theUAspect->Values (aCU, aTlU, aWU);
    theVAspect->Values (aCV, aTlV, aWV);
    return aCU  == aCV 
        && aTlU == aTlV
        && aWU  == aWV;
  }

};

// =========================================================================
// function: AddPolygon
// purpose :
// =========================================================================
Standard_Boolean Prs3d_WFShape::AddPolygon (const TopoDS_Edge&    theEdge,
                                            const Standard_Real   theDeflection,
                                            TColgp_SequenceOfPnt& thePoints)
{
  TopLoc_Location aLocation;
  Standard_Real aFirst, aLast;
  Handle(Geom_Curve)     aCurve3d = BRep_Tool::Curve     (theEdge, aFirst, aLast);
  Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D (theEdge, aLocation);
  if (!aPolygon.IsNull())
  {
    if ((aPolygon->Deflection() <= theDeflection) || aCurve3d.IsNull())
    {
      const TColgp_Array1OfPnt& aPoints = aPolygon->Nodes();
      Standard_Integer anIndex = aPoints.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= aPoints.Upper(); ++anIndex)
        {
          thePoints.Append (aPoints.Value (anIndex));
        }
      }
      else
      {
        for (; anIndex <= aPoints.Upper(); ++anIndex)
        {
          thePoints.Append (aPoints.Value (anIndex).Transformed (aLocation));
        }
      }
      return Standard_True;
    }
  }

  Handle(Poly_Triangulation)          aTriangulation;
  Handle(Poly_PolygonOnTriangulation) aHIndices;
  BRep_Tool::PolygonOnTriangulation (theEdge, aHIndices, aTriangulation, aLocation);
  if (!aHIndices.IsNull())
  {
    if ((aHIndices->Deflection() <= theDeflection) || aCurve3d.IsNull())
    {
      const TColStd_Array1OfInteger& anIndices = aHIndices->Nodes();
      const TColgp_Array1OfPnt&      aNodes    = aTriangulation->Nodes();

      Standard_Integer anIndex = anIndices.Lower();
      if (aLocation.IsIdentity())
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          thePoints.Append (aNodes (anIndices (anIndex)));
        }
      }
      else
      {
        for (; anIndex <= anIndices.Upper(); ++anIndex)
        {
          thePoints.Append (aNodes (anIndices (anIndex)).Transformed (aLocation));
        }
      }
      return Standard_True;
    }
  }
  return Standard_False;
}

// =========================================================================
// function: Add
// purpose :
// =========================================================================
void Prs3d_WFShape::Add (const Handle (Prs3d_Presentation)& thePresentation,
                         const TopoDS_Shape&                theShape,
                         const Handle (Prs3d_Drawer)&       theDrawer)
{
  if (theShape.IsNull())
  {
    return;
  }

  Prs3d_ShapeTool aTool (theShape);
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

  Standard_Real aDeflection = theDrawer->MaximalChordialDeviation();
  if (theDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    // The arrow calculation is based on the global min max
    Bnd_Box aBndBox;
    BRepBndLib::Add (theShape, aBndBox);
    if (!aBndBox.IsVoid())
    {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      aBndBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      aDeflection = Max (aXmax-aXmin, Max (aYmax-aYmin, aZmax-aZmin))
                       * theDrawer->DeviationCoefficient();
    }
  }

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);

  Prs3d_NListOfSequenceOfPnt anUIsoCurves;
  Prs3d_NListOfSequenceOfPnt aVIsoCurves;
  Prs3d_NListOfSequenceOfPnt aWireCurves;
  Prs3d_NListOfSequenceOfPnt aFreeCurves;
  Prs3d_NListOfSequenceOfPnt anUnFreeCurves;
  TColgp_SequenceOfPnt       aShapePoints;

  const Standard_Integer anIsoU = theDrawer->UIsoAspect()->Number();
  const Standard_Integer anIsoV = theDrawer->VIsoAspect()->Number();

  Standard_Boolean hasIsoU = anIsoU > 0;
  Standard_Boolean hasIsoV = anIsoV > 0;

  if (IsSame (theDrawer->UIsoAspect()->Aspect(),
              theDrawer->VIsoAspect()->Aspect()))
  {
    if (hasIsoU || hasIsoV)
    {
      BRepAdaptor_Surface aSurface;
      for (aTool.InitFace(); aTool.MoreFace(); aTool.NextFace())
      {
        if (aTool.HasSurface())
        {
          if (!aTool.IsPlanarFace() || theDrawer->IsoOnPlane())
          {
            aSurface.Initialize (aTool.GetFace());
            Handle(BRepAdaptor_HSurface) aHSurface = new BRepAdaptor_HSurface (aSurface);
            try
            {
              OCC_CATCH_SIGNALS
              Prs3d_NListOfSequenceOfPnt aCurUIsoCurves;
              myFaceAlgo.Add (thePresentation, aHSurface,
                              hasIsoU, hasIsoV,
                              aDeflection,
                              anIsoU, anIsoV,
                              theDrawer,
                              aCurUIsoCurves);
              Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
              for (anIt.Init (aCurUIsoCurves); anIt.More(); anIt.Next())
              {
                anUIsoCurves.Append (anIt.Value());
              }
            }
            catch (Standard_Failure)
            {
            }
          }
        }
      }
    }
  }
  else
  {
    if (hasIsoU)
    {
      BRepAdaptor_Surface aSurface;
      for (aTool.InitFace(); aTool.MoreFace(); aTool.NextFace())
      {
        if (aTool.HasSurface())
        {
          if (!aTool.IsPlanarFace() || theDrawer->IsoOnPlane())
          {
            aSurface.Initialize (aTool.GetFace());
            Handle(BRepAdaptor_HSurface) aHSurface = new BRepAdaptor_HSurface (aSurface);
            try
            {
              OCC_CATCH_SIGNALS
              Prs3d_NListOfSequenceOfPnt aCurUIsoCurves;
              myFaceAlgo.Add (thePresentation, aHSurface,
                              hasIsoU, Standard_False,
                              aDeflection,
                              anIsoU, 0,
                              theDrawer,
                              aCurUIsoCurves);
            }
            catch (Standard_Failure)
            {
            #ifdef DEB
              const TopoDS_Face& aFace = aSurface.Face();
              std::cout << "Problem with the face " << (void* ) &(*(aFace).TShape()) << std::endl;
            #endif
            }
          }
        }
      }
    }
    if (hasIsoV)
    {
      BRepAdaptor_Surface aSurface;
      for (aTool.InitFace(); aTool.MoreFace(); aTool.NextFace())
      {
        if (aTool.HasSurface())
        {
          if (!aTool.IsPlanarFace() || theDrawer->IsoOnPlane())
          {
            aSurface.Initialize (aTool.GetFace());
            Handle(BRepAdaptor_HSurface) aHSurface = new BRepAdaptor_HSurface (aSurface);
            try
            {
              OCC_CATCH_SIGNALS
              Prs3d_NListOfSequenceOfPnt aCurUIsoCurves;
              myFaceAlgo.Add (thePresentation, aHSurface,
                              Standard_False, hasIsoV,
                              aDeflection,
                              0, anIsoV,
                              theDrawer,
                              aCurUIsoCurves);
            }
            catch (Standard_Failure)
            {
            #ifdef DEB
              const TopoDS_Face& aFace = aSurface.Face();
              std::cout << "Problem with the face " << (void* ) &(*(aFace).TShape()) << std::endl;
            #endif
            }
          }
        }
      }
    }
  }

  Standard_Integer aNbVertices = 0, aNbBounds = 0;
  if (anUIsoCurves.Size() > 0)
  {
    aNbBounds = anUIsoCurves.Size();
    Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
    for (anIt.Init (anUIsoCurves); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value().Length();
    }
    Handle(Graphic3d_ArrayOfPolylines) anUIsoArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anIt.Init (anUIsoCurves); anIt.More(); anIt.Next())
    {
      TColgp_SequenceOfPnt aPoints;
      aPoints.Assign (anIt.Value());
      anUIsoArray->AddBound (aPoints.Length());
      for (Standard_Integer anI = 1; anI <= aPoints.Length(); ++anI)
      {
        anUIsoArray->AddVertex (aPoints.Value (anI));
      }
    }
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->UIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (anUIsoArray);
  }

  if (aVIsoCurves.Size() > 0)
  {
    aNbBounds = aVIsoCurves.Size();
    Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
    for (anIt.Init (aVIsoCurves); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value().Length();
    }
    Handle(Graphic3d_ArrayOfPolylines) VIsoArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anIt.Init (aVIsoCurves); anIt.More(); anIt.Next())
    {
      TColgp_SequenceOfPnt aPoints;
      aPoints.Assign (anIt.Value());
      VIsoArray->AddBound (aPoints.Length());
      for (int anI = 1; anI <= aPoints.Length(); anI++)
      {
        VIsoArray->AddVertex (aPoints.Value (anI));
      }
    }
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->VIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (VIsoArray);
  }

  TopLoc_Location aLocation;
  Standard_Integer anI, aJ, aN[3];

  TColgp_SequenceOfPnt aSurfPoints;
  for (aTool.InitFace(); aTool.MoreFace(); aTool.NextFace())
  {
    if (!aTool.HasSurface())
    {
      Handle(Poly_Triangulation) T = aTool.CurrentTriangulation (aLocation);
      if (!T.IsNull())
      {
        const TColgp_Array1OfPnt& aNodes = T->Nodes();
        // Build the connect tool
        Poly_Connect aPolyConnect (T);

        Standard_Integer aNbTriangles = T->NbTriangles();
        Standard_Integer aT[3];

        // Count the free edges
        Standard_Integer aNbFree = 0;
        for (anI = 1; anI <= aNbTriangles; ++anI)
        {
          aPolyConnect.Triangles (anI, aT[0], aT[1], aT[2]);
          for (aJ = 0; aJ < 3; ++aJ)
          {
            if (aT[aJ] == 0)
            {
              ++aNbFree;
            }
          }
        }

        // Allocate the arrays
        TColStd_Array1OfInteger aFree (1, 2 * aNbFree);
        Standard_Integer aNbInternal = (3 * aNbTriangles - aNbFree) / 2;
        TColStd_Array1OfInteger anInternal (0, 2 * aNbInternal);

        Standard_Integer aFreeIndex = 1, anIntIndex = 1;
        const Poly_Array1OfTriangle& aTriangles = T->Triangles();
        for (anI = 1; anI <= aNbTriangles; ++anI)
        {
          aPolyConnect.Triangles (anI, aT[0], aT[1], aT[2]);
          aTriangles (anI).Get (aN[0], aN[1], aN[2]);
          for (aJ = 0; aJ < 3; aJ++)
          {
            Standard_Integer k = (aJ + 1) % 3;
            if (aT[aJ] == 0)
            {
              aFree (aFreeIndex)     = aN[aJ];
              aFree (aFreeIndex + 1) = aN[k];
              aFreeIndex += 2;
            }
            // internal edge if this triangle has a lower index than the adjacent
            else if (anI < aT[aJ])
            {
              anInternal (anIntIndex)     = aN[aJ];
              anInternal (anIntIndex + 1) = aN[k];
              anIntIndex += 2;
            }
          }
        }

        if (!aTool.HasSurface())
        {
          // free edges
          Standard_Integer aFreeHalfNb = aFree.Length() / 2;
          for (anI = 1; anI <= aFreeHalfNb; ++anI)
          {
            gp_Pnt aPoint1 = aNodes (aFree (2 * anI - 1)).Transformed (aLocation);
            gp_Pnt aPoint2 = aNodes (aFree (2 * anI    )).Transformed (aLocation);
            aSurfPoints.Append (aPoint1);
            aSurfPoints.Append (aPoint2);
          }
        }
      }
    }
  }
  if (aSurfPoints.Length() > 0)
  {
    aNbVertices = aSurfPoints.Length();
    aNbBounds   = (Standard_Integer)aNbVertices / 2;
    Handle(Graphic3d_ArrayOfPolylines) aSurfArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anI = 1; anI <= aNbVertices; anI += 2)
    {
      aSurfArray->AddBound (2);
      aSurfArray->AddVertex (aSurfPoints.Value (anI));
      aSurfArray->AddVertex (aSurfPoints.Value (anI + 1));
    }
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->FreeBoundaryAspect()->Aspect());
    aGroup->AddPrimitiveArray (aSurfArray);
  }

  TopTools_ListIteratorOfListOfShape anIt;
  if (theDrawer->WireDraw())
  {
    // Wire (without any neighbour)
    aGroup->SetPrimitivesAspect (theDrawer->WireAspect()->Aspect());
    for (anIt.Initialize(aLWire); anIt.More(); anIt.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anIt.Value());
      try
      {
        OCC_CATCH_SIGNALS
        TColgp_SequenceOfPnt aPoints;
        if (!AddPolygon (anEdge, aDeflection, aPoints))
        {
          if (BRep_Tool::IsGeometric (anEdge))
          {
            BRepAdaptor_Curve aCurve (anEdge);
            myCurveAlgo.Add (thePresentation, aCurve, aDeflection, theDrawer,
                             aPoints, Standard_False);
            aWireCurves.Append (aPoints);
          }
        }
        else
        {
          aWireCurves.Append (aPoints);
        }
      }
      catch (Standard_Failure)
      {
      #ifdef DEB
        std::cout << "probleme sur aLocation'edge " << (void* ) &(*(anEdge).TShape()) << std::endl;
      #endif
      }
    }
  }

  if (theDrawer->FreeBoundaryDraw())
  {
    // aFree boundaries;
    for (anIt.Initialize (aLFree); anIt.More(); anIt.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anIt.Value());
      if (!BRep_Tool::Degenerated (anEdge))
      {
        try
        {
          OCC_CATCH_SIGNALS
          TColgp_SequenceOfPnt aPoints;
          if (!AddPolygon (anEdge, aDeflection, aPoints))
          {
            if (BRep_Tool::IsGeometric (anEdge))
            {
              BRepAdaptor_Curve aCurve (anEdge);
              myCurveAlgo.Add (thePresentation, aCurve, aDeflection, theDrawer,
                               aPoints, Standard_False);
              aFreeCurves.Append (aPoints);
            }
          }
          else
          {
            aFreeCurves.Append (aPoints);
          }
        }
        catch (Standard_Failure)
        {
        #ifdef DEB
          std::cout << "probleme sur aLocation'edge " << (void* ) &(*(anEdge).TShape()) << std::endl;
        #endif
        }
      }
    }
  }

  if (theDrawer->UnFreeBoundaryDraw())
  {
    // Unfree boundaries;
    for (anIt.Initialize (aLUnFree); anIt.More(); anIt.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anIt.Value());
      try
      {
        OCC_CATCH_SIGNALS
        TColgp_SequenceOfPnt aPoints;
        if (!AddPolygon (anEdge, aDeflection, aPoints))
        {
          if (BRep_Tool::IsGeometric (anEdge))
          {
            BRepAdaptor_Curve aCurve (anEdge);
            myCurveAlgo.Add (thePresentation, aCurve, aDeflection, theDrawer, aPoints, Standard_False);
            anUnFreeCurves.Append (aPoints);
          }
        }
        else
        {
          anUnFreeCurves.Append (aPoints);
        }
      }
      catch (Standard_Failure)
      {
      #ifdef DEB
        std::cout << "probleme sur aLocation'edge " << (void* ) &(*(anEdge).TShape()) << std::endl;
      #endif
      }
    }
  }

  if (aWireCurves.Size() > 0)
  {
    aNbBounds = aWireCurves.Size();
    Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
    for (anIt.Init (aWireCurves); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value().Length();
    }
    Handle(Graphic3d_ArrayOfPolylines) WireArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anIt.Init (aWireCurves); anIt.More(); anIt.Next())
    {
      TColgp_SequenceOfPnt aPoints;
      aPoints.Assign (anIt.Value());
      WireArray->AddBound (aPoints.Length());
      for (anI = 1; anI <= aPoints.Length(); ++anI)
      {
        WireArray->AddVertex (aPoints.Value (anI));
      }
    }
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->WireAspect()->Aspect());
    aGroup->AddPrimitiveArray (WireArray);
  }

  if (aFreeCurves.Size() > 0)
  {
    aNbBounds = aFreeCurves.Size();
    Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
    for (anIt.Init (aFreeCurves); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value().Length();
    }
    Handle(Graphic3d_ArrayOfPolylines) aFreeArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anIt.Init(aFreeCurves); anIt.More(); anIt.Next())
    {
      TColgp_SequenceOfPnt aPoints;
      aPoints.Assign (anIt.Value());
      aFreeArray->AddBound (aPoints.Length());
      for (anI = 1; anI <= aPoints.Length(); ++anI)
      {
        aFreeArray->AddVertex (aPoints.Value (anI));
      }
    }  
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->FreeBoundaryAspect()->Aspect());
    aGroup->AddPrimitiveArray (aFreeArray);
  }

  if (anUnFreeCurves.Size() > 0)
  {
    aNbBounds = anUnFreeCurves.Size();
    Prs3d_NListIteratorOfListOfSequenceOfPnt anIt;
    for (anIt.Init (anUnFreeCurves); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value().Length();
    }
    Handle(Graphic3d_ArrayOfPolylines) anUnFreeArray = new Graphic3d_ArrayOfPolylines (aNbVertices, aNbBounds);
    for (anIt.Init (anUnFreeCurves); anIt.More(); anIt.Next())
    {
      TColgp_SequenceOfPnt aPoints;
      aPoints.Assign (anIt.Value());
      anUnFreeArray->AddBound (aPoints.Length());
      for (anI = 1; anI <= aPoints.Length(); ++anI)
      {
        anUnFreeArray->AddVertex (aPoints.Value (anI));
      }
    }
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->UnFreeBoundaryAspect()->Aspect());
    aGroup->AddPrimitiveArray (anUnFreeArray);
  }

  // Points
  for (aTool.InitVertex(); aTool.MoreVertex(); aTool.NextVertex())
  {
    aShapePoints.Append (BRep_Tool::Pnt (aTool.GetVertex()));
  }

  aNbVertices = aShapePoints.Length();
  if (aNbVertices > 0)
  {
    Handle(Graphic3d_ArrayOfPoints) aPointArray = new Graphic3d_ArrayOfPoints (aNbVertices);
    for (anI = 1; anI <= aNbVertices; ++anI)
    {
      aPointArray->AddVertex (aShapePoints.Value (anI));
    }

    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->PointAspect()->Aspect());
    aGroup->AddPrimitiveArray (aPointArray);
  }
}

// =========================================================================
// function: PickCurve
// purpose :
// =========================================================================
Handle(TopTools_HSequenceOfShape) Prs3d_WFShape::PickCurve
      (const Quantity_Length        theX,
       const Quantity_Length        theY,
       const Quantity_Length        theZ,
       const Quantity_Length        theDistance,
       const TopoDS_Shape&          theShape,
       const Handle (Prs3d_Drawer)& theDrawer)
{
  Handle(TopTools_HSequenceOfShape) aSeq = new TopTools_HSequenceOfShape();
  Prs3d_ShapeTool aTool (theShape);
  for (aTool.InitCurve(); aTool.MoreCurve(); aTool.NextCurve())
  {
    Bnd_Box aBndBox = aTool.CurveBound();
    aBndBox.Enlarge (theDistance);
    if (!aBndBox.IsOut (gp_Pnt (theX, theY, theZ)))
    {
      if (myCurveAlgo.Match (theX, theY, theZ, theDistance,
                             BRepAdaptor_Curve (aTool.GetCurve()), theDrawer))
      {
        Standard_Boolean isContain = Standard_False;
        for (Standard_Integer anI = 1; anI <= aSeq->Length(); ++anI)
        {
          if (aSeq->Value (anI) == (aTool.GetCurve()))
          {
            isContain = Standard_True;
            break;
          }
        }
        if (!isContain)
        {
          aSeq->Append (aTool.GetCurve());
        }
      }
    }
  }
  return aSeq;
}

// =========================================================================
// function: PickPatch
// purpose :
// =========================================================================
Handle(TopTools_HSequenceOfShape) Prs3d_WFShape::PickPatch
      (const Quantity_Length       theX,
       const Quantity_Length       theY,
       const Quantity_Length       theZ,
       const Quantity_Length       theDistance,
       const TopoDS_Shape&         theShape,
       const Handle(Prs3d_Drawer)& theDrawer)
{
   Handle(TopTools_HSequenceOfShape) aSeq = new TopTools_HSequenceOfShape();
   Prs3d_ShapeTool aTool (theShape);

   Standard_Boolean aRba1 = theDrawer->UIsoAspect()->Number() != 0;
   Standard_Boolean aRba2 = theDrawer->VIsoAspect()->Number() != 0;
   Standard_Boolean isContain;

   if (aRba1 || aRba2)
   {
     BRepAdaptor_Surface aSurface;
     for (aTool.InitFace(); aTool.MoreFace(); aTool.NextFace())
     {
       Bnd_Box aBndBox = aTool.FaceBound();
       aBndBox.Enlarge (theDistance);
       if (!aBndBox.IsOut (gp_Pnt (theX, theY, theZ)))
       {
         aSurface.Initialize (aTool.GetFace());
         Handle(BRepAdaptor_HSurface) aHSurface = new BRepAdaptor_HSurface (aSurface);
         if (myFaceAlgo.Match (theX, theY, theZ, theDistance, aHSurface, theDrawer))
         {
           isContain = Standard_False;
           for (Standard_Integer anI = 1; anI <= aSeq->Length(); ++anI)
           {
             if (aSeq->Value (anI) == (aTool.GetFace()))
             {
               isContain = Standard_True;
               break;
             }
           }
           if (!isContain)
           {
             aSeq->Append (aTool.GetFace());
           }
         }
       }
     }
   }

   for (aTool.InitCurve(); aTool.MoreCurve(); aTool.NextCurve())
   {
     Bnd_Box aBndBox = aTool.CurveBound();
     aBndBox.Enlarge (theDistance);
     if (!aBndBox.IsOut (gp_Pnt (theX, theY, theZ)))
     {
       if (myCurveAlgo.Match (theX, theY, theZ, theDistance,
                              BRepAdaptor_Curve(aTool.GetCurve()), theDrawer))
       {
         Handle(TopTools_HSequenceOfShape) aSurface = aTool.FacesOfEdge();
         for (Standard_Integer anI = 1; anI <= aSurface->Length(); ++anI)
         {
           isContain = Standard_False;
           for (Standard_Integer aJ = 1; aJ <= aSeq->Length(); ++aJ)
           {
             if (aSeq->Value (aJ) == (aSurface->Value (anI)))
             {
               isContain = Standard_True;
               break;
             }
           }
           if (!isContain)
           {
             aSeq->Append (aSurface->Value (anI));
           }
         }
       }
     }
   }
   return aSeq;
}
