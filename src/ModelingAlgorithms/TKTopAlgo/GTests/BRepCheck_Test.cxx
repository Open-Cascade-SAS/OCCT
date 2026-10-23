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

#include <gtest/gtest.h>
#include <NCollection_Array2.hxx>
#include <Geom_BSplineSurface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Status.hxx>
#include <Geom_OffsetCurve.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_List.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <GeomConvert.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <array>
#include <thread>
#include <atomic>
#include <cmath>
#include <utility>

TEST(BRepCheckTest, PlaneWithoutPCurveIsIndependentOfSurfaceOrigin)
{
  constexpr double                               aSlope = 1.0e-6;
  const gp_Dir                                   aNormal(0.0, aSlope, 1.0);
  const gp_Dir                                   anXAxis(0.0, -1.0, aSlope);
  const double                                   aTolerance    = Precision::Confusion();
  const std::array<std::pair<double, double>, 6> aGapsAndRises = {
    {{0.0, 0.0},
     {0.5 * aTolerance, 0.0},
     {2.0 * aTolerance, 0.0},
     {0.0, 2.0 * aTolerance},
     {2.0 * aTolerance, -2.0 * aTolerance},
     {-2.0 * aTolerance, 2.0 * aTolerance}}};
  for (const double anOrigin : {0.0, 1.0e9, -1.0e9, 1.0e10, -1.0e10})
  {
    for (const auto& [aGap, aRise] : aGapsAndRises)
    {
      for (const bool isExact : {false, true})
      {
        SCOPED_TRACE(anOrigin);
        SCOPED_TRACE(aGap);
        SCOPED_TRACE(aRise);
        SCOPED_TRACE(isExact);
        const occ::handle<Geom_Plane> aPlane =
          new Geom_Plane(gp_Ax3(gp_Pnt(0.0, anOrigin, 0.0), aNormal, anXAxis));
        const occ::handle<Geom_Line> aLine =
          new Geom_Line(gp_Pnt(0.0, 0.0, anOrigin * aSlope + aGap / aNormal.Z()),
                        gp_Dir(1.0, 0.0, aRise / aNormal.Z()));
        BRepBuilderAPI_MakeEdge aMaker(aLine, 0.0, 1.0);
        ASSERT_TRUE(aMaker.IsDone());
        const TopoDS_Edge anEdge = aMaker.Edge();
        BRep_Builder      aBuilder;
        TopoDS_Face       aFace;
        aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
        TopoDS_Wire aWire;
        aBuilder.MakeWire(aWire);
        aBuilder.Add(aWire, anEdge);
        aBuilder.Add(aFace, aWire);
        BRepCheck_Edge aChecker(anEdge);
        aChecker.SetExactMethod(isExact);
        aChecker.InContext(aFace);
        const NCollection_List<BRepCheck_Status>& aStatus = aChecker.StatusOnShape(aFace);
        ASSERT_EQ(aStatus.Size(), 1u);
        EXPECT_EQ(aStatus.First(),
                  std::max(std::abs(aGap), std::abs(aGap + aRise)) < aTolerance
                    ? BRepCheck_NoError
                    : BRepCheck_InvalidCurveOnSurface);
      }
    }
  }
}

TEST(BRepCheckTest, StoredPCurveOnPlaneRetainsTangentialDistanceCheck)
{
  const double                  aTolerance = Precision::Confusion();
  const occ::handle<Geom_Line>  aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)));
  BRepBuilderAPI_MakeEdge aMaker(aLine, 0.0, 1.0);
  ASSERT_TRUE(aMaker.IsDone());
  const TopoDS_Edge anEdge = aMaker.Edge();
  BRep_Builder      aBuilder;
  TopoDS_Face       aFace;
  aBuilder.MakeFace(aFace, aPlane, aTolerance);
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, anEdge);
  aBuilder.Add(aFace, aWire);
  aBuilder.UpdateEdge(anEdge,
                      new Geom2d_Line(gp_Pnt2d(0.0, 2.0 * aTolerance), gp_Dir2d(1.0, 0.0)),
                      aFace,
                      aTolerance);
  for (const bool isExact : {false, true})
  {
    SCOPED_TRACE(isExact);
    BRepCheck_Edge aChecker(anEdge);
    aChecker.SetExactMethod(isExact);
    aChecker.InContext(aFace);
    const NCollection_List<BRepCheck_Status>& aStatus = aChecker.StatusOnShape(aFace);
    EXPECT_NE(std::find(aStatus.begin(), aStatus.end(), BRepCheck_InvalidCurveOnSurface),
              aStatus.end());
  }
}

