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

#include <StdPrs_Isolines.hxx>

#include <Adaptor3d_IsoCurve.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gce_MakeLin2d.hxx>
#include <gce_MakePln.hxx>
#include <gce_MakeLin.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomLib.hxx>
#include <GeomLib_Tool.hxx>
#include <gp_Lin2d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Hatch_Hatcher.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_List.hxx>
#include <NCollection_QuickSort.hxx>
#include <ProjLib.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <Prs3d_NListIteratorOfListOfSequenceOfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <Standard_ErrorHandler.hxx>

typedef NCollection_Sequence<Handle(TColgp_HSequenceOfPnt)> Prs3d_NSequenceOfSequenceOfPnt;

namespace
{
  const gp_Lin2d isoU (const Standard_Real theU) { return gp_Lin2d (gp_Pnt2d (theU, 0.0), gp::DY2d()); }
  const gp_Lin2d isoV (const Standard_Real theV) { return gp_Lin2d (gp_Pnt2d (0.0, theV), gp::DX2d()); }

  //! Assembles array of primitives for sequence of polyine points.
  //! @param thePoints [in] the polyline points.
  //! @return array of primitives.
  template <typename T>
  inline Handle(T) primitivesForPolyline (const Prs3d_NSequenceOfSequenceOfPnt& thePoints)
  {
    if (thePoints.IsEmpty())
    {
      return Handle(T)();
    }

    Standard_Integer aNbBounds   = thePoints.Size();
    Standard_Integer aNbVertices = 0;
    for (Prs3d_NSequenceOfSequenceOfPnt::Iterator anIt (thePoints); anIt.More(); anIt.Next())
    {
      aNbVertices += anIt.Value()->Length();
    }
    Handle(T) aPrimitives = new T (aNbVertices, aNbBounds);
    for (NCollection_Sequence<Handle(TColgp_HSequenceOfPnt)>::Iterator anIt (thePoints); anIt.More(); anIt.Next())
    {
      const Handle(TColgp_HSequenceOfPnt)& aPoints = anIt.Value();
      aPrimitives->AddBound (aPoints->Length());
      for (Standard_Integer anI = 1; anI <= aPoints->Length(); ++anI)
      {
        aPrimitives->AddVertex (aPoints->Value (anI));
      }
    }

    return aPrimitives;
  }

  //! Reoder and adjust to the limit a curve's parameter values.
  //! @param theCurve [in] the curve.
  //! @param theLimit [in] the parameter limit value.
  //! @param theFirst [in/out] the first parameter value.
  //! @param theLast  [in/out] the last parameter value.
  static void findLimits (const Adaptor3d_Curve& theCurve,
                          const Standard_Real    theLimit,
                          Standard_Real&         theFirst,
                          Standard_Real&         theLast)
  {
    theFirst = Max (theCurve.FirstParameter(), theFirst);
    theLast  = Min (theCurve.LastParameter(), theLast);

    Standard_Boolean isFirstInf = Precision::IsNegativeInfinite (theFirst);
    Standard_Boolean isLastInf  = Precision::IsPositiveInfinite (theLast);

    if (!isFirstInf && !isLastInf)
    {
      return;
    }

    gp_Pnt aP1, aP2;
    Standard_Real aDelta = 1.0;
    if (isFirstInf && isLastInf)
    {
      do
      {
        aDelta *= 2.0;
        theFirst = -aDelta;
        theLast  =  aDelta;
        theCurve.D0 (theFirst, aP1);
        theCurve.D0 (theLast,  aP2);
      }
      while (aP1.Distance (aP2) < theLimit);
    }
    else if (isFirstInf)
    {
      theCurve.D0 (theLast, aP2);
      do
      {
        aDelta *= 2.0;
        theFirst = theLast - aDelta;
        theCurve.D0 (theFirst, aP1);
      }
      while (aP1.Distance (aP2) < theLimit);
    }
    else if (isLastInf)
    {
      theCurve.D0 (theFirst, aP1);
      do
      {
        aDelta *= 2.0;
        theLast = theFirst + aDelta;
        theCurve.D0 (theLast, aP2);
      }
      while (aP1.Distance (aP2) < theLimit);
    }
  }

}

