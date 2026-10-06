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
#include <ElSLib.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <IntPatch_TheIWalking.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntWalk_PWalking.hxx>
#include <Geom_BezierSurface.hxx>
#include <NCollection_Array2.hxx>
#include <IntTools_FaceFace.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS_Face.hxx>
#include <algorithm>
#include <cmath>
#include <Standard_Real.hxx>
#include <IntPatch_TheSurfFunction.hxx>
#include <IntSurf_PathPoint.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntImp_ComputeTangence.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <IntWalk_TheInt2S.hxx>

TEST(IntPatch_WalkingTest, RegularSeedPreservesFixedParameterAfterTangencyProbe)
{
  const occ::handle<Adaptor3d_Surface> aFirst =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3(gp::Origin(), gp::DZ(), gp::DX())));
  const occ::handle<Adaptor3d_Surface> aSecond =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX())));
  IntWalk_TheInt2S           aIntersection(aFirst, aSecond, Precision::Confusion());
  math_FunctionSetRoot       aSolver(aIntersection.Function());
  NCollection_Array1<double> aSeed(1, 4);
  aSeed(1) = 2.0;
  aSeed(2) = 0.0;
  aSeed(3) = -3.0;
  aSeed(4) = 0.0;

  // The tangency probe prefers U on the first plane; the requested fixed U is on the second.
  ASSERT_NO_THROW(aIntersection.Perform(aSeed, aSolver, IntImp_UIsoparametricOnCaro2));
  ASSERT_TRUE(aIntersection.IsDone());
  ASSERT_FALSE(aIntersection.IsEmpty());
  EXPECT_FALSE(aIntersection.IsTangent());
  EXPECT_LE(aIntersection.Point().Value().Distance(gp_Pnt(2.0, 0.0, 0.0)), Precision::Confusion());
  double aU1, aV1, aU2, aV2;
  aIntersection.Point().Parameters(aU1, aV1, aU2, aV2);
  EXPECT_DOUBLE_EQ(aU2, aSeed(3));
}

TEST(IntPatch_WalkingTest, ReachesBoundaryAtRegularAndSingularPoints)
{
  for (double aScale : {0.01, 1.0, 100.0})
  {
    SCOPED_TRACE(aScale);
    for (double anOffset : {0.0, 1.e-6, 0.1})
    {
      SCOPED_TRACE(anOffset);
      for (double aU : {0.0, M_PI})
      {
        SCOPED_TRACE(aU);
        const double aMajor = aScale * 91.4695099096445, aMinor = aScale * 104.341168003437;
        const double aLower = 2 * M_PI - std::acos(-aMajor / aMinor) + anOffset;
        const double aStart = aLower + 0.5;
        const occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(
          gp_Torus(gp_Ax3(gp::Origin(), gp::DX(), gp::DY()), aMajor, aMinor));
        occ::handle<Adaptor3d_Surface> aSurface =
          new GeomAdaptor_Surface(aTorus, -0.1, 2 * M_PI + 0.1, aLower, aStart);
        IntSurf_Quadric          aPlane(gp_Pln(gp::Origin(), gp::DZ()));
        IntPatch_TheSurfFunction aFunction(aSurface, aPlane);
        aFunction.Set(1.e-7);
        math_Vector aUV(1, 2), aValue(1, 1);
        aUV(1) = aU;
        aUV(2) = aStart;
        ASSERT_TRUE(aFunction.Value(aUV, aValue));
        ASSERT_FALSE(aFunction.IsTangent());
        IntSurf_PathPoint aPath(aFunction.Point(), aU, aStart);
        if (aFunction.Direction2d().Y() > 0)
        {
          aPath.SetDirections(-aFunction.Direction3d(), aFunction.Direction2d().Reversed());
        }
        else
        {
          aPath.SetDirections(aFunction.Direction3d(), aFunction.Direction2d());
        }
        aPath.SetTangency(false);
        aPath.SetPassing(false);
        NCollection_Sequence<IntSurf_PathPoint> aPaths;
        aPaths.Append(aPath);
        IntPatch_TheIWalking                        aWalking(1.e-7, 1.e-3, 0.001);
        NCollection_Sequence<IntSurf_InteriorPoint> aInterior;
        aWalking.Perform(aPaths, aInterior, aFunction, aSurface);
        ASSERT_TRUE(aWalking.IsDone());
        ASSERT_EQ(aWalking.NbLines(), 1);
        const auto& aLine = aWalking.Value(1);
        ASSERT_GT(aLine->NbPoints(), 1);
        const auto& aLast = aLine->Value(aLine->NbPoints());
        double      aLastU, aLastV;
        aLast.ParametersOnS2(aLastU, aLastV);
        EXPECT_DOUBLE_EQ(aLastV, aLower);
        EXPECT_LE(aLast.Value().Distance(aSurface->Value(aU, aLower)), Precision::Confusion());
      }
    }
  }
}