TEST(BRepCheckTest, ExactPlaneDistanceIsIndependentOfParameterOrigin)
{
  constexpr double aLargeCoordinate = 281474976710656.0;
  for (const double anOrigin : {0.0, aLargeCoordinate, -aLargeCoordinate})
  {
    for (const double aGap : {0.0, 0.01})
    {
      for (const bool isRational : {false, true})
      {
        for (const bool isBSpline : {false, true})
        {
          SCOPED_TRACE(anOrigin);
          SCOPED_TRACE(aGap);
          SCOPED_TRACE(isRational);
          SCOPED_TRACE(isBSpline);
          NCollection_Array1<gp_Pnt> aPoles(1, 2);
          aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, aGap);
          aPoles.ChangeAt(1) = gp_Pnt(1.0, 0.0, aGap);
          NCollection_Array1<double> aWeights(1, 2);
          aWeights(1) = 1.0;
          aWeights(2) = 2.0;
          const occ::handle<Geom_BezierCurve> aBezier =
            isRational ? new Geom_BezierCurve(aPoles, aWeights) : new Geom_BezierCurve(aPoles);
          NCollection_Array1<gp_Pnt2d> aPoles2d(1, 2);
          for (size_t anIndex = 0; anIndex < aPoles2d.Size(); ++anIndex)
          {
            aPoles2d.ChangeAt(anIndex) = gp_Pnt2d(-anOrigin + double(anIndex), 0.0);
          }
          const occ::handle<Geom2d_BezierCurve> aBezier2d =
            isRational ? new Geom2d_BezierCurve(aPoles2d, aWeights)
                       : new Geom2d_BezierCurve(aPoles2d);
          const occ::handle<Geom_Curve>   aCurve = isBSpline
                                                     ? GeomConvert::CurveToBSplineCurve(aBezier)
                                                     : occ::handle<Geom_Curve>(aBezier);
          const occ::handle<Geom2d_Curve> aPCurve =
            isBSpline ? Geom2dConvert::CurveToBSplineCurve(aBezier2d)
                      : occ::handle<Geom2d_Curve>(aBezier2d);
          const occ::handle<Geom_Plane> aPlane =
            new Geom_Plane(gp_Ax3(gp_Pnt(anOrigin, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
          const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
          const occ::handle<Adaptor3d_CurveOnSurface> anOther =
            new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve, 0.0, 1.0),
                                         new GeomAdaptor_Surface(aPlane));
          BRepLib_ValidateEdge aValidator(aReference, anOther, true);
          aValidator.SetExactMethod(true);
          aValidator.Process();
          ASSERT_TRUE(aValidator.IsDone());
          EXPECT_EQ(aValidator.CheckTolerance(Precision::Confusion()), aGap == 0.0);
          aValidator.SetExitIfToleranceExceeded(Precision::Confusion());
          aValidator.Process();
          ASSERT_TRUE(aValidator.IsDone());
          EXPECT_EQ(aValidator.CheckTolerance(Precision::Confusion()), aGap == 0.0);
          GeomLib_CheckCurveOnSurface aDistance(aReference);
          aDistance.Perform(anOther);
          ASSERT_TRUE(aDistance.IsDone());
          EXPECT_NEAR(aDistance.MaxDistance(), aGap, RealEpsilon());
        }
      }
    }
  }
}