//==================================================================
// function : AddOnTriangulation
// purpose  :
//==================================================================
void StdPrs_Isolines::AddOnTriangulation (const Handle(Prs3d_Presentation)& thePresentation,
                                          const TopoDS_Face&                theFace,
                                          const Handle(Prs3d_Drawer)&       theDrawer)
{
  const Standard_Integer aNbIsoU = theDrawer->UIsoAspect()->Number();
  const Standard_Integer aNbIsoV = theDrawer->VIsoAspect()->Number();
  if (aNbIsoU < 1 && aNbIsoV < 1)
  {
    return;
  }

  // Evalute parameters for uv isolines.
  TColStd_SequenceOfReal aUIsoParams;
  TColStd_SequenceOfReal aVIsoParams;
  UVIsoParameters (theFace, aNbIsoU, aNbIsoV, theDrawer->MaximalParameterValue(), aUIsoParams, aVIsoParams);

  // Access surface definition.
  TopLoc_Location aLocSurface;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface (theFace, aLocSurface);

  // Access triangulation.
  TopLoc_Location aLocTriangulation;
  const Handle(Poly_Triangulation)& aTriangulation = BRep_Tool::Triangulation (theFace, aLocTriangulation);
  if (aTriangulation.IsNull())
  {
    return;
  }

  // Setup equal location for surface and triangulation.
  if (!aLocTriangulation.IsEqual (aLocSurface))
  {
    aSurface = Handle (Geom_Surface)::DownCast (
      aSurface->Transformed ((aLocSurface / aLocTriangulation).Transformation()));
  }

  AddOnTriangulation (thePresentation,
                      aTriangulation,
                      aSurface,
                      aLocTriangulation,
                      theDrawer,
                      aUIsoParams,
                      aVIsoParams);
}

//==================================================================
// function : AddOnTriangulation
// purpose  :
//==================================================================
void StdPrs_Isolines::AddOnTriangulation (const Handle(Prs3d_Presentation)& thePresentation,
                                          const Handle(Poly_Triangulation)& theTriangulation,
                                          const Handle(Geom_Surface)&       theSurface,
                                          const TopLoc_Location&            theLocation,
                                          const Handle(Prs3d_Drawer)&       theDrawer,
                                          const TColStd_SequenceOfReal&     theUIsoParams,
                                          const TColStd_SequenceOfReal&     theVIsoParams)
{
  const Standard_Integer aNbIsoU = theUIsoParams.Length();
  const Standard_Integer aNbIsoV = theVIsoParams.Length();

  Prs3d_NSequenceOfSequenceOfPnt aUPolylines;
  Prs3d_NSequenceOfSequenceOfPnt aVPolylines;

  const Poly_Array1OfTriangle& aTriangles = theTriangulation->Triangles();
  const TColgp_Array1OfPnt&    aNodes     = theTriangulation->Nodes();
  const TColgp_Array1OfPnt2d&  aUVNodes   = theTriangulation->UVNodes();

  TColStd_Array1OfInteger aUIsoIndexes (1, aNbIsoU);
  TColStd_Array1OfInteger aVIsoIndexes (1, aNbIsoV);
  aUIsoIndexes.Init (-1);
  aVIsoIndexes.Init (-1);

  for (Standard_Integer anI = aTriangles.Lower(); anI <= aTriangles.Upper(); ++anI)
  {
    Standard_Integer aNodeIdxs[3];
    aTriangles.Value (anI).Get (aNodeIdxs[0], aNodeIdxs[1],aNodeIdxs[2]);
    const gp_Pnt aNodesXYZ[3] = { aNodes.Value (aNodeIdxs[0]),
                                  aNodes.Value (aNodeIdxs[1]),
                                  aNodes.Value (aNodeIdxs[2]) };
    const gp_Pnt2d aNodesUV[3] = { aUVNodes.Value (aNodeIdxs[0]),
                                   aUVNodes.Value (aNodeIdxs[1]),
                                   aUVNodes.Value (aNodeIdxs[2]) };

    // Evaluate polyline points for u isolines.
    for (Standard_Integer anIsoIdx = 1; anIsoIdx <= aNbIsoU; ++anIsoIdx)
    {
      gp_Pnt aSegment[2];
      const gp_Lin2d anIsolineUV = isoU (theUIsoParams.Value (anIsoIdx));

      // Find intersections with triangle in uv space and its projection on triangulation.
      if (!findSegmentOnTriangulation (theSurface, anIsolineUV, aNodesXYZ, aNodesUV, aSegment))
      {
        continue;
      }

      if (aUIsoIndexes.Value (anIsoIdx) == -1)
      {
        aUPolylines.Append (new TColgp_HSequenceOfPnt());
        aUIsoIndexes.SetValue (anIsoIdx, aUPolylines.Size());
      }

      Handle(TColgp_HSequenceOfPnt) anIsoPnts = aUPolylines.ChangeValue (aUIsoIndexes.Value (anIsoIdx));
      anIsoPnts->Append (theLocation.IsIdentity() ? aSegment[0] : aSegment[0].Transformed (theLocation));
      anIsoPnts->Append (theLocation.IsIdentity() ? aSegment[1] : aSegment[1].Transformed (theLocation));
    }

    // Evaluate polyline points for v isolines.
    for (Standard_Integer anIsoIdx = 1; anIsoIdx <= aNbIsoV; ++anIsoIdx)
    {
      gp_Pnt aSegment[2];
      const gp_Lin2d anIsolineUV = isoV (theVIsoParams.Value (anIsoIdx));

      if (!findSegmentOnTriangulation (theSurface, anIsolineUV, aNodesXYZ, aNodesUV, aSegment))
      {
        continue;
      }

      if (aVIsoIndexes.Value (anIsoIdx) == -1)
      {
        aVPolylines.Append (new TColgp_HSequenceOfPnt());
        aVIsoIndexes.SetValue (anIsoIdx, aVPolylines.Size());
      }

      Handle(TColgp_HSequenceOfPnt) anIsoPnts = aVPolylines.ChangeValue (aVIsoIndexes.Value (anIsoIdx));
      anIsoPnts->Append (theLocation.IsIdentity() ? aSegment[0] : aSegment[0].Transformed (theLocation));
      anIsoPnts->Append (theLocation.IsIdentity() ? aSegment[1] : aSegment[1].Transformed (theLocation));
    }
  }

  // Add primitive arrays for isoline segments.
  Handle(Graphic3d_ArrayOfSegments) aUPrimitives = primitivesForPolyline<Graphic3d_ArrayOfSegments> (aUPolylines);
  Handle(Graphic3d_ArrayOfSegments) aVPrimitives = primitivesForPolyline<Graphic3d_ArrayOfSegments> (aVPolylines);

  if (!aUPrimitives.IsNull())
  {
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->UIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (aUPrimitives);
  }

  if (!aVPrimitives.IsNull())
  {
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->VIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (aVPrimitives);
  }
}