TEST(IntPatch_WalkingTest, PathOnlyPerformInitializesAndResetsItsState)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ())), -1.0, 1.0, -1.0, 1.0);
  const IntSurf_Quadric    aPlane(gp_Pln(gp::Origin(), gp::DX()));
  IntPatch_TheSurfFunction aFunction(aSurface, aPlane);
  aFunction.Set(Precision::Confusion());
  math_Vector aParameters(1, 2), aValue(1, 1);
  aParameters(1) = 0.0;
  aParameters(2) = 1.0;
  ASSERT_TRUE(aFunction.Value(aParameters, aValue));
  ASSERT_FALSE(aFunction.IsTangent());
  IntSurf_PathPoint aPath(aFunction.Point(), 0.0, 1.0);
  aPath.SetDirections(aFunction.Direction3d(), aFunction.Direction2d());
  aPath.SetTangency(false);
  aPath.SetPassing(false);
  NCollection_Sequence<IntSurf_PathPoint> aPaths;
  aPaths.Append(aPath);
  IntPatch_TheIWalking aWalking(Precision::Confusion(), 1.e-3, 0.01);
  aWalking.Perform(aPaths, aFunction, aSurface);
  ASSERT_TRUE(aWalking.IsDone());
  ASSERT_EQ(aWalking.NbLines(), 1);
  const auto& aLine = aWalking.Value(1);
  ASSERT_GT(aLine->NbPoints(), 1);
  EXPECT_LE(aLine->Value(aLine->NbPoints()).Value().Distance(gp_Pnt(0.0, -1.0, 0.0)),
            Precision::Confusion());
  aPaths.Clear();
  aWalking.Perform(aPaths, aFunction, aSurface);
  ASSERT_TRUE(aWalking.IsDone());
  EXPECT_EQ(aWalking.NbLines(), 0);
  EXPECT_EQ(aWalking.NbSinglePnts(), 0);
}

TEST(IntPatch_WalkingTest, RegularBoundaryTangencyKeepsBranchEndpointsTogether)
{
  const occ::handle<Geom_SurfaceOfRevolution> aSurface =
    new Geom_SurfaceOfRevolution(new Geom_Line(gp_Pnt(0.0, 25.0, -25.0), gp_Dir(-1.0, 0.0, 2.0)),
                                 gp_Ax1(gp::Origin(), gp::DZ()));
  const TopoDS_Face aRevolutionFace = BRepBuilderAPI_MakeFace(aSurface,
                                                              0.0,
                                                              2.0 * M_PI,
                                                              0.0,
                                                              25.0 * std::sqrt(5.0),
                                                              Precision::Confusion());
  const TopoDS_Face aPlaneFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(-25.0, 0.0, 0.0), gp::DX()), -25.0, 25.0, -25.0, 25.0);
  IntTools_FaceFace anIntersection;
  anIntersection.SetParameters(true, true, true, Precision::Confusion());
  anIntersection.Perform(aPlaneFace, aRevolutionFace);
  ASSERT_TRUE(anIntersection.IsDone());
  ASSERT_EQ(anIntersection.Lines().Length(), 2);
  const occ::handle<Geom_Curve>& aFirst  = anIntersection.Lines()(1).Curve();
  const occ::handle<Geom_Curve>& aSecond = anIntersection.Lines()(2).Curve();
  ASSERT_FALSE(aFirst.IsNull());
  ASSERT_FALSE(aSecond.IsNull());
  double aJoinDistance = RealLast();
  for (double aFirstParameter : {aFirst->FirstParameter(), aFirst->LastParameter()})
  {
    for (double aSecondParameter : {aSecond->FirstParameter(), aSecond->LastParameter()})
    {
      aJoinDistance =
        std::min(aJoinDistance,
                 aFirst->Value(aFirstParameter).Distance(aSecond->Value(aSecondParameter)));
    }
  }
  EXPECT_LE(aJoinDistance, Precision::Confusion());
}