TEST(BRepCheckTest, SplineToleranceDoesNotHideRepresentableGap)
{
  constexpr double aLargeCoordinate      = 65537.0;
  constexpr double aGap                  = 0.25;
  const double     aCoordinateResolution = Epsilon(aLargeCoordinate);
  const double     aBorderlineTolerance  = aGap - 0.5 * aCoordinateResolution;
  const double     aSmallerTolerance     = aGap - 2.0 * aCoordinateResolution;
  for (const double aCoordinate : std::array<double, 3>{0.0, aLargeCoordinate, -aLargeCoordinate})
  {
    for (const bool isBSpline : std::array<bool, 2>{false, true})
    {
      SCOPED_TRACE(aCoordinate);
      SCOPED_TRACE(isBSpline);
      NCollection_Array1<gp_Pnt> aPoles(1, 2);
      aPoles(1) = gp_Pnt(aCoordinate + aGap, 0.0, 0.0);
      aPoles(2) = gp_Pnt(aCoordinate + aGap, 1.0, 0.0);
      occ::handle<Geom_Curve> aCurve;
      if (isBSpline)
      {
        NCollection_Array1<double> aKnots(1, 2);
        NCollection_Array1<int>    aMultiplicities(1, 2);
        aKnots(1) = 0.0;
        aKnots(2) = 1.0;
        aMultiplicities.Init(2);
        aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
      }
      else
      {
        aCurve = new Geom_BezierCurve(aPoles);
      }
      const occ::handle<Geom2d_Line> aPCurve =
        new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
      const occ::handle<Geom_Plane> aPlane = new Geom_Plane(
        gp_Ax3(gp_Pnt(aCoordinate, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)));
      const occ::handle<Adaptor3d_Curve>          aReference = new GeomAdaptor_Curve(aCurve);
      const occ::handle<Adaptor3d_CurveOnSurface> anOther =
        new Adaptor3d_CurveOnSurface(new Geom2dAdaptor_Curve(aPCurve, 0.0, 1.0),
                                     new GeomAdaptor_Surface(aPlane));

      for (const bool isExact : std::array<bool, 2>{false, true})
      {
        SCOPED_TRACE(isExact);
        BRepLib_ValidateEdge aValidator(aReference, anOther, true);
        aValidator.SetExactMethod(isExact);
        aValidator.Process();

        ASSERT_TRUE(aValidator.IsDone());
        EXPECT_FALSE(aValidator.CheckTolerance(aBorderlineTolerance));
        EXPECT_FALSE(aValidator.CheckTolerance(aSmallerTolerance));
        EXPECT_TRUE(aValidator.CheckTolerance(aGap + aCoordinateResolution));
        for (const double aTolerance : {aBorderlineTolerance, aGap + aCoordinateResolution})
        {
          aValidator.SetExitIfToleranceExceeded(aTolerance);
          aValidator.Process();
          ASSERT_TRUE(aValidator.IsDone());
          EXPECT_EQ(aValidator.CheckTolerance(aTolerance), aTolerance > aGap);
        }
      }
    }
  }
}

TEST(BRepCheckTest, PlanarOffsetWithoutStoredPCurve)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt(2.0, 2.0, 0.0);
  aPoles.ChangeAt(3) = gp_Pnt(3.0, 0.0, 0.0);
  const occ::handle<Geom_OffsetCurve> aCurve =
    new Geom_OffsetCurve(new Geom_BezierCurve(aPoles), 0.2, gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeEdge anEdgeMaker(aCurve);
  ASSERT_TRUE(anEdgeMaker.IsDone());
  const TopoDS_Edge             anEdge = anEdgeMaker.Edge();
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  TopoDS_Face  aFace;
  BRep_Builder aBuilder;
  aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
  TopoDS_Wire aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, anEdge);
  aBuilder.Add(aFace, aWire);
  double                          aFirst = 0.0, aLast = 0.0;
  const occ::handle<Geom2d_Curve> aPCurve =
    BRep_Tool::CurveOnPlane(anEdge, aPlane, TopLoc_Location(), aFirst, aLast);
  ASSERT_FALSE(aPCurve.IsNull());
  const GeomAdaptor_Curve   aReference(aCurve, aFirst, aLast);
  const Geom2dAdaptor_Curve aProjected(aPCurve, aFirst, aLast);
  constexpr int             aNbSamples = 64;
  for (int anIndex = 0; anIndex <= aNbSamples; ++anIndex)
  {
    const double   aParameter = aFirst + (aLast - aFirst) * double(anIndex) / aNbSamples;
    const gp_Pnt2d aUV        = aProjected.EvalD0(aParameter);
    EXPECT_LE(aReference.EvalD0(aParameter).Distance(aPlane->EvalD0(aUV.X(), aUV.Y())),
              Precision::PConfusion());
  }
  BRepCheck_Edge aChecker(anEdge);
  aChecker.InContext(aFace);
  const NCollection_List<BRepCheck_Status>& aStatus = aChecker.StatusOnShape(aFace);
  ASSERT_EQ(aStatus.Size(), 1u);
  EXPECT_EQ(aStatus.First(), BRepCheck_NoError);
}

//==================================================================================================