//==================================================================
// function : AddOnSurface
// purpose  :
//==================================================================
void StdPrs_Isolines::AddOnSurface (const Handle(Prs3d_Presentation)& thePresentation,
                                    const TopoDS_Face&                theFace,
                                    const Handle(Prs3d_Drawer)&       theDrawer,
                                    const Standard_Real               theDeflection)
{
  const Standard_Integer aNbIsoU = theDrawer->UIsoAspect()->Number();
  const Standard_Integer aNbIsoV = theDrawer->VIsoAspect()->Number();
  if (aNbIsoU < 1 && aNbIsoV < 1)
  {
    return;
  }

  // Evalute parameters for uv isolines.
  TColStd_SequenceOfReal aUIsoParams;
  TColStd_SequenceOfReal aVIsoParams;
  UVIsoParameters (theFace, aNbIsoU, aNbIsoV, theDrawer->MaximalParameterValue(), aUIsoParams, aVIsoParams);

  BRepAdaptor_Surface aSurface (theFace);
  AddOnSurface (thePresentation,
                new BRepAdaptor_HSurface (aSurface),
                theDrawer,
                theDeflection,
                aUIsoParams,
                aVIsoParams);
}

//==================================================================
// function : AddOnSurface
// purpose  :
//==================================================================
void StdPrs_Isolines::AddOnSurface (const Handle(Prs3d_Presentation)&   thePresentation,
                                    const Handle(BRepAdaptor_HSurface)& theSurface,
                                    const Handle(Prs3d_Drawer)&         theDrawer,
                                    const Standard_Real                 theDeflection,
                                    const TColStd_SequenceOfReal&       theUIsoParams,
                                    const TColStd_SequenceOfReal&       theVIsoParams)
{
  // Choose a deflection for sampling edge uv curves.
  Standard_Real aUVLimit = theDrawer->MaximalParameterValue();
  Standard_Real aUmin  = Max (theSurface->FirstUParameter(), -aUVLimit);
  Standard_Real aUmax  = Min (theSurface->LastUParameter(),   aUVLimit);
  Standard_Real aVmin  = Max (theSurface->FirstVParameter(), -aUVLimit);
  Standard_Real aVmax  = Min (theSurface->LastVParameter(),   aUVLimit);
  Standard_Real aSamplerDeflection = Max (aUmax - aUmin, aVmax - aVmin) * theDrawer->DeviationCoefficient();
  Standard_Real aHatchingTolerance = RealLast();
  Prs3d_NSequenceOfSequenceOfPnt aUPolylines;
  Prs3d_NSequenceOfSequenceOfPnt aVPolylines;

  try
  {
    OCC_CATCH_SIGNALS
    // Determine edge points for trimming uv hatch region.
    TColgp_SequenceOfPnt2d aTrimPoints;
    StdPrs_ToolRFace anEdgeTool (theSurface);
    for (anEdgeTool.Init(); anEdgeTool.More(); anEdgeTool.Next())
    {
      TopAbs_Orientation anOrientation = anEdgeTool.Orientation();
      if (anOrientation != TopAbs_FORWARD && anOrientation != TopAbs_REVERSED)
      {
        continue;
      }

      Adaptor2d_Curve2dPtr anEdgeCurve = anEdgeTool.Value();
      if (anEdgeCurve->GetType() != GeomAbs_Line)
      {
        GCPnts_QuasiUniformDeflection aSampler (*anEdgeCurve, aSamplerDeflection);
        if (!aSampler.IsDone())
        {
#ifdef OCCT_DEBUG
          std::cout << "Cannot evaluate curve on surface" << std::endl;
#endif
          continue;
        }

        Standard_Integer aNumberOfPoints = aSampler.NbPoints();
        if (aNumberOfPoints < 2)
        {
          continue;
        }

        for (Standard_Integer anI = 1; anI < aNumberOfPoints; ++anI)
        {
          gp_Pnt2d aP1 (aSampler.Value (anI    ).X(), aSampler.Value (anI    ).Y());
          gp_Pnt2d aP2 (aSampler.Value (anI + 1).X(), aSampler.Value (anI + 1).Y());

          aHatchingTolerance = Min (aP1.SquareDistance (aP2), aHatchingTolerance);

          aTrimPoints.Append (anOrientation == TopAbs_FORWARD ? aP1 : aP2);
          aTrimPoints.Append (anOrientation == TopAbs_FORWARD ? aP2 : aP1);
        }
      }
      else
      {
        Standard_Real aU1 = anEdgeCurve->FirstParameter();
        Standard_Real aU2 = anEdgeCurve->LastParameter();

        // MSV 17.08.06 OCC13144: U2 occured less than U1, to overcome it
        // ensure that distance U2-U1 is not greater than aLimit*2,
        // if greater then choose an origin and use aLimit to define
        // U1 and U2 anew.
        Standard_Real anOrigin = 0.0;

        if (!Precision::IsNegativeInfinite (aU1) || !Precision::IsPositiveInfinite(aU2))
        {
          if (Precision::IsNegativeInfinite (aU1))
          {
            anOrigin = aU2 - aUVLimit;
          }
          else if (Precision::IsPositiveInfinite (aU2))
          {
            anOrigin = aU1 + aUVLimit;
          }
          else
          {
            anOrigin = (aU1 + aU2) * 0.5;
          }
        }

        aU1 = Max (anOrigin - aUVLimit, aU1);
        aU2 = Min (anOrigin + aUVLimit, aU2);

        gp_Pnt2d aP1 = anEdgeCurve->Value (aU1);
        gp_Pnt2d aP2 = anEdgeCurve->Value (aU2);

        aHatchingTolerance = Min (aP1.SquareDistance(aP2), aHatchingTolerance);

        aTrimPoints.Append (anOrientation == TopAbs_FORWARD ? aP1 : aP2);
        aTrimPoints.Append (anOrientation == TopAbs_FORWARD ? aP2 : aP1);
      }
    }

    // Compute a hatching tolerance.
    aHatchingTolerance *= 0.1;
    aHatchingTolerance = Max (Precision::Confusion(), aHatchingTolerance);
    aHatchingTolerance = Min (1.0E-5, aHatchingTolerance);

    // Load isolines into hatcher.
    Hatch_Hatcher aHatcher (aHatchingTolerance, anEdgeTool.IsOriented());

    for (Standard_Integer anIso = 1; anIso <= theUIsoParams.Length(); ++anIso)
    {
      aHatcher.AddXLine (theUIsoParams.Value (anIso));
    }
    for (Standard_Integer anIso = 1; anIso <= theVIsoParams.Length(); ++anIso)
    {
      aHatcher.AddYLine (theVIsoParams.Value (anIso));
    }

    // Trim hatching region.
    for (Standard_Integer anI = 1; anI <= aTrimPoints.Length(); anI += 2)
    {
      aHatcher.Trim (aTrimPoints (anI), aTrimPoints (anI + 1));
    }

    // Use surface definition for evaluation of Bezier, B-spline surface.
    // Use isoline adapter for other types of surfaces.
    GeomAbs_SurfaceType  aSurfType = theSurface->GetType();
    Handle(Geom_Surface) aBSurface;
    GeomAdaptor_Curve    aBSurfaceCurve;
    Adaptor3d_IsoCurve   aCanonicalCurve;
    if (aSurfType == GeomAbs_BezierSurface)
    {
      aBSurface = theSurface->Bezier();
    }
    else if (aSurfType == GeomAbs_BSplineSurface)
    {
      aBSurface = theSurface->BSpline();
    }
    else
    {
      aCanonicalCurve.Load (theSurface);
    }

    // For each isoline: compute its segments.
    for (Standard_Integer anI = 1; anI <= aHatcher.NbLines(); anI++)
    {
      Standard_Real anIsoParam = aHatcher.Coordinate (anI);
      Standard_Boolean isIsoU  = aHatcher.IsXLine (anI);

      // For each isoline's segment: evaluate its points.
      for (Standard_Integer aJ = 1; aJ <= aHatcher.NbIntervals (anI); aJ++)
      {
        Standard_Real aSegmentP1 = aHatcher.Start (anI, aJ);
        Standard_Real aSegmentP2 = aHatcher.End (anI, aJ);

        if (!aBSurface.IsNull())
        {
          aBSurfaceCurve.Load (isIsoU ? aBSurface->UIso (anIsoParam) : aBSurface->VIso (anIsoParam));

          findLimits (aBSurfaceCurve, aUVLimit, aSegmentP1, aSegmentP2);

          if (aSegmentP2 - aSegmentP1 <= Precision::Confusion())
          {
            continue;
          }
        }
        else
        {
          aCanonicalCurve.Load (isIsoU ? GeomAbs_IsoU : GeomAbs_IsoV, anIsoParam, aSegmentP1, aSegmentP2);

          findLimits (aCanonicalCurve, aUVLimit, aSegmentP1, aSegmentP2);

          if (aSegmentP2 - aSegmentP1 <= Precision::Confusion())
          {
            continue;
          }
        }
        Adaptor3d_Curve* aCurve = aBSurface.IsNull() ? (Adaptor3d_Curve*) &aCanonicalCurve
          : (Adaptor3d_Curve*) &aBSurfaceCurve;

        Handle(TColgp_HSequenceOfPnt) aPoints = new TColgp_HSequenceOfPnt();
        StdPrs_DeflectionCurve::Add (thePresentation,
                                     *aCurve,
                                     aSegmentP1,
                                     aSegmentP2,
                                     theDeflection,
                                     aPoints->ChangeSequence(),
                                     theDrawer->DeviationAngle(),
                                     Standard_False);
        if (aPoints->IsEmpty())
        {
          continue;
        }

        if (isIsoU)
        {
          aUPolylines.Append (aPoints);
        }
        else
        {
          aVPolylines.Append (aPoints);
        }
      }
    }
  }
  catch (Standard_Failure)
  {
    // ...
  }

  // Add primitive arrays for isoline segments.
  Handle(Graphic3d_ArrayOfPolylines) aUPrimitives = primitivesForPolyline<Graphic3d_ArrayOfPolylines> (aUPolylines);
  Handle(Graphic3d_ArrayOfPolylines) aVPrimitives = primitivesForPolyline<Graphic3d_ArrayOfPolylines> (aVPolylines);

  if (!aUPrimitives.IsNull())
  {
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->UIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (aUPrimitives);
  }

  if (!aVPrimitives.IsNull())
  {
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);
    aGroup->SetPrimitivesAspect (theDrawer->VIsoAspect()->Aspect());
    aGroup->AddPrimitiveArray (aVPrimitives);
  }
}