namespace
{
class ExtremaWalking : public IntWalk_PWalking
{
public:
  using IntWalk_PWalking::DistanceMinimizeByExtrema;
  using IntWalk_PWalking::IntWalk_PWalking;
};
} // namespace

TEST(IntPatch_WalkingTest, ProjectionUsesMixedHessianOnSkewedPlane)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.01, 0.0);
  aPoles(2, 2) = gp_Pnt(2.0, 0.01, 0.0);
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_BezierSurface(aPoles));
  ExtremaWalking aWalking(aSurface, aSurface, 1.e-7, 1.e-7, 1.e-3, 0.01);
  double         aU = 0.1, aV = 0.1;
  ASSERT_TRUE(aWalking.DistanceMinimizeByExtrema(aSurface, gp_Pnt(1.0, 0.005, 0.0), aU, aV));
  EXPECT_NEAR(aU, 0.5, 1.e-10);
  EXPECT_NEAR(aV, 0.5, 1.e-10);
}

namespace
{
class ContactWalking : public IntWalk_PWalking
{
public:
  using IntWalk_PWalking::ComputeContactDirection;
};

//=================================================================================================

static occ::handle<Adaptor3d_Surface> makeContactGraph(double         theScaleU,
                                                       double         theScaleV,
                                                       double         theSkew,
                                                       double         theHeight,
                                                       double         theCurvatureV,
                                                       const gp_Trsf& theTransform)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  const double               aQuadratic[3] = {0.25, -0.25, 0.25};
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      aPoles.ChangeValue(i + 1, j + 1) =
        gp_Pnt(theScaleU * (0.5 * i - 0.5) + theSkew * (0.5 * j - 0.5),
               theScaleV * (0.5 * j - 0.5),
               theHeight * (aQuadratic[i] + theCurvatureV * aQuadratic[j]))
          .Transformed(theTransform);
    }
  }
  return new GeomAdaptor_Surface(new Geom_BezierSurface(aPoles));
}
} // namespace

TEST(IntPatch_WalkingTest, ContactDirectionDistinguishesCurveFromIsolatedAndCrossingContacts)
{
  for (double aScaleU : {0.001, 1.0, 1000.0})
  {
    for (double aScaleV : {0.001, 1.0, 1000.0})
    {
      for (double aSkew : {0.0, 1.0})
      {
        for (double aCurvatureV : {0.0, 1.0, -1.0, 1.e-5})
        {
          for (bool isRotated : {false, true})
          {
            SCOPED_TRACE(::testing::Message() << aScaleU << ' ' << aScaleV << ' ' << aSkew << ' '
                                              << aCurvatureV << ' ' << isRotated);
            gp_Trsf aTransform;
            if (isRotated)
            {
              aTransform.SetRotation(gp_Ax1(gp::Origin(), gp_Dir(1.0, 2.0, 3.0)), 0.8);
              aTransform.SetTranslationPart(gp_Vec(2.0, 3.0, 4.0));
            }
            const auto aPlane = makeContactGraph(aScaleU, aScaleV, aSkew, 0.0, 0.0, aTransform);
            const auto aCurved =
              makeContactGraph(aScaleU, aScaleV, aSkew, 1.0, aCurvatureV, aTransform);
            IntSurf_PntOn2S aPoint;
            aPoint.SetValue(aPlane->Value(0.5, 0.5), 0.5, 0.5, 0.5, 0.5);
            const gp_Dir anExpected(gp_Vec(aSkew, aScaleV, 0.0).Transformed(aTransform));
            gp_Dir       aDirection = anExpected;
            gp_Dir2d     aDirection1, aDirection2;
            const bool   isContact = ContactWalking::ComputeContactDirection(aPlane,
                                                                           aCurved,
                                                                           aPoint,
                                                                           aDirection,
                                                                           aDirection1,
                                                                           aDirection2);
            EXPECT_EQ(isContact, aCurvatureV == 0.0);
            if (isContact)
            {
              EXPECT_NEAR(aDirection.Dot(anExpected), 1.0, 1.e-8);
            }
            EXPECT_FALSE(ContactWalking::ComputeContactDirection(aCurved,
                                                                 aCurved,
                                                                 aPoint,
                                                                 aDirection,
                                                                 aDirection1,
                                                                 aDirection2));
          }
        }
      }
    }
  }
}