TEST(BRepCheckTest, ParallelFaceChecksMatchSerialForSharedBSplineEdge)
{
  constexpr int              aDegree     = 3;
  constexpr int              aNbSpans    = 32;
  constexpr int              aNbContexts = 64;
  NCollection_Array1<double> aKnots(aNbSpans + 1);
  NCollection_Array1<int>    aMultiplicities(aKnots.Size());
  for (int i = 0; i <= aNbSpans; ++i)
  {
    aKnots.ChangeAt(i)          = i;
    aMultiplicities.ChangeAt(i) = i == 0 || i == aNbSpans ? aDegree + 1 : 1;
  }
  NCollection_Array1<gp_Pnt> aPoles(aNbSpans + aDegree);
  for (size_t i = 0; i < aPoles.Size(); ++i)
    aPoles.ChangeAt(i) = gp_Pnt(0.25 * i, 0.5 * (i % 4), 0.0);
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, aDegree);
  BRepBuilderAPI_MakeEdge aMakeEdge(aCurve);
  ASSERT_TRUE(aMakeEdge.IsDone());
  const TopoDS_Edge               anEdge = aMakeEdge.Edge();
  BRep_Builder                    aBuilder;
  NCollection_Array1<TopoDS_Face> aFaces(aNbContexts);
  const occ::handle<Geom2d_Line>  aPCurve = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  NCollection_Array1<double>      aVKnots(2);
  aVKnots.ChangeAt(0)                                    = 0.0;
  aVKnots.ChangeAt(1)                                    = 1.0;
  constexpr std::array<int, 2>              aVDegrees    = {1, 6};
  std::array<NCollection_Array2<gp_Pnt>, 2> aPoleBuffers = {
    NCollection_Array2<gp_Pnt>(aPoles.Size(), aVDegrees[0] + 1),
    NCollection_Array2<gp_Pnt>(aPoles.Size(), aVDegrees[1] + 1)};
  NCollection_Array1<int> aVMults(2);
  for (int i = 0; i < aNbContexts; ++i)
  {
    const size_t aBuffer  = static_cast<size_t>(i) % aVDegrees.size();
    const int    aVDegree = aVDegrees[aBuffer];
    aVMults.Init(aVDegree + 1);
    NCollection_Array2<gp_Pnt>& aSurfacePoles = aPoleBuffers[aBuffer];
    for (size_t u = 0; u < aPoles.Size(); ++u)
      for (int v = 0; v <= aVDegree; ++v)
      {
        const gp_Pnt& aPoint         = aPoles.At(u);
        aSurfacePoles.ChangeAt(u, v) = gp_Pnt(aPoint.X(), aPoint.Y(), (i + 1.0) * v / aVDegree);
      }
    const occ::handle<Geom_BSplineSurface> aSurface = new Geom_BSplineSurface(aSurfacePoles,
                                                                              aKnots,
                                                                              aVKnots,
                                                                              aMultiplicities,
                                                                              aVMults,
                                                                              aDegree,
                                                                              aVDegree);
    TopoDS_Face&                           aFace    = aFaces.ChangeAt(i);
    aBuilder.MakeFace(aFace, aSurface, Precision::Confusion());
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    aBuilder.Add(aWire, anEdge);
    aBuilder.Add(aFace, aWire);
    aBuilder.UpdateEdge(anEdge, aPCurve, aFace, Precision::Confusion());
    aBuilder.Range(anEdge, aFace, 0.0, aNbSpans);
  }
  aBuilder.SameParameter(anEdge, true);
  aBuilder.SameRange(anEdge, true);

  BRepCheck_Edge aSerial(anEdge);
  for (const TopoDS_Face& aFace : aFaces)
  {
    aSerial.InContext(aFace);
    const NCollection_List<BRepCheck_Status>& aStatus = aSerial.StatusOnShape(aFace);
    ASSERT_EQ(aStatus.Size(), 1u);
    ASSERT_EQ(aStatus.First(), BRepCheck_NoError);
  }

  BRepCheck_Edge aCheck(anEdge);
  aCheck.SetParallel(true);
  std::atomic<bool>          aStart(false);
  std::array<std::thread, 2> aThreads;
  for (size_t i = 0; i < aThreads.size(); ++i)
  {
    aThreads[i] = std::thread([&, i] {
      while (!aStart.load(std::memory_order_acquire))
        std::this_thread::yield();
      for (size_t j = i; j < aFaces.Size(); j += aThreads.size())
        aCheck.InContext(aFaces.At(j));
    });
  }
  aStart.store(true, std::memory_order_release);
  for (std::thread& aThread : aThreads)
    aThread.join();
  for (size_t i = 0; i < aFaces.Size(); ++i)
  {
    const NCollection_List<BRepCheck_Status>& aStatus = aCheck.StatusOnShape(aFaces.At(i));
    ASSERT_EQ(aStatus.Size(), 1u) << i;
    EXPECT_EQ(aStatus.First(), BRepCheck_NoError) << i;
  }
}