//==================================================================
// function : UVIsoParameters
// purpose  :
//==================================================================
void StdPrs_Isolines::UVIsoParameters (const TopoDS_Face&      theFace,
                                       const Standard_Integer  theNbIsoU,
                                       const Standard_Integer  theNbIsoV,
                                       const Standard_Real     theUVLimit,
                                       TColStd_SequenceOfReal& theUIsoParams,
                                       TColStd_SequenceOfReal& theVIsoParams)
{
  Standard_Real aUmin = 0.0;
  Standard_Real aUmax = 0.0;
  Standard_Real aVmin = 0.0;
  Standard_Real aVmax = 0.0;

  BRepTools::UVBounds (theFace, aUmin, aUmax, aVmin, aVmax);

  aUmin = Max (aUmin, -theUVLimit);
  aUmax = Min (aUmax,  theUVLimit);
  aVmin = Max (aVmin, -theUVLimit);
  aVmax = Min (aVmax,  theUVLimit);

  TopLoc_Location aLocation;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface (theFace, aLocation);

  const Standard_Boolean isUClosed = aSurface->IsUClosed();
  const Standard_Boolean isVClosed = aSurface->IsVClosed();

  if (!isUClosed)
  {
    aUmin = aUmin + (aUmax - aUmin) / 1000.0;
    aUmax = aUmax - (aUmax - aUmin) / 1000.0;
  }

  if (!isVClosed)
  {
    aVmin = aVmin + (aVmax - aVmin) / 1000.0;
    aVmax = aVmax - (aVmax - aVmin) / 1000.0;
  }

  Standard_Real aUstep = (aUmax - aUmin) / (1 + theNbIsoU);
  Standard_Real aVstep = (aVmax - aVmin) / (1 + theNbIsoV);

  for (Standard_Integer anIso = 1; anIso <= theNbIsoU; ++anIso)
  {
    theUIsoParams.Append (aUmin + aUstep * anIso);
  }

  for (Standard_Integer anIso = 1; anIso <= theNbIsoV; ++anIso)
  {
    theVIsoParams.Append (aVmin + aVstep * anIso);
  }
}