TEST(IntPatch_WalkingTest, ExactSeedTraversesContactLine)
{
  for (double aScaleU : {0.01, 1.0, 100.0})
  {
    for (double aScaleV : {0.01, 1.0, 100.0})
    {
      for (double aSkew : {0.0, 0.5})
      {
        SCOPED_TRACE(::testing::Message() << aScaleU << ' ' << aScaleV << ' ' << aSkew);
        const gp_Trsf aTransform;
        const auto    aPlane  = makeContactGraph(aScaleU, aScaleV, aSkew, 0.0, 0.0, aTransform);
        const auto    aCurved = makeContactGraph(aScaleU, aScaleV, aSkew, 1.0, 0.0, aTransform);
        NCollection_Array1<double> aParameters(1, 4);
        aParameters.Init(0.5);
        IntWalk_PWalking aWalking(aPlane, aCurved, 1.e-7, 1.e-7, 1.e-3, 0.01);
        aWalking.Perform(aParameters);
        ASSERT_TRUE(aWalking.IsDone());
        ASSERT_GT(aWalking.NbPoints(), 1);
        EXPECT_NEAR(aWalking.Value(1).Value().Distance(aWalking.Value(aWalking.NbPoints()).Value()),
                    std::hypot(aSkew, aScaleV),
                    1.e-6);
        for (int i = 1; i <= aWalking.NbPoints(); ++i)
        {
          double aU, aV;
          aWalking.Value(i).ParametersOnS1(aU, aV);
          EXPECT_NEAR(aU, 0.5, 1.e-7);
          EXPECT_NEAR(aWalking.Value(i).Value().Z(), 0.0, 1.e-7);
        }
      }
    }
  }
}

TEST(IntPatch_WalkingTest, PoleDerivativeDoesNotDefineRegularIntersection)
{
  for (const double aScale : {1.0, 1.e6})
  {
    for (const double aUDerivative : {4.e-16, 1.e-6})
    {
      const gp_Vec              aDerivatives[4] = {gp_Vec(aScale * aUDerivative, 0.0, 0.0),
                                                   gp_Vec(0.0, 0.0, 3.0),
                                                   gp_Vec(0.0, aScale * aUDerivative, 0.0),
                                                   gp_Vec(0.0, 0.0, 3.0)};
      const double              aResolution     = Precision::Confusion() / 3.0;
      const double              aResolutions[4] = {aResolution / aScale,
                                                   aResolution,
                                                   aResolution / aScale,
                                                   aResolution};
      double                    aTangent[4];
      IntImp_ConstIsoparametric anIso[4];
      EXPECT_EQ(IntImp_ComputeTangence(aDerivatives, aResolutions, aTangent, anIso),
                aUDerivative == 4.e-16);
    }
  }
}