TEST(BRepCheckTest, PlaneWithoutPCurveUsesAccurateOffsetProjection)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt(2.0, -2.0, 0.0);
  aPoles.ChangeAt(3) = gp_Pnt(3.0, 0.0, 0.0);
  const occ::handle<Geom_Curve> aCurve =
    new Geom_OffsetCurve(new Geom_BezierCurve(aPoles), 0.1, gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeEdge aMaker(aCurve, 0.01, 0.99);
  ASSERT_TRUE(aMaker.IsDone());
  const TopoDS_Edge anEdge = aMaker.Edge();
  for (const double aPlaneHeight : {0.0, 1.0e-5})
  {
    for (const bool isExact : {false, true})
    {
      SCOPED_TRACE(aPlaneHeight);
      SCOPED_TRACE(isExact);
      const occ::handle<Geom_Plane> aPlane =
        new Geom_Plane(gp_Pnt(0.0, 0.0, aPlaneHeight), gp_Dir(1.0e-16, 0.0, 1.0));
      BRep_Builder aBuilder;
      TopoDS_Face  aFace;
      aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
      TopoDS_Wire aWire;
      aBuilder.MakeWire(aWire);
      aBuilder.Add(aWire, anEdge);
      aBuilder.Add(aFace, aWire);
      BRepCheck_Edge aChecker(anEdge);
      aChecker.SetExactMethod(isExact);
      aChecker.InContext(aFace);
      const NCollection_List<BRepCheck_Status>& aStatus = aChecker.StatusOnShape(aFace);
      ASSERT_EQ(aStatus.Size(), 1u);
      EXPECT_EQ(aStatus.First(),
                aPlaneHeight == 0.0 ? BRepCheck_NoError : BRepCheck_InvalidCurveOnSurface);
    }
  }
}

TEST(BRepCheckTest, PlaneWithoutPCurveAtOffsetCurvatureJump)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles.ChangeAt(0) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt(2.0, 0.0, 0.0);
  aPoles.ChangeAt(3) = gp_Pnt(3.0, -2.0, 0.0);
  aPoles.ChangeAt(4) = gp_Pnt(4.0, 0.0, 0.0);
  aPoles.ChangeAt(5) = gp_Pnt(5.0, 0.0, 0.0);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(2) = 1.0;
  NCollection_Array1<int> aMults(1, 3);
  aMults.ChangeAt(0) = 4;
  aMults.ChangeAt(1) = 2;
  aMults.ChangeAt(2) = 4;
  for (const double aJoin : {0.1, 0.5, 0.9})
  {
    SCOPED_TRACE(aJoin);
    aKnots.ChangeAt(1) = aJoin;
    const occ::handle<Geom_Curve> aCurve =
      new Geom_OffsetCurve(new Geom_BSplineCurve(aPoles, aKnots, aMults, 3),
                           0.1,
                           gp_Dir(0.0, 0.0, 1.0));
    BRepBuilderAPI_MakeEdge aMaker(aCurve, 0.01, 0.99);
    ASSERT_TRUE(aMaker.IsDone());
    const TopoDS_Edge             anEdge = aMaker.Edge();
    const occ::handle<Geom_Plane> aPlane =
      new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0e-16, 0.0, 1.0));
    BRep_Builder aBuilder;
    TopoDS_Face  aFace;
    aBuilder.MakeFace(aFace, aPlane, Precision::Confusion());
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    aBuilder.Add(aWire, anEdge);
    aBuilder.Add(aFace, aWire);
    BRepCheck_Edge aChecker(anEdge);
    aChecker.SetExactMethod(true);
    aChecker.InContext(aFace);
    const NCollection_List<BRepCheck_Status>& aStatus = aChecker.StatusOnShape(aFace);
    ASSERT_EQ(aStatus.Size(), 1u);
    EXPECT_EQ(aStatus.First(), BRepCheck_NoError);
  }
}