//==================================================================
// function : FindSegmentOnTriangulation
// purpose  :
//==================================================================
Standard_Boolean StdPrs_Isolines::findSegmentOnTriangulation (const Handle(Geom_Surface)& theSurface,
                                                              const gp_Lin2d&             theIsoline,
                                                              const gp_Pnt*               theNodesXYZ,
                                                              const gp_Pnt2d*             theNodesUV,
                                                              gp_Pnt*                     theSegment)
{
  Standard_Integer aNPoints = 0;

  for (Standard_Integer aLinkIter = 0; aLinkIter < 3 && aNPoints < 2; ++aLinkIter)
  {
    // ...
    // Check that uv isoline crosses the triangulation link in parametric space
    // ...

    const gp_Pnt2d& aNodeUV1 = theNodesUV[aLinkIter];
    const gp_Pnt2d& aNodeUV2 = theNodesUV[(aLinkIter + 1) % 3];
    const gp_Pnt& aNode1 = theNodesXYZ[aLinkIter];
    const gp_Pnt& aNode2 = theNodesXYZ[(aLinkIter + 1) % 3];

    // Compute distance of uv points to isoline taking into consideration their relative
    // location against the isoline (left or right). Null value for a node means that the
    // isoline crosses the node. Both positive or negative means that the isoline does not
    // cross the segment.
    Standard_Boolean isLeftUV1 = (theIsoline.Direction().XY() ^ gp_Vec2d (theIsoline.Location(), aNodeUV1).XY()) > 0.0;
    Standard_Boolean isLeftUV2 = (theIsoline.Direction().XY() ^ gp_Vec2d (theIsoline.Location(), aNodeUV2).XY()) > 0.0;
    Standard_Real aDistanceUV1 = isLeftUV1 ? theIsoline.Distance (aNodeUV1) : -theIsoline.Distance (aNodeUV1);
    Standard_Real aDistanceUV2 = isLeftUV2 ? theIsoline.Distance (aNodeUV2) : -theIsoline.Distance (aNodeUV2);

    // Isoline crosses first point of an edge.
    if (Abs (aDistanceUV1) < Precision::PConfusion())
    {
      theSegment[aNPoints++] = aNode1;
      continue;
    }

    // Isoline crosses second point of an edge.
    if (Abs (aDistanceUV2) < Precision::PConfusion())
    {
      theSegment[aNPoints++] = aNode2;
      aLinkIter++;
      continue;
    }

    // Isoline does not cross the triangle link.
    if (aDistanceUV1 * aDistanceUV2 > 0.0)
    {
      continue;
    }

    // Isoline crosses degenerated link.
    if (aNode1.SquareDistance (aNode2) < Precision::PConfusion())
    {
      theSegment[aNPoints++] = aNode1;
      continue;
    }

    // ...
    // Derive cross-point from parametric coordinates
    // ...

    Standard_Real anAlpha = Abs (aDistanceUV1) / (Abs (aDistanceUV1) + Abs (aDistanceUV2));

    gp_Pnt aCross (0.0, 0.0, 0.0);

    // Is surface definition available?
    if (theSurface.IsNull())
    {
      // Do linear interpolation of point coordinates using
      // triangulation nodes.
      aCross.SetX (aNode1.X() + anAlpha * (aNode2.X() - aNode1.X()));
      aCross.SetY (aNode1.Y() + anAlpha * (aNode2.Y() - aNode1.Y()));
      aCross.SetZ (aNode1.Z() + anAlpha * (aNode2.Z() - aNode1.Z()));
    }
    else
    {
      // Do linear interpolation of point coordinates by triangulation nodes.
      Standard_Real aCrossU = aNodeUV1.X() + anAlpha * (aNodeUV2.X() - aNodeUV1.X());
      Standard_Real aCrossV = aNodeUV1.Y() + anAlpha * (aNodeUV2.Y() - aNodeUV1.Y());

      // Get 3d point on surface.
      Handle(Geom_Curve) anIso1, anIso2, anIso3;
      Standard_Real aPntOnNode1Iso = 0.0;
      Standard_Real aPntOnNode2Iso = 0.0;
      Standard_Real aPntOnNode3Iso = 0.0;

      if (theIsoline.Direction().X() == 0.0)
      {
        aPntOnNode1Iso = aNodeUV1.X();
        aPntOnNode2Iso = aNodeUV2.X();
        aPntOnNode3Iso = aCrossU;
        anIso1 = theSurface->VIso (aNodeUV1.Y());
        anIso2 = theSurface->VIso (aNodeUV2.Y());
        anIso3 = theSurface->VIso (aCrossV);
      }
      else if (theIsoline.Direction().Y() == 0.0)
      {
        aPntOnNode1Iso = aNodeUV1.Y();
        aPntOnNode2Iso = aNodeUV2.Y();
        aPntOnNode3Iso = aCrossV;
        anIso1 = theSurface->UIso (aNodeUV1.X());
        anIso2 = theSurface->UIso (aNodeUV2.X());
        anIso3 = theSurface->UIso (aCrossU);
      }

      GeomAdaptor_Curve aCurveAdaptor1 (anIso1);
      GeomAdaptor_Curve aCurveAdaptor2 (anIso2);
      GeomAdaptor_Curve aCurveAdaptor3 (anIso3);
      Standard_Real aLength1 = GCPnts_AbscissaPoint::Length (aCurveAdaptor1, aPntOnNode1Iso, aPntOnNode3Iso, 1e-2);
      Standard_Real aLength2 = GCPnts_AbscissaPoint::Length (aCurveAdaptor2, aPntOnNode2Iso, aPntOnNode3Iso, 1e-2);
      if (Abs (aLength1) < Precision::Confusion() || Abs (aLength2) < Precision::Confusion())
      {
        theSegment[aNPoints++] = (aNode2.XYZ() - aNode1.XYZ()) * anAlpha + aNode1.XYZ();
        continue;
      }

      aCross = (aNode2.XYZ() - aNode1.XYZ()) * (aLength1 / (aLength1 + aLength2)) + aNode1.XYZ();
    }

    theSegment[aNPoints++] = aCross;
  }

  return aNPoints == 2;
}