TEST(IntPatch_WalkingTest, SphereEllipsoidContactCoversCircleWithoutOverlap)
{
  for (const double aStretch :
       {1.5, 1.75, std::nextafter(2.0, 0.0), 2.0, std::nextafter(2.0, 3.0), 2.25, 2.5, 3.0})
  {
    SCOPED_TRACE(aStretch);
    const auto aSphere = GeomConvert::SurfaceToBSplineSurface(new Geom_RectangularTrimmedSurface(
      new Geom_SphericalSurface(gp_Ax3(gp::Origin(), gp::DZ()), 3.0),
      0.0,
      2.0 * M_PI,
      -0.5 * M_PI,
      0.5 * M_PI));
    const auto anEllipsoid = occ::down_cast<Geom_BSplineSurface>(aSphere->Copy());
    for (int i = 1; i <= anEllipsoid->NbUPoles(); ++i)
    {
      for (int j = 1; j <= anEllipsoid->NbVPoles(); ++j)
      {
        gp_Pnt aPole = anEllipsoid->Pole(i, j);
        aPole.SetX(aStretch * aPole.X());
        anEllipsoid->SetPole(i, j, aPole);
      }
    }
    GeomAPI_IntSS anIntersection;
    ASSERT_NO_THROW(anIntersection.Perform(aSphere, anEllipsoid, Precision::Confusion()));
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_GT(anIntersection.NbLines(), 0);
    double aLength = 0.0;
    for (int i = 1; i <= anIntersection.NbLines(); ++i)
    {
      const auto&             aCurve = anIntersection.Line(i);
      const GeomAdaptor_Curve anAdaptor(aCurve);
      aLength += GCPnts_AbscissaPoint::Length(anAdaptor, Precision::Confusion());
      for (int j = 0; j <= 100; ++j)
      {
        const double aParameter =
          aCurve->FirstParameter()
          + (aCurve->LastParameter() - aCurve->FirstParameter()) * j / 100.0;
        const gp_Pnt aPoint = aCurve->Value(aParameter);
        EXPECT_NEAR(aPoint.X(), 0.0, 2.e-4);
        EXPECT_NEAR(std::hypot(aPoint.Y(), aPoint.Z()), 3.0, 2.e-4);
      }
    }
    EXPECT_NEAR(aLength, 6.0 * M_PI, 2.e-4);
    for (int i = 0; i < 100; ++i)
    {
      const double anAngle = 2.0 * M_PI * i / 100.0;
      const gp_Pnt aPoint(0.0, 3.0 * std::cos(anAngle), 3.0 * std::sin(anAngle));
      double       aDistance = RealLast();
      for (int j = 1; j <= anIntersection.NbLines(); ++j)
      {
        GeomAPI_ProjectPointOnCurve aProjection(aPoint, anIntersection.Line(j));
        if (aProjection.NbPoints() != 0)
        {
          aDistance = std::min(aDistance, aProjection.LowerDistance());
        }
      }
      EXPECT_LE(aDistance, 2.e-4);
    }
  }
}

TEST(IntPatch_WalkingTest, CrossingToriReachTheirSingularJunctions)
{
  for (double anAngle : {1.0, 1.56, 1.57, 1.58, 0.5 * M_PI})
  {
    SCOPED_TRACE(anAngle);
    const occ::handle<Geom_ToroidalSurface> aFirstSurface =
      new Geom_ToroidalSurface(gp_Ax3(gp::Origin(),
                                      gp_Dir(std::sin(anAngle), 0.0, std::cos(anAngle)),
                                      gp_Dir(std::cos(anAngle), 0.0, -std::sin(anAngle))),
                               100.0,
                               25.0);
    const occ::handle<Geom_ToroidalSurface> aSecondSurface =
      new Geom_ToroidalSurface(gp_Ax3(gp::Origin(), gp::DZ()), 100.0, 25.0);
    const TopoDS_Face aFirstFace  = BRepBuilderAPI_MakeFace(aFirstSurface, Precision::Confusion());
    const TopoDS_Face aSecondFace = BRepBuilderAPI_MakeFace(aSecondSurface, Precision::Confusion());
    const gp_Pnt      aJunctions[4] = {gp_Pnt(0.0, -125.0, 0.0),
                                       gp_Pnt(0.0, -75.0, 0.0),
                                       gp_Pnt(0.0, 75.0, 0.0),
                                       gp_Pnt(0.0, 125.0, 0.0)};
    for (bool isReversed : {false, true})
    {
      SCOPED_TRACE(isReversed);
      IntTools_FaceFace anIntersection;
      anIntersection.SetParameters(true, true, true, Precision::Confusion());
      anIntersection.Perform(isReversed ? aSecondFace : aFirstFace,
                             isReversed ? aFirstFace : aSecondFace);
      ASSERT_TRUE(anIntersection.IsDone());
      ASSERT_EQ(anIntersection.Lines().Size(), 8u);
      double aDistances[4] = {RealLast(), RealLast(), RealLast(), RealLast()};
      for (const auto& aLine : anIntersection.Lines())
      {
        const occ::handle<Geom_Curve>& aCurve = aLine.Curve();
        ASSERT_FALSE(aCurve.IsNull());
        for (double aParameter : {aCurve->FirstParameter(), aCurve->LastParameter()})
        {
          const gp_Pnt aPoint   = aCurve->EvalD0(aParameter);
          double       aNearest = RealLast();
          for (size_t i = 0; i < 4; ++i)
          {
            const double aDistance = aPoint.Distance(aJunctions[i]);
            aNearest               = std::min(aNearest, aDistance);
            aDistances[i]          = std::min(aDistances[i], aDistance);
          }
          EXPECT_LE(aNearest, Precision::Confusion());
        }
      }
      for (double aDistance : aDistances)
      {
        EXPECT_LE(aDistance, Precision::Confusion());
      }
    }
  }
}

TEST(IntPatch_WalkingTest, SingularJunctionIsNotExtendedToNearbyBoundary)
{
  for (double anOffset : {0.0, 100.0})
  {
    SCOPED_TRACE(anOffset);
    const gp_Vec   aShift(anOffset, anOffset, anOffset);
    const gp_Torus aTorus1(gp_Ax3(gp_Pnt(anOffset, anOffset, anOffset), gp::DZ()), 100.0, 10.0);
    const gp_Torus aTorus2(gp_Ax3(gp_Pnt(100.0 + anOffset, anOffset, anOffset), gp::DZ()),
                           100.0,
                           10.0);
    const double   aMargin = 1.e-6;
    const occ::handle<Adaptor3d_Surface> aSurface1 =
      new GeomAdaptor_Surface(new Geom_ToroidalSurface(aTorus1),
                              M_PI / 3.0 - aMargin,
                              5.0 * M_PI / 3.0 + aMargin,
                              0.0,
                              2.0 * M_PI);
    const occ::handle<Adaptor3d_Surface> aSurface2 =
      new GeomAdaptor_Surface(new Geom_ToroidalSurface(aTorus2),
                              2.0 * M_PI / 3.0 - aMargin,
                              4.0 * M_PI / 3.0 + aMargin,
                              0.0,
                              2.0 * M_PI);
    const gp_Pnt aSeed     = gp_Pnt(40.0, -std::sqrt(7425.0), -std::sqrt(75.0)).Translated(aShift);
    const gp_Pnt aJunction = gp_Pnt(50.0, -std::sqrt(7500.0), -10.0).Translated(aShift);
    NCollection_Array1<double> aParameters(1, 4);
    ElSLib::Parameters(aTorus1, aSeed, aParameters.ChangeAt(0), aParameters.ChangeAt(1));
    ElSLib::Parameters(aTorus2, aSeed, aParameters.ChangeAt(2), aParameters.ChangeAt(3));
    IntWalk_PWalking aWalking(aSurface1,
                              aSurface2,
                              Precision::Confusion(),
                              Precision::Confusion(),
                              0.01,
                              0.01);
    aWalking.Perform(aParameters);
    ASSERT_TRUE(aWalking.IsDone());
    ASSERT_GT(aWalking.NbPoints(), 2);
    ASSERT_LE(std::min(aWalking.Value(1).Value().Distance(aJunction),
                       aWalking.Value(aWalking.NbPoints()).Value().Distance(aJunction)),
              Precision::Confusion());

    aWalking.PutToBoundary(aSurface1, aSurface2);
    EXPECT_LE(std::min(aWalking.Value(1).Value().Distance(aJunction),
                       aWalking.Value(aWalking.NbPoints()).Value().Distance(aJunction)),
              Precision::Confusion());
  }
}
