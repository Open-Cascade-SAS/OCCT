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
#include <GeomInt_IntSS.hxx>
#include <GeomInt_WLApprox.hxx>
#include <BRepApprox_Approx.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomInt_ThePrmPrmSvSurfacesOfWLApprox.hxx>
#include <GeomInt_TheImpPrmSvSurfacesOfWLApprox.hxx>
#include <BRepApprox_ThePrmPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_TheImpPrmSvSurfacesOfApprox.hxx>
#include <BRepApprox_ApproxLine.hxx>
#include <BRepApprox_TheMultiLineOfApprox.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <array>
#include <Geom_Ellipse.hxx>
#include <Geom_Circle.hxx>
#include <gp_Dir.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Elips.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <IntPatch_TheIWalking.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_RstInt.hxx>
#include <IntPatch_WLine.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntWalk_PWalking.hxx>
#include <Geom_BezierSurface.hxx>
#include <NCollection_Array2.hxx>
#include <IntTools_FaceFace.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
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
#include <gp_Lin.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <IntWalk_TheInt2S.hxx>
#include <NCollection_LinearVector.hxx>
#include <tuple>

namespace
{
//==================================================================================================

template <class Approximation, class FirstSurface, class SecondSurface>
void checkApproximationCache(const FirstSurface&                         theFirst,
                             const SecondSurface&                        theSecond,
                             const std::array<std::array<double, 4>, 2>& theSeeds = {
                               {{2.0, 0.4, -2.9, 0.3}, {3.0, -0.2, -2.1, -0.1}}})
{
  Approximation aCached(theFirst, theSecond);
  for (const int anIndex : {0, 0, 1, 0, 1})
  {
    SCOPED_TRACE(anIndex);
    std::array<double, 4> aParameters = theSeeds[anIndex], anExpectedParameters = aParameters;
    Approximation         aReference(theFirst, theSecond);
    gp_Pnt                anExpectedPoint;
    gp_Vec                anExpectedTangent;
    gp_Vec2d              anExpectedTangent1, anExpectedTangent2;
    ASSERT_TRUE(aReference.Compute(anExpectedParameters[0],
                                   anExpectedParameters[1],
                                   anExpectedParameters[2],
                                   anExpectedParameters[3],
                                   anExpectedPoint,
                                   anExpectedTangent,
                                   anExpectedTangent1,
                                   anExpectedTangent2));
    gp_Pnt   aPoint(-1.0, -2.0, -3.0);
    gp_Vec   aTangent(-1.0, -2.0, -3.0);
    gp_Vec2d aTangent1(-1.0, -2.0), aTangent2(-2.0, -3.0);
    ASSERT_TRUE(aCached.Compute(aParameters[0],
                                aParameters[1],
                                aParameters[2],
                                aParameters[3],
                                aPoint,
                                aTangent,
                                aTangent1,
                                aTangent2));
    for (size_t i = 0; i < aParameters.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(aParameters[i], anExpectedParameters[i]);
    }
    EXPECT_DOUBLE_EQ(aPoint.SquareDistance(anExpectedPoint), 0.0);
    EXPECT_DOUBLE_EQ((aTangent - anExpectedTangent).SquareMagnitude(), 0.0);
    EXPECT_DOUBLE_EQ((aTangent1 - anExpectedTangent1).SquareMagnitude(), 0.0);
    EXPECT_DOUBLE_EQ((aTangent2 - anExpectedTangent2).SquareMagnitude(), 0.0);
    IntSurf_PntOn2S aFound;
    ASSERT_TRUE(aCached.SeekPoint(theSeeds[anIndex][0],
                                  theSeeds[anIndex][1],
                                  theSeeds[anIndex][2],
                                  theSeeds[anIndex][3],
                                  aFound));
    EXPECT_DOUBLE_EQ(aFound.Value().SquareDistance(anExpectedPoint), 0.0);
    aFound.Parameters(aParameters[0], aParameters[1], aParameters[2], aParameters[3]);
    for (size_t i = 0; i < aParameters.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(aParameters[i], anExpectedParameters[i]);
    }
    const std::array<double, 4>& anOtherSeed = theSeeds[1 - anIndex];
    ASSERT_TRUE(
      aCached.SeekPoint(anOtherSeed[0], anOtherSeed[1], anOtherSeed[2], anOtherSeed[3], aFound));
    const std::array<double, 4>& aSeed = theSeeds[anIndex];
    aCached.Pnt(aSeed[0], aSeed[1], aSeed[2], aSeed[3], aPoint);
    EXPECT_DOUBLE_EQ(aPoint.SquareDistance(anExpectedPoint), 0.0);
  }
}

//==================================================================================================

template <class Approximation, class Surface>
void checkImplicitApproximationCache(const Surface& thePlane)
{
  const IntSurf_Quadric                      aCylinder(gp_Cylinder(gp_Ax3(), 1.0));
  const std::array<std::array<double, 4>, 2> aSeeds = {
    {{0.0, 0.0, 1.0, 0.0}, {0.6, 0.0, std::cos(0.6), std::sin(0.6)}}};
  checkApproximationCache<Approximation>(aCylinder, thePlane, aSeeds);
  const std::array<std::array<double, 4>, 2> aReversedSeeds = {
    {{1.0, 0.0, 0.0, 0.0}, {std::cos(0.6), std::sin(0.6), 0.6, 0.0}}};
  checkApproximationCache<Approximation>(thePlane, aCylinder, aReversedSeeds);
}
} // namespace

TEST(IntPatch_WalkingTest, SurfaceApproximationCacheRestoresOutputs)
{
  const occ::handle<Adaptor3d_Surface> aFirst = new GeomAdaptor_Surface(new Geom_Plane(gp::XOY()));
  const occ::handle<Adaptor3d_Surface> aSecond =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX())));
  checkApproximationCache<GeomInt_ThePrmPrmSvSurfacesOfWLApprox>(aFirst, aSecond);
}

TEST(IntPatch_WalkingTest, FaceApproximationCacheRestoresOutputs)
{
  const TopoDS_Face aFirstFace = BRepBuilderAPI_MakeFace(gp_Pln(gp_Ax3(gp::XOY()))).Face();
  const TopoDS_Face aSecondFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Ax3(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX()))).Face();
  const BRepAdaptor_Surface aFirst(aFirstFace), aSecond(aSecondFace);
  checkApproximationCache<BRepApprox_ThePrmPrmSvSurfacesOfApprox>(aFirst, aSecond);
}

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

TEST(IntPatch_WalkingTest, TangentSeedOutsideDomainUsesBoundedSolution)
{
  const occ::handle<Geom_Plane>        aPlane = new Geom_Plane(gp::XOY());
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(aPlane, 0.0, 1.0, 0.0, 1.0);
  for (double aV : {-0.2, 0.5, 1.2})
  {
    SCOPED_TRACE(aV);
    IntWalk_TheInt2S           aIntersection(aSurface, aSurface, Precision::Confusion());
    math_FunctionSetRoot       aSolver(aIntersection.Function());
    NCollection_Array1<double> aSeed(1, 4);
    aSeed(1) = aSeed(3) = 0.5;
    aSeed(2) = aSeed(4) = aV;
    math_Vector aFreeParameters(1, 3), aLower(1, 3), anUpper(1, 3), aTolerance(1, 3);
    aIntersection.Function().ComputeParameters(IntImp_UIsoparametricOnCaro1,
                                               aSeed,
                                               aFreeParameters,
                                               aLower,
                                               anUpper,
                                               aTolerance);
    aIntersection.Perform(aSeed, aSolver, IntImp_UIsoparametricOnCaro1);
    ASSERT_TRUE(aIntersection.IsDone());
    ASSERT_FALSE(aIntersection.IsEmpty());
    double aU1, aV1, aU2, aV2;
    aIntersection.Point().Parameters(aU1, aV1, aU2, aV2);
    EXPECT_DOUBLE_EQ(aU1, aSeed(1));
    EXPECT_GE(aV1, aLower(1));
    EXPECT_LE(aV1, anUpper(1));
    EXPECT_GE(aV2, aLower(3));
    EXPECT_LE(aV2, anUpper(3));
    EXPECT_LE(aSurface->EvalD0(aU1, aV1).Distance(aSurface->EvalD0(aU2, aV2)),
              Precision::Confusion());
  }
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

namespace
{
class IntPatch_PassageWalking : public IntPatch_TheIWalking
{
public:
  using IntPatch_TheIWalking::IntPatch_TheIWalking;
  using IntPatch_TheIWalking::TestArretPassage;
};
} // namespace

TEST(IntPatch_WalkingTest, SelectedStopUpdatesSurfaceFunction)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp::Origin(), gp::DZ())), -1.0, 1.0, -1.0, 1.0);
  const IntSurf_Quadric aPlane(gp_Pln(gp::Origin(), gp::DX()));
  for (const double aStartV : {-1.0, 1.0})
  {
    SCOPED_TRACE(aStartV);
    IntPatch_TheSurfFunction aFunction(aSurface, aPlane);
    aFunction.Set(Precision::Confusion());
    math_Vector aParameters(1, 2), aValue(1, 1);
    aParameters(1) = 0.0;
    aParameters(2) = aStartV;
    ASSERT_TRUE(aFunction.Value(aParameters, aValue));
    ASSERT_FALSE(aFunction.IsTangent());
    IntSurf_PathPoint aPath(aFunction.Point(), 0.0, aStartV);
    if (aFunction.Direction2d().Y() * aStartV > 0.0)
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
    IntPatch_PassageWalking aWalking(Precision::Confusion(), 1.e-3, 0.01);
    aWalking.Perform(aPaths, aFunction, aSurface);
    ASSERT_TRUE(aWalking.IsDone());
    ASSERT_EQ(aWalking.NbLines(), 1);

    // Crossing the processed path point selects its parameters from the candidate list.
    aParameters(2) = 1.5 * aStartV;
    ASSERT_TRUE(aFunction.Value(aParameters, aValue));
    NCollection_Sequence<double> aMultipleU, aMultipleV;
    int                          aStopIndex = 0;
    ASSERT_TRUE(
      aWalking.TestArretPassage(aMultipleU, aMultipleV, aFunction, aParameters, aStopIndex));
    EXPECT_EQ(aStopIndex, 1);
    EXPECT_DOUBLE_EQ(aParameters(2), aStartV);
    EXPECT_LE(aFunction.Point().Distance(aSurface->EvalD0(aParameters(1), aParameters(2))),
              Precision::Confusion());
    EXPECT_FALSE(aFunction.IsTangent());
  }
}

TEST(IntPatch_WalkingTest, MarchingPreservesDirectionOnSkewSurface)
{
  for (const double aSkew : {0.25, 0.75, 1.0, 2.0})
  {
    SCOPED_TRACE(aSkew);
    NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
    for (size_t aU = 0; aU < 2; ++aU)
    {
      for (size_t aV = 0; aV < 2; ++aV)
      {
        aPoles.ChangeAt(aU, aV) =
          gp_Pnt(2.0 * aU - 1.0 + aSkew * (2.0 * aV - 1.0), 2.0 * aV - 1.0, 0.0);
      }
    }
    const occ::handle<Adaptor3d_Surface> aSurface =
      new GeomAdaptor_Surface(new Geom_BezierSurface(aPoles));
    const IntSurf_Quadric    aPlane(gp_Pln(gp::Origin(), gp::DX()));
    IntPatch_TheSurfFunction aFunction(aSurface, aPlane);
    aFunction.Set(Precision::Confusion());
    math_Vector aParameters(1, 2), aValue(1, 1);
    aParameters(1) = aSkew <= 1.0 ? 0.5 * (1.0 - aSkew) : 0.0;
    aParameters(2) = aSkew <= 1.0 ? 1.0 : 0.5 * (1.0 + 1.0 / aSkew);
    ASSERT_TRUE(aFunction.Value(aParameters, aValue));
    ASSERT_FALSE(aFunction.IsTangent());
    IntSurf_PathPoint aPath(aFunction.Point(), aParameters(1), aParameters(2));
    aPath.SetDirections(aFunction.Direction3d(), aFunction.Direction2d());
    aPath.SetTangency(false);
    aPath.SetPassing(false);
    NCollection_Sequence<IntSurf_PathPoint> aPaths;
    aPaths.Append(aPath);
    IntPatch_PassageWalking aWalking(Precision::Confusion(), 1.e-3, 0.01);
    aWalking.Perform(aPaths, aFunction, aSurface);
    ASSERT_TRUE(aWalking.IsDone());
    ASSERT_EQ(aWalking.NbLines(), 1);

    const occ::handle<IntPatch_TheIWLineOfTheIWalking>& aLine = aWalking.Value(1);
    ASSERT_GT(aLine->NbPoints(), 2);
    double aPreviousU, aPreviousV;
    aLine->Value(1).ParametersOnS2(aPreviousU, aPreviousV);
    for (int aPointIndex = 2; aPointIndex <= aLine->NbPoints(); ++aPointIndex)
    {
      double aU, aV;
      aLine->Value(aPointIndex).ParametersOnS2(aU, aV);
      EXPECT_NEAR(aU - 0.5 * (1.0 - aSkew * (2.0 * aV - 1.0)), 0.0, Precision::PConfusion());
      EXPECT_GT(aPreviousV - aV, 0.0);
      EXPECT_NEAR((aU - aPreviousU) + aSkew * (aV - aPreviousV), 0.0, Precision::PConfusion());
      aPreviousU = aU;
      aPreviousV = aV;
    }
  }
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
  const double aTolerance = BRep_Tool::Tolerance(aPlaneFace) + BRep_Tool::Tolerance(aRevolutionFace)
                            + anIntersection.FuzzyValue();
  const gp_Pnt aJoin(-25.0, 0.0, -25.0);
  const gp_Lin aPositiveBranch(aJoin, gp_Dir(0.0, 1.0, 2.0));
  const gp_Lin aNegativeBranch(aJoin, gp_Dir(0.0, -1.0, 2.0));
  for (const auto& aCurve : {aFirst, aSecond})
  {
    const double aJoinError = std::min(aCurve->EvalD0(aCurve->FirstParameter()).Distance(aJoin),
                                       aCurve->EvalD0(aCurve->LastParameter()).Distance(aJoin));
    EXPECT_LE(aJoinError, aTolerance);
    const GeomAdaptor_Curve anAdaptor(aCurve);
    for (size_t anIndex = 0; anIndex <= 8; ++anIndex)
    {
      const double aParameter =
        aCurve->FirstParameter()
        + anIndex * (aCurve->LastParameter() - aCurve->FirstParameter()) / 8;
      const gp_Pnt aPoint = anAdaptor.EvalD0(aParameter);
      EXPECT_LE(std::min(aPositiveBranch.Distance(aPoint), aNegativeBranch.Distance(aPoint)),
                aTolerance);
    }
  }
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
  // The two endpoints can each differ from the exact join by the intersection tolerance.
  EXPECT_LE(aJoinDistance, 2.0 * aTolerance);
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
        const double     anIncrement = 1.e-3;
        IntWalk_PWalking aWalking(aPlane, aCurved, 1.e-7, 1.e-7, 1.e-3, anIncrement);
        aWalking.Perform(aParameters);
        ASSERT_TRUE(aWalking.IsDone());
        ASSERT_GT(aWalking.NbPoints(), 1);
        // A straight contact permits step growth.
        EXPECT_LT(aWalking.NbPoints(), static_cast<int>(1.0 / anIncrement));
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

TEST(IntPatch_WalkingTest, ApproximationPreservesCylinderIntersection)
{
  const double                  aRadius = 0.249128788767645;
  const occ::handle<Geom_Curve> anEllipse =
    new Geom_Ellipse(gp_Elips(gp_Ax2(gp::Origin(), gp::DZ()), 6.03031367203927, 3.11993062568844));
  const occ::handle<Geom_Surface> anExtrusion =
    new Geom_SurfaceOfLinearExtrusion(anEllipse, gp::DZ());
  const occ::handle<Geom_Surface> aCylinder =
    new Geom_CylindricalSurface(gp_Ax3(gp::Origin(), gp::DX()), aRadius);

  for (const bool aIsReversed : {false, true})
  {
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(aIsReversed ? aCylinder : anExtrusion,
                           aIsReversed ? anExtrusion : aCylinder,
                           Precision::Confusion(),
                           true);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbLines(), 4);
    for (int aLine = 1; aLine <= anIntersection.NbLines(); ++aLine)
    {
      const occ::handle<Geom_Curve>& aCurve = anIntersection.Line(aLine);
      ASSERT_FALSE(aCurve.IsNull());
      GeomAdaptor_Curve anAdaptor(aCurve);
      double            aMaxError = 0.0;
      for (size_t aSample = 0; aSample <= 300; ++aSample)
      {
        const double aParameter =
          aCurve->FirstParameter()
          + (aCurve->LastParameter() - aCurve->FirstParameter()) * double(aSample) / 300.0;
        const gp_Pnt aPoint = anAdaptor.EvalD0(aParameter);
        aMaxError = std::max(aMaxError, std::abs(std::hypot(aPoint.Y(), aPoint.Z()) - aRadius));
      }
      // Check the continuous approximation between the walking points.
      EXPECT_LE(aMaxError, 1.0e-5) << "Line " << aLine << ", reversed " << aIsReversed;
    }
  }
}

TEST(IntPatch_WalkingTest, ApproximationPreservesConeCylinderIntersection)
{
  const gp_Cone aCone(gp_Ax3(gp_Pnt(320.178553907536, 4365.68326149859, 2.0), gp::DZ(), gp::DX()),
                      0.122173047639603,
                      4365.0);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(120.7, -50.0, 103.15), gp::DY(), -gp::DZ()), 97.5);
  const occ::handle<Geom_Surface> aFirst  = new Geom_ConicalSurface(aCone);
  const occ::handle<Geom_Surface> aSecond = new Geom_CylindricalSurface(aCylinder);
  const IntSurf_Quadric           aFirstQuadric(aCone), aSecondQuadric(aCylinder);

  for (const bool aIsReversed : {false, true})
  {
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(aIsReversed ? aSecond : aFirst,
                           aIsReversed ? aFirst : aSecond,
                           1.0e-4,
                           true);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbLines(), 2);
    for (int aLine = 1; aLine <= anIntersection.NbLines(); ++aLine)
    {
      const occ::handle<Geom_Curve>& aCurve = anIntersection.Line(aLine);
      ASSERT_FALSE(aCurve.IsNull());
      GeomAdaptor_Curve anAdaptor(aCurve);
      double            aMaxError = 0.0;
      for (size_t aSample = 0; aSample <= 300; ++aSample)
      {
        const double aParameter =
          aCurve->FirstParameter()
          + (aCurve->LastParameter() - aCurve->FirstParameter()) * double(aSample) / 300.0;
        const gp_Pnt aPoint = anAdaptor.EvalD0(aParameter);
        aMaxError           = std::max(aMaxError, std::abs(aFirstQuadric.Distance(aPoint)));
        aMaxError           = std::max(aMaxError, std::abs(aSecondQuadric.Distance(aPoint)));
      }
      // The 3D fit must retain the accuracy of the intersection walking points.
      EXPECT_LE(aMaxError, 2.0e-7) << "Line " << aLine << ", reversed " << aIsReversed;
    }
  }
}

namespace
{
//==================================================================================================

void checkCurveDistances(const GeomInt_IntSS&   theIntersection,
                         const IntSurf_Quadric& theFirst,
                         const IntSurf_Quadric& theSecond,
                         const double           theTolerance,
                         const size_t           theSampleCount)
{
  for (int aLine = 1; aLine <= theIntersection.NbLines(); ++aLine)
  {
    const occ::handle<Geom_Curve>& aCurve = theIntersection.Line(aLine);
    ASSERT_FALSE(aCurve.IsNull());
    GeomAdaptor_Curve anAdaptor(aCurve);
    const double      aFirst    = aCurve->FirstParameter();
    const double      aRange    = aCurve->LastParameter() - aFirst;
    double            aMaxError = 0.0;
    for (size_t aSample = 0; aSample <= theSampleCount; ++aSample)
    {
      const gp_Pnt aPoint =
        anAdaptor.EvalD0(aFirst + aRange * double(aSample) / double(theSampleCount));
      aMaxError = std::max(aMaxError, std::abs(theFirst.Distance(aPoint)));
      aMaxError = std::max(aMaxError, std::abs(theSecond.Distance(aPoint)));
    }
    EXPECT_LE(aMaxError, theTolerance) << "Line " << aLine;
  }
}
} // namespace

TEST(IntPatch_WalkingTest, ApproximationPreservesBothCylinderSurfaces)
{
  const gp_Cylinder aFirstCylinder(
    gp_Ax3(gp_Pnt(4.999999999813, 1.35836143386791, 0.20975890778078), -gp::DX(), gp::DZ()),
    10.0);
  const gp_Cylinder aSecondCylinder(
    gp_Ax3(gp_Pnt(9.999999999813, 9.1401960568287, 0.11837300583107),
           gp_Dir(0.0, -0.75870713028692, 0.651431877061437),
           gp_Dir(0.0, -0.651431877061437, -0.75870713028692)),
    5.0);
  const occ::handle<Geom_Surface> aFirst  = new Geom_CylindricalSurface(aFirstCylinder);
  const occ::handle<Geom_Surface> aSecond = new Geom_CylindricalSurface(aSecondCylinder);
  const IntSurf_Quadric           aFirstQuadric(aFirstCylinder), aSecondQuadric(aSecondCylinder);
  for (const bool aReverse : {false, true})
  {
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(aReverse ? aSecond : aFirst, aReverse ? aFirst : aSecond, 1.0e-7);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbLines(), 6);
    checkCurveDistances(anIntersection, aFirstQuadric, aSecondQuadric, 2.0e-7, 360);
  }
}

TEST(IntPatch_WalkingTest, ApproximationRejectsCoincidentLinearCurves)
{
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(-127.0, 84.0, 100.0), -gp::DZ(), gp::DX()), 23.0, 10.0);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(-132.0, 61.5500556793564, 131.0), -gp::DZ(), gp::DX()),
                              10.0);
  const occ::handle<Geom_Surface> aFirst  = new Geom_ToroidalSurface(aTorus);
  const occ::handle<Geom_Surface> aSecond = new Geom_CylindricalSurface(aCylinder);
  const IntSurf_Quadric           aFirstQuadric(aTorus), aSecondQuadric(aCylinder);
  for (const bool aReverse : {false, true})
  {
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(aReverse ? aSecond : aFirst, aReverse ? aFirst : aSecond, 1.0e-4);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbLines(), 6);
    checkCurveDistances(anIntersection, aFirstQuadric, aSecondQuadric, 6.0e-7, 297);
  }
}

TEST(IntPatch_WalkingTest, RestrictionKeepsEndpointsAndAddsNearbyCrossings)
{
  const occ::handle<GeomAdaptor_Surface> aPlane =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3()), -1.0, 1.0, 0.0, 1.0);
  const occ::handle<GeomAdaptor_Surface> aCrossPlane =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3(gp::Origin(), gp::DX(), gp::DY())),
                            -1.0,
                            2.0,
                            -1.0,
                            1.0);
  const occ::handle<Adaptor3d_TopolTool> aDomain = new Adaptor3d_TopolTool(aPlane);
  for (const double aMargin : {0.0, 10.0 * Precision::Confusion()})
  {
    for (const bool isReversed : {false, true})
    {
      SCOPED_TRACE(::testing::Message() << aMargin << ' ' << isReversed);
      const occ::handle<IntSurf_LineOn2S> aPoints = new IntSurf_LineOn2S();
      for (const double aParameter : {-aMargin, 0.5, 1.0 + aMargin})
      {
        IntSurf_PntOn2S aPoint;
        if (isReversed)
        {
          aPoint.SetValue(gp_Pnt(0.0, aParameter, 0.0), aParameter, 0.0, 0.0, aParameter);
        }
        else
        {
          aPoint.SetValue(gp_Pnt(0.0, aParameter, 0.0), 0.0, aParameter, aParameter, 0.0);
        }
        aPoints->Add(aPoint);
      }
      const occ::handle<IntPatch_WLine> aLine = new IntPatch_WLine(aPoints, false);
      aLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);
      IntPatch_RstInt::PutVertexOnLine(aLine,
                                       aPlane,
                                       aDomain,
                                       aCrossPlane,
                                       !isReversed,
                                       Precision::Confusion());
      ASSERT_GE(aLine->NbPnts(), 3);
      EXPECT_DOUBLE_EQ(aLine->Point(1).Value().Y(), -aMargin);
      EXPECT_DOUBLE_EQ(aLine->Point(aLine->NbPnts()).Value().Y(), 1.0 + aMargin);
      bool hasFirst = false, hasLast = false;
      for (int i = 1; i <= aLine->NbVertex(); ++i)
      {
        const IntPatch_Point& aVertex = aLine->Vertex(i);
        if (isReversed ? !aVertex.IsOnDomS2() : !aVertex.IsOnDomS1())
        {
          continue;
        }
        const gp_Pnt& aPoint = aVertex.Value();
        EXPECT_NEAR(aPoint.X(), 0.0, Precision::Confusion());
        EXPECT_NEAR(aPoint.Z(), 0.0, Precision::Confusion());
        hasFirst = hasFirst || std::abs(aPoint.Y()) <= Precision::Confusion();
        hasLast  = hasLast || std::abs(aPoint.Y() - 1.0) <= Precision::Confusion();
      }
      EXPECT_TRUE(hasFirst);
      EXPECT_TRUE(hasLast);
    }
  }
}

namespace
{
//==================================================================================================

template <class TheLine, class TheMultiLine, class TheSurfaceFunction>
void checkResamplingTurn(TheSurfaceFunction& theSurfaces)
{
  struct Sample
  {
    std::array<double, 3> Parameters; // Positions along the intersection.
    bool                  IsMonotone; // Whether the path keeps its direction.
  };

  const std::array<Sample, 4> aSamples{{{{0.0, 0.4, 1.0}, true},
                                        {{0.0, 0.125, 1.0}, true},
                                        {{0.0, 0.875, 1.0}, true},
                                        {{0.0, 1.0, 0.0}, false}}};
  for (const auto& aSample : aSamples)
  {
    SCOPED_TRACE(aSample.Parameters[1]);
    const occ::handle<IntSurf_LineOn2S> aPoints = new IntSurf_LineOn2S();
    for (const double aY : aSample.Parameters)
    {
      IntSurf_PntOn2S aPoint;
      aPoint.SetValue(gp_Pnt(0.0, aY, 0.0), 0.0, aY, aY, 0.0);
      aPoints->Add(aPoint);
    }
    const occ::handle<TheLine> aLine             = new TheLine(aPoints, false);
    const int                  aNbInputPoints    = static_cast<int>(aSample.Parameters.size());
    constexpr int              aNbPointsToInsert = 3;
    const TheMultiLine         aSource(aLine,
                               &theSurfaces,
                               1,
                               2,
                               true,
                               true,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               true,
                               1,
                               aNbInputPoints);
    const TheMultiLine aResult = aSource.MakeMLBetween(1, aNbInputPoints, aNbPointsToInsert);
    const int          aCount  = aResult.LastPoint() - aResult.FirstPoint() + 1;
    if (!aSample.IsMonotone)
    {
      EXPECT_EQ(aCount, 1);
      continue;
    }
    ASSERT_GE(aCount, aNbInputPoints + aNbPointsToInsert);
    const TheMultiLine aRefined =
      aResult.MakeMLBetween(aResult.FirstPoint(), aResult.LastPoint(), aNbPointsToInsert);
    EXPECT_GE(aRefined.LastPoint() - aRefined.FirstPoint() + 1, aCount + aNbPointsToInsert);

    NCollection_Array1<gp_Pnt>   aPoint(1, 1);
    NCollection_Array1<gp_Pnt2d> aUV(1, 2);
    double                       aPreviousY = -1.0;
    std::array<bool, 3>          hasOriginal{};
    for (int i = aResult.FirstPoint(); i <= aResult.LastPoint(); ++i)
    {
      aResult.Value(i, aPoint, aUV);
      const double aY = aPoint.At(0).Y();
      EXPECT_GT(aY, aPreviousY);
      EXPECT_NEAR(aPoint.At(0).X(), 0.0, Precision::Confusion());
      EXPECT_NEAR(aPoint.At(0).Z(), 0.0, Precision::Confusion());
      EXPECT_NEAR(aUV.At(0).X(), 0.0, Precision::PConfusion());
      EXPECT_NEAR(aUV.At(0).Y(), aY, Precision::PConfusion());
      EXPECT_NEAR(aUV.At(1).X(), aY, Precision::PConfusion());
      EXPECT_NEAR(aUV.At(1).Y(), 0.0, Precision::PConfusion());
      aPreviousY = aY;
      for (size_t j = 0; j < hasOriginal.size(); ++j)
        hasOriginal[j] = hasOriginal[j] || aY == aSample.Parameters[j];
    }
    EXPECT_DOUBLE_EQ(aPreviousY, 1.0);
    for (size_t j = 0; j < hasOriginal.size(); ++j)
      EXPECT_TRUE(hasOriginal[j]) << aSample.Parameters[j];
  }
}

//==================================================================================================

template <class TheLine, class TheMultiLine, class TheSurfaceFunction>
void checkResamplingCircle(TheSurfaceFunction& theSurfaces, const bool theReversed)
{
  const gp_Cylinder                   aCylinder(gp_Ax3(), 1.0);
  const gp_Pln                        aPlane;
  const occ::handle<IntSurf_LineOn2S> aPoints = new IntSurf_LineOn2S();
  for (const double anAngle : {0.0, 0.3, 0.6})
  {
    const gp_Pnt    aPosition = ElSLib::Value(anAngle, 0.0, aCylinder);
    IntSurf_PntOn2S aPoint;
    aPoint.SetValue(aPosition, anAngle, 0.0, aPosition.X(), aPosition.Y());
    if (theReversed)
    {
      aPoint.SetValue(aPosition, aPosition.X(), aPosition.Y(), anAngle, 0.0);
    }
    aPoints->Add(aPoint);
  }
  const occ::handle<TheLine> aLine = new TheLine(aPoints, false);
  const TheMultiLine
    aSource(aLine, &theSurfaces, 1, 2, true, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, true, 1, 3);
  const TheMultiLine aResult = aSource.MakeMLBetween(1, 3, 3);
  ASSERT_GE(aResult.LastPoint() - aResult.FirstPoint() + 1, 6);
  NCollection_Array1<gp_Pnt>   aPoint(1, 1);
  NCollection_Array1<gp_Pnt2d> aUV(1, 2);
  for (int i = aResult.FirstPoint(); i <= aResult.LastPoint(); ++i)
  {
    aResult.Value(i, aPoint, aUV);
    const gp_Pnt2d& aCylinderUV = aUV.At(theReversed ? 1 : 0);
    const gp_Pnt2d& aPlaneUV    = aUV.At(theReversed ? 0 : 1);
    EXPECT_LE(aPoint.At(0).Distance(ElSLib::Value(aCylinderUV.X(), aCylinderUV.Y(), aCylinder)),
              Precision::Confusion());
    EXPECT_LE(aPoint.At(0).Distance(ElSLib::Value(aPlaneUV.X(), aPlaneUV.Y(), aPlane)),
              Precision::Confusion());
  }
}

} // namespace

TEST(ApproxInt_MultiLineTest, SurfaceResamplingSeparatesSpacingFromTurning)
{
  const occ::handle<Adaptor3d_Surface> aFirst =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp_Ax3(gp::Origin(), gp::DZ(), gp::DX()))),
                            -1.0,
                            1.0,
                            -1.0,
                            1.0);
  const occ::handle<Adaptor3d_Surface> aSecond =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp_Ax3(gp::Origin(), gp::DX(), gp::DY()))),
                            -1.0,
                            1.0,
                            -1.0,
                            1.0);
  GeomInt_ThePrmPrmSvSurfacesOfWLApprox aSurfaces(aFirst, aSecond);
  checkResamplingTurn<IntPatch_WLine, GeomInt_TheMultiLineOfWLApprox>(aSurfaces);
}

TEST(ApproxInt_MultiLineTest, FaceResamplingSeparatesSpacingFromTurning)
{
  BRepBuilderAPI_MakeFace aFirstFace(gp_Pln(gp_Ax3(gp::Origin(), gp::DZ(), gp::DX())),
                                     -1.0,
                                     1.0,
                                     -1.0,
                                     1.0);
  BRepBuilderAPI_MakeFace aSecondFace(gp_Pln(gp_Ax3(gp::Origin(), gp::DX(), gp::DY())),
                                      -1.0,
                                      1.0,
                                      -1.0,
                                      1.0);
  ASSERT_TRUE(aFirstFace.IsDone());
  ASSERT_TRUE(aSecondFace.IsDone());
  const BRepAdaptor_Surface              aFirst(aFirstFace.Face());
  const BRepAdaptor_Surface              aSecond(aSecondFace.Face());
  BRepApprox_ThePrmPrmSvSurfacesOfApprox aSurfaces(aFirst, aSecond);
  checkResamplingTurn<BRepApprox_ApproxLine, BRepApprox_TheMultiLineOfApprox>(aSurfaces);
}

TEST(ApproxInt_MultiLineTest, SurfaceResamplingKeepsPointsOnBothSurfaces)
{
  const IntSurf_Quadric                 aCylinder(gp_Cylinder(gp_Ax3(), 1.0));
  const occ::handle<Adaptor3d_Surface>  aPlane = new GeomAdaptor_Surface(new Geom_Plane(gp_Pln()));
  GeomInt_TheImpPrmSvSurfacesOfWLApprox aForward(aCylinder, aPlane);
  GeomInt_TheImpPrmSvSurfacesOfWLApprox aReversed(aPlane, aCylinder);
  checkResamplingCircle<IntPatch_WLine, GeomInt_TheMultiLineOfWLApprox>(aForward, false);
  checkResamplingCircle<IntPatch_WLine, GeomInt_TheMultiLineOfWLApprox>(aReversed, true);
}

TEST(ApproxInt_MultiLineTest, FaceResamplingKeepsPointsOnBothSurfaces)
{
  const IntSurf_Quadric   aCylinder(gp_Cylinder(gp_Ax3(), 1.0));
  BRepBuilderAPI_MakeFace aFace(gp_Pln(), -1.0, 1.0, -1.0, 1.0);
  ASSERT_TRUE(aFace.IsDone());
  const BRepAdaptor_Surface              aPlane(aFace.Face());
  BRepApprox_TheImpPrmSvSurfacesOfApprox aForward(aCylinder, aPlane);
  BRepApprox_TheImpPrmSvSurfacesOfApprox aReversed(aPlane, aCylinder);
  checkResamplingCircle<BRepApprox_ApproxLine, BRepApprox_TheMultiLineOfApprox>(aForward, false);
  checkResamplingCircle<BRepApprox_ApproxLine, BRepApprox_TheMultiLineOfApprox>(aReversed, true);
}

TEST(IntPatch_WalkingTest, ImplicitSurfaceApproximationKeepsEvaluationState)
{
  const occ::handle<Adaptor3d_Surface> aPlane = new GeomAdaptor_Surface(new Geom_Plane(gp_Pln()));
  checkImplicitApproximationCache<GeomInt_TheImpPrmSvSurfacesOfWLApprox>(aPlane);
}

TEST(IntPatch_WalkingTest, ImplicitFaceApproximationKeepsEvaluationState)
{
  BRepBuilderAPI_MakeFace aFace(gp_Pln(), -1.0, 1.0, -1.0, 1.0);
  ASSERT_TRUE(aFace.IsDone());
  const BRepAdaptor_Surface aPlane(aFace.Face());
  checkImplicitApproximationCache<BRepApprox_TheImpPrmSvSurfacesOfApprox>(aPlane);
}

namespace
{
//==================================================================================================

template <class TheLine, class TheApproximation, class TheSurface>
void checkApproximationFailure(const TheSurface& thePlane, const TheSurface& theCylinder)
{
  constexpr int    aNbPoints  = 5;
  constexpr double aTolerance = Precision::Confusion();
  TheApproximation anApproximation;
  anApproximation.SetParameters(aTolerance, aTolerance, 2, 2, 0, aNbPoints, false);
  for (int aRun = 0; aRun < 3; ++aRun)
  {
    SCOPED_TRACE(aRun);
    const bool                          toFit       = aRun == 1;
    const double                        anAngleSpan = toFit ? 0.001 : 2.0 * M_PI;
    const occ::handle<IntSurf_LineOn2S> aPoints     = new IntSurf_LineOn2S();
    for (int i = 0; i < aNbPoints; ++i)
    {
      const double    anAngle = anAngleSpan * i / (aNbPoints - 1);
      const double    aX = std::cos(anAngle), aY = std::sin(anAngle);
      IntSurf_PntOn2S aPoint;
      aPoint.SetValue(gp_Pnt(aX, aY, 0.0), aX, aY, anAngle, 0.0);
      aPoints->Add(aPoint);
    }
    const occ::handle<TheLine> aLine = new TheLine(aPoints, false);
    anApproximation.Perform(thePlane, theCylinder, aLine, true, false, false, 1, aNbPoints);
    ASSERT_EQ(anApproximation.IsDone(), toFit);
    if (!toFit)
    {
      continue;
    }
    const AppParCurves_MultiBSpCurve& aResult = anApproximation.Value(1);
    ASSERT_EQ(aResult.NbCurves(), 1);
    NCollection_Array1<gp_Pnt> aPoles(1, aResult.NbPoles());
    aResult.Curve(1, aPoles);
    const occ::handle<Geom_BSplineCurve> aCurve =
      new Geom_BSplineCurve(aPoles, aResult.Knots(), aResult.Multiplicities(), aResult.Degree());
    const GeomAdaptor_Curve anAdaptor(aCurve);
    constexpr int           aNbSamples = 16;
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aParameter =
        aCurve->FirstParameter()
        + (aCurve->LastParameter() - aCurve->FirstParameter()) * i / aNbSamples;
      const gp_Pnt aPoint = anAdaptor.EvalD0(aParameter);
      EXPECT_NEAR(std::hypot(aPoint.X(), aPoint.Y()), 1.0, aTolerance);
      EXPECT_NEAR(aPoint.Z(), 0.0, aTolerance);
    }
  }
}
} // namespace

TEST(ApproxInt_ApproxTest, SurfaceReportsUnreachedTolerance)
{
  const occ::handle<Adaptor3d_Surface> aPlane = new GeomAdaptor_Surface(new Geom_Plane(gp_Pln()));
  const occ::handle<Adaptor3d_Surface> aCylinder =
    new GeomAdaptor_Surface(new Geom_CylindricalSurface(gp_Cylinder(gp_Ax3(), 1.0)));
  checkApproximationFailure<IntPatch_WLine, GeomInt_WLApprox>(aPlane, aCylinder);
}

TEST(ApproxInt_ApproxTest, FaceReportsUnreachedTolerance)
{
  BRepBuilderAPI_MakeFace aPlaneFace(gp_Pln(), -2.0, 2.0, -2.0, 2.0);
  BRepBuilderAPI_MakeFace aCylinderFace(gp_Cylinder(gp_Ax3(), 1.0), 0.0, 2.0 * M_PI, -1.0, 1.0);
  ASSERT_TRUE(aPlaneFace.IsDone());
  ASSERT_TRUE(aCylinderFace.IsDone());
  const BRepAdaptor_Surface aPlane(aPlaneFace.Face()), aCylinder(aCylinderFace.Face());
  checkApproximationFailure<BRepApprox_ApproxLine, BRepApprox_Approx>(aPlane, aCylinder);
}

//==================================================================================================

TEST(IntPatch_WalkingTest, ShortSubrangesPreserveSurfaceAccuracy)
{
  const double aCoordinates[][3] = {
    {0, 0, -5.1},
    {4.07532146808326, 7.22269932110671, -5.09999999999999},
    {16.8843748832916, 4.96145448747163, -5.10000000000001},
    {38.1484191375938, 23.2187231680138, -5.09999999999999},
    {19.6238528888437, 58.7473213631888, -5.10000000000001},
    {5.47634115036741, 70.8683779090066, -5.09999999999998},
    {-13.7509651710097, 90.6382395206007, -5.10000000000001},
    {-29.3218813389176, 100.841909668458, -5.09999999999999},
    {-41.4368424510491, 103.083452055541, -5.1},
    {0, 0, 4.61170555704949},
    {3.82437075092074, 7.23978697999829, 4.6117055570495},
    {17.2027492773699, 5.3063333687735, 4.61170555704946},
    {36.6479742204899, 25.0801707707339, 4.61170555704954},
    {21.8254468649805, 57.2660375503142, 4.61170555704943},
    {2.93336679334539, 71.4995050502465, 4.6117055570495},
    {-14.498204405723, 89.7265715721086, 4.61170555704943},
    {-35.778413402781, 99.1737311786689, 4.6117055570495},
    {-46.2683679126628, 100.761464904549, 4.61170555704949},
    {0, 0, 12.2097326220285},
    {5.15231604531722, 6.25005766185982, 12.2097326220284},
    {17.1426842348232, 3.70035121338721, 12.2097326220286},
    {41.4937368567691, 16.4622793167896, 12.2097326220283},
    {29.7890220611776, 56.6228234952865, 12.2097326220287},
    {6.28432593930254, 73.4788071728172, 12.2097326220284},
    {-20.2145577990804, 89.9644310262281, 12.2097326220287},
    {-31.8337446014426, 100.748344865393, 12.2097326220285},
    {-43.4691187193245, 103.402372211472, 12.2097326220285},
    {0, 0, 26.9445459006783},
    {7.45413898898124, 1.96615204594949, 26.9445459006783},
    {18.6355102895789, 0.399448619127453, 26.9445459006782},
    {43.752012734361, 3.32860367537667, 26.9445459006784},
    {62.7000665876363, 29.03058877219, 26.9445459006781},
    {39.5238865888819, 69.8908248399692, 26.9445459006783},
    {4.57994160388073, 91.2207365984017, 26.9445459006782},
    {-3.15976601272166, 106.716028473289, 26.9445459006783},
    {-9.37254707971436, 112.24813744932, 26.9445459006783},
    {0, 0, 38.3300508581958},
    {5.81584363519715, -2.58717723289163, 38.3300508581957},
    {18.8051466067143, -5.10751507399171, 38.330050858196},
    {39.9258113429699, -9.07491835492431, 38.3300508581957},
    {63.4763437493813, -15.312788932262, 38.330050858196},
    {57.7355450456725, 65.9437632171674, 38.3300508581958},
    {5.98258524250968, 88.2174891571841, 38.3300508581959},
    {-9.01368561061726, 106.198342938881, 38.3300508581958},
    {-20.1534895952446, 108.795786209942, 38.3300508581958},
    {0, -0, 44.76591},
    {5.43621045502826, -2.983292965871, 44.76591},
    {18.7350256971381, -5.42246471868835, 44.7659099999999},
    {39.1135004609102, -10.8451112170494, 44.76591},
    {61.4720749824439, -18.4555336308943, 44.7659099999999},
    {61.9114733486087, 63.8593471742139, 44.76591},
    {6.73801587599916, 88.9340282920019, 44.76591},
    {-4.95616885353499, 106.571883518454, 44.7659099999999},
    {-15.7227798068713, 109.98183575093, 44.76591},
  };
  NCollection_Array2<gp_Pnt> aPoles(1, 6, 1, 9);
  for (size_t i = 0; i < aPoles.Size(); ++i)
  {
    aPoles.ChangeAt(i) = gp_Pnt(aCoordinates[i][0], aCoordinates[i][1], aCoordinates[i][2]);
  }
  const double aUKnotValues[] = {0.0, 0.259284280069896, 0.662667125329985, 1.0};
  const int    aUMultValues[] = {4, 1, 1, 4};
  const double aVKnotValues[] = {0.0,
                                 0.104073335977061,
                                 0.209990779146179,
                                 0.36468787043736,
                                 0.630152479120871,
                                 0.843214221453166,
                                 1.0};
  const int    aVMultValues[] = {4, 1, 1, 1, 1, 1, 4};
  const NCollection_Array1<double> aUKnots(aUKnotValues[0], 1, 4);
  const NCollection_Array1<double> aVKnots(aVKnotValues[0], 1, 7);
  const NCollection_Array1<int>    aUMults(aUMultValues[0], 1, 4);
  const NCollection_Array1<int>    aVMults(aVMultValues[0], 1, 7);
  const occ::handle<Geom_Surface>  aSpline =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
  const gp_Pnt aGeneratorPoints[] = {
    gp_Pnt(-1.3535533683419, -3.76402620594628, 0),
    gp_Pnt(-1.3535533683419, -3.76402620594628, 18.395644884786),
    gp_Pnt(-1.3535533683419, -3.76402620594628, 39.66591),
    gp_Pnt(-6.75302504833528, -18.7791363427977, 39.66591),
    gp_Pnt(-35.8691642610603, -99.7466944575763, 39.66591),
  };
  const NCollection_Array1<gp_Pnt> aGeneratorPoles(aGeneratorPoints[0], 1, 5);
  const occ::handle<Geom_Surface>  aRevolution =
    new Geom_SurfaceOfRevolution(new Geom_BezierCurve(aGeneratorPoles),
                                 gp_Ax1(gp::Origin(), gp::DZ()));
  GeomAPI_ProjectPointOnSurf aSplineProjection, aRevolutionProjection;
  aSplineProjection.Init(aSpline, 0.0, 1.0, 0.0, 1.0, Precision::Confusion());
  aRevolutionProjection.Init(aRevolution, 0.0, 2.0 * M_PI, 0.0, 1.0, Precision::Confusion());
  for (const bool isReversed : {false, true})
  {
    SCOPED_TRACE(isReversed);
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(isReversed ? aRevolution : aSpline,
                           isReversed ? aSpline : aRevolution,
                           Precision::Confusion(),
                           true);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbLines(), 1);
    const occ::handle<Geom_Curve>& aCurve = anIntersection.Line(1);
    ASSERT_FALSE(aCurve.IsNull());
    double aMaximum = 0.0;
    for (size_t i = 0; i <= 300; ++i)
    {
      const double aParameter =
        aCurve->FirstParameter()
        + (aCurve->LastParameter() - aCurve->FirstParameter()) * double(i) / 300.0;
      const gp_Pnt aPoint = aCurve->EvalD0(aParameter);
      aSplineProjection.Perform(aPoint);
      aRevolutionProjection.Perform(aPoint);
      ASSERT_TRUE(aSplineProjection.IsDone());
      ASSERT_TRUE(aRevolutionProjection.IsDone());
      ASSERT_GT(aSplineProjection.NbPoints(), 0);
      ASSERT_GT(aRevolutionProjection.NbPoints(), 0);
      aMaximum = std::max(
        {aMaximum, aSplineProjection.LowerDistance(), aRevolutionProjection.LowerDistance()});
    }
    EXPECT_LE(aMaximum, 2.0 * Precision::Confusion());
  }
}

//==================================================================================================

namespace
{
//==================================================================================================

void checkRevolutionIntersectionCoverage(const Geom_SurfaceOfRevolution& theFirst,
                                         const Geom_SurfaceOfRevolution& theSecond,
                                         const GeomInt_IntSS&            theIntersection)
{
  const occ::handle<Geom_Circle> aFirstCircle = occ::down_cast<Geom_Circle>(theFirst.BasisCurve());
  const occ::handle<Geom_Circle> aSecondCircle =
    occ::down_cast<Geom_Circle>(theSecond.BasisCurve());
  ASSERT_FALSE(aFirstCircle.IsNull());
  ASSERT_FALSE(aSecondCircle.IsNull());
  const std::array<gp_Pnt, 2> anAxes   = {theFirst.Axis().Location(), theSecond.Axis().Location()};
  const std::array<gp_Pnt, 2> aCentres = {aFirstCircle->Location(), aSecondCircle->Location()};
  ASSERT_DOUBLE_EQ(aFirstCircle->Radius(), aSecondCircle->Radius());
  ASSERT_DOUBLE_EQ(aCentres[0].Z(), aCentres[1].Z());
  for (const gp_Dir& aDirection : {theFirst.Axis().Direction(), theSecond.Axis().Direction()})
  {
    ASSERT_DOUBLE_EQ(aDirection.X(), 0.0);
    ASSERT_DOUBLE_EQ(aDirection.Y(), 0.0);
    ASSERT_DOUBLE_EQ(std::abs(aDirection.Z()), 1.0);
  }
  for (const gp_Dir& aNormal :
       {aFirstCircle->Axis().Direction(), aSecondCircle->Axis().Direction()})
  {
    ASSERT_DOUBLE_EQ(aNormal.X(), 0.0);
    ASSERT_DOUBLE_EQ(std::abs(aNormal.Y()), 1.0);
    ASSERT_DOUBLE_EQ(aNormal.Z(), 0.0);
  }

  // Each horizontal section contains two circles per surface. Their four pairs
  // give independent intersection points, including the tangent junctions.
  const double aRadius   = aFirstCircle->Radius();
  const double aDX       = anAxes[1].X() - anAxes[0].X();
  const double aDY       = anAxes[1].Y() - anAxes[0].Y();
  const double aDistance = std::hypot(aDX, aDY);
  ASSERT_GT(aDistance, 0.0);
  constexpr int                                                aNbIntervals = 300;
  NCollection_LinearVector<std::tuple<gp_Pnt, double, size_t>> aQueries;
  aQueries.Reserve(8 * (aNbIntervals + 1));
  for (int aLevel = 0; aLevel <= aNbIntervals; ++aLevel)
  {
    const double anAngle = -M_PI / 2.0 + M_PI * double(aLevel) / aNbIntervals;
    const double aHeight = aRadius * std::sin(anAngle);
    const double aSide   = aRadius * std::abs(std::cos(anAngle));
    for (size_t aBranch1 = 0; aBranch1 < 2; ++aBranch1)
    {
      for (size_t aBranch2 = 0; aBranch2 < 2; ++aBranch2)
      {
        const double aMeridian1 =
          aCentres[0].X() - anAxes[0].X() + (aBranch1 == 0 ? aSide : -aSide);
        const double aMeridian2 =
          aCentres[1].X() - anAxes[1].X() + (aBranch2 == 0 ? aSide : -aSide);
        const double aRadius1 = std::hypot(aMeridian1, aCentres[0].Y() - anAxes[0].Y());
        const double aRadius2 = std::hypot(aMeridian2, aCentres[1].Y() - anAxes[1].Y());
        if (aRadius1 + aRadius2 < aDistance || std::abs(aRadius1 - aRadius2) > aDistance)
        {
          continue;
        }
        const double anAlong =
          (aRadius1 * aRadius1 - aRadius2 * aRadius2 + aDistance * aDistance) / (2.0 * aDistance);
        const double aHalfSquared = (aRadius1 - anAlong) * (aRadius1 + anAlong);
        ASSERT_GE(aHalfSquared, 0.0);
        const double aHalf = std::sqrt(aHalfSquared);
        for (const double aSign : {-1.0, 1.0})
        {
          const gp_Pnt aPoint(anAxes[0].X() + (anAlong * aDX - aSign * aHalf * aDY) / aDistance,
                              anAxes[0].Y() + (anAlong * aDY + aSign * aHalf * aDX) / aDistance,
                              aCentres[0].Z() + aHeight);
          aQueries.Append(std::make_tuple(aPoint, RealLast(), 2 * aBranch1 + aBranch2));
        }
      }
    }
  }

  GeomAPI_ProjectPointOnCurve aProjection;
  for (int aCurveIndex = 1; aCurveIndex <= theIntersection.NbLines(); ++aCurveIndex)
  {
    const occ::handle<Geom_Curve>& aCurve = theIntersection.Line(aCurveIndex);
    ASSERT_FALSE(aCurve.IsNull());
    const double aFirst = aCurve->FirstParameter();
    const double aLast  = aCurve->LastParameter();
    aProjection.Init(aCurve, aFirst, aLast);
    for (auto& [aPoint, aMinimum, aBranch] : aQueries)
    {
      aMinimum = std::min({aMinimum,
                           aPoint.Distance(aCurve->EvalD0(aFirst)),
                           aPoint.Distance(aCurve->EvalD0(aLast))});
      aProjection.Perform(aPoint);
      if (aProjection.NbPoints() > 0)
      {
        aMinimum = std::min(aMinimum, aProjection.LowerDistance());
      }
    }
  }
  std::array<double, 4> aMaximum{};
  std::array<size_t, 4> aSampleCounts{};
  for (const auto& [aPoint, aMinimum, aBranch] : aQueries)
  {
    aMaximum[aBranch] = std::max(aMaximum[aBranch], aMinimum);
    ++aSampleCounts[aBranch];
  }
  for (size_t aBranch = 0; aBranch < aMaximum.size(); ++aBranch)
  {
    SCOPED_TRACE(aBranch);
    EXPECT_GT(aSampleCounts[aBranch], 0u);
    EXPECT_LE(aMaximum[aBranch], 2.0 * Precision::Confusion());
  }
}
} // namespace

//==================================================================================================

TEST(IntPatch_WalkingTest, TangentReversalPreservesIntersectionBranch)
{
  const occ::handle<Geom_SurfaceOfRevolution> aFirst = new Geom_SurfaceOfRevolution(
    new Geom_Circle(gp_Ax2(gp_Pnt(0.4959754943848, 5.824995994568, -17.72396278381),
                           gp_Dir(0, 1, 0),
                           gp_Dir(0.999847661852972, 0, -0.0174543142845826)),
                    0.2999649047852),
    gp_Ax1(gp_Pnt(2.150002479553, 5.824993610382, -16.29999542236), gp_Dir(0, 0, -1)));
  const occ::handle<Geom_SurfaceOfRevolution> aSecond = new Geom_SurfaceOfRevolution(
    new Geom_Circle(gp_Ax2(gp_Pnt(0.4959752559662, 3.074996232986, -17.72396278381),
                           gp_Dir(0, 1, 0),
                           gp_Dir(0.999847662095117, 0, -0.0174543004136144)),
                    0.2999649047852),
    gp_Ax1(gp_Pnt(2.150002002716, 3.074993848801, -16.29999542236), gp_Dir(0, 0, -1)));
  GeomAPI_ProjectPointOnSurf aProjection1, aProjection2;
  aProjection1.Init(aFirst, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, Precision::Confusion());
  aProjection2.Init(aSecond, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, Precision::Confusion());
  for (const bool isReversed : {false, true})
  {
    SCOPED_TRACE(isReversed);
    GeomInt_IntSS anIntersection;
    anIntersection.Perform(isReversed ? aSecond : aFirst,
                           isReversed ? aFirst : aSecond,
                           Precision::Confusion(),
                           true);
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_GT(anIntersection.NbLines(), 0);
    checkRevolutionIntersectionCoverage(*aFirst, *aSecond, anIntersection);
    for (int aCurveIndex = 1; aCurveIndex <= anIntersection.NbLines(); ++aCurveIndex)
    {
      SCOPED_TRACE(aCurveIndex);
      const occ::handle<Geom_Curve>& aCurve = anIntersection.Line(aCurveIndex);
      ASSERT_FALSE(aCurve.IsNull());
      double aMaximum = 0.0;
      for (int aPointIndex = 0; aPointIndex <= 300; ++aPointIndex)
      {
        const double aParameter =
          aCurve->FirstParameter()
          + (aCurve->LastParameter() - aCurve->FirstParameter()) * aPointIndex / 300.0;
        const gp_Pnt aPoint = aCurve->EvalD0(aParameter);
        aProjection1.Perform(aPoint);
        aProjection2.Perform(aPoint);
        ASSERT_TRUE(aProjection1.IsDone());
        ASSERT_TRUE(aProjection2.IsDone());
        ASSERT_GT(aProjection1.NbPoints(), 0);
        ASSERT_GT(aProjection2.NbPoints(), 0);
        aMaximum = std::max({aMaximum, aProjection1.LowerDistance(), aProjection2.LowerDistance()});
      }
      EXPECT_LE(aMaximum, 2.0 * Precision::Confusion());
    }
  }
}

//==================================================================================================

TEST(IntPatch_WalkingTest, BoundaryCorrectionRespectsDeflection)
{
  // The intersection is (x, x*x, 0). It can touch or cross the plane's lower V bound.
  NCollection_Array2<gp_Pnt> aCylinderPoles(1, 3, 1, 2);
  for (int i = 1; i <= 3; ++i)
  {
    const double anX     = double(i - 2);
    const double aY      = i == 2 ? -1.0 : 1.0;
    aCylinderPoles(i, 1) = gp_Pnt(anX, aY, -1.0);
    aCylinderPoles(i, 2) = gp_Pnt(anX, aY, 1.0);
  }
  const occ::handle<GeomAdaptor_Surface> aCylinder =
    new GeomAdaptor_Surface(new Geom_BezierSurface(aCylinderPoles));
  constexpr double aDeflection = 1.e-5;
  for (const double aLowerY : {-0.01, 0.0, 0.0001, 0.01})
  {
    SCOPED_TRACE(aLowerY);
    NCollection_Array2<gp_Pnt> aPlanePoles(1, 2, 1, 2);
    aPlanePoles(1, 1) = gp_Pnt(-1.0, aLowerY, 0.0);
    aPlanePoles(1, 2) = gp_Pnt(-1.0, 1.0, 0.0);
    aPlanePoles(2, 1) = gp_Pnt(9.0, aLowerY, 0.0);
    aPlanePoles(2, 2) = gp_Pnt(9.0, 1.0, 0.0);
    const occ::handle<GeomAdaptor_Surface> aPlane =
      new GeomAdaptor_Surface(new Geom_BezierSurface(aPlanePoles));
    const double aStartY = (0.64 - aLowerY) / (1.0 - aLowerY);
    for (const bool isReversed : {false, true})
    {
      SCOPED_TRACE(isReversed);
      for (const double aStep : {0.001, 0.01, 0.05, 0.1})
      {
        SCOPED_TRACE(aStep);
        NCollection_Array1<double> aParameters(1, 4);
        aParameters(1) = isReversed ? 0.1 : 0.02;
        aParameters(3) = isReversed ? 0.02 : 0.1;
        aParameters(2) = isReversed ? 0.5 : aStartY;
        aParameters(4) = isReversed ? aStartY : 0.5;
        IntWalk_PWalking aWalk(isReversed ? aCylinder : aPlane,
                               isReversed ? aPlane : aCylinder,
                               Precision::Confusion(),
                               Precision::Confusion(),
                               aDeflection,
                               aStep);
        aWalk.Perform(aParameters, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
        ASSERT_TRUE(aWalk.IsDone());
        ASSERT_GT(aWalk.NbPoints(), 2);
        double aMinX = 1.0, aMaxX = -1.0, aMaximumDeflection = 0.0;
        for (int i = 1; i <= aWalk.NbPoints(); ++i)
        {
          const gp_Pnt& aPoint = aWalk.Value(i).Value();
          EXPECT_NEAR(aPoint.Y(), aPoint.X() * aPoint.X(), Precision::Confusion());
          EXPECT_NEAR(aPoint.Z(), 0.0, Precision::Confusion());
          EXPECT_GE(aPoint.Y(), aLowerY - Precision::Confusion());
          aMinX = std::min(aMinX, aPoint.X());
          aMaxX = std::max(aMaxX, aPoint.X());
          if (i > 1)
          {
            const gp_Pnt& aPrevious = aWalk.Value(i - 1).Value();
            const double  aMiddleX  = 0.5 * (aPrevious.X() + aPoint.X());
            const gp_Pnt  aMiddle(aMiddleX, aMiddleX * aMiddleX, 0.0);
            const gp_Vec  aChord(aPrevious, aPoint), aToMiddle(aPrevious, aMiddle);
            if (aChord.Magnitude() > gp::Resolution())
            {
              aMaximumDeflection =
                std::max(aMaximumDeflection,
                         aChord.Crossed(aToMiddle).Magnitude() / aChord.Magnitude());
            }
          }
        }
        EXPECT_NEAR(aMinX, -1.0, Precision::Confusion());
        EXPECT_NEAR(aMaxX, aLowerY >= 0.0 ? -std::sqrt(aLowerY) : 1.0, Precision::Confusion());
        EXPECT_LE(aMaximumDeflection, aDeflection);
      }
    }
  }
}

namespace
{
//==================================================================================================

occ::handle<Geom_Surface> makeResamplingBezierSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 2);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      aPoles(i, j) = gp_Pnt(50.0 * (i - 1), (i == 2 ? -2.0 : 0.0) + j - 1, 0.0);
    }
  }
  // The straight intersection Y = 0 has V = 4 * U * (1 - U) on this surface.
  return new Geom_BezierSurface(aPoles);
}

//==================================================================================================

template <class TheLine, class TheMultiLine, class TheSurfaceFunction>
void checkResamplingCurvedParameters(TheSurfaceFunction& theSurfaces, const bool theReversed)
{
  const occ::handle<IntSurf_LineOn2S> aPoints = new IntSurf_LineOn2S();
  for (const double aU : {0.0, 0.5, 1.0})
  {
    const double    aV = 4.0 * aU * (1.0 - aU);
    IntSurf_PntOn2S aPoint;
    const gp_Pnt    aPosition(100.0 * aU, 0.0, 0.0);
    if (theReversed)
    {
      aPoint.SetValue(aPosition, aPosition.X(), 0.0, aU, aV);
    }
    else
    {
      aPoint.SetValue(aPosition, aU, aV, aPosition.X(), 0.0);
    }
    aPoints->Add(aPoint);
  }
  const occ::handle<TheLine> aLine = new TheLine(aPoints, false);
  const TheMultiLine
    aSource(aLine, &theSurfaces, 1, 2, true, true, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, true, 1, 3);
  const TheMultiLine aResult = aSource.MakeMLBetween(1, 3, 3);
  ASSERT_GE(aResult.LastPoint() - aResult.FirstPoint() + 1, 6);
  const TheMultiLine aRefined = aResult.MakeMLBetween(aResult.FirstPoint(), aResult.LastPoint(), 3);
  EXPECT_GT(aRefined.LastPoint() - aRefined.FirstPoint(),
            aResult.LastPoint() - aResult.FirstPoint());

  NCollection_Array1<gp_Pnt>   aPoint(1, 1);
  NCollection_Array1<gp_Pnt2d> aUV(1, 2);
  double                       aPreviousX = -1.0;
  std::array<bool, 3>          hasOriginal{};
  for (int i = aResult.FirstPoint(); i <= aResult.LastPoint(); ++i)
  {
    aResult.Value(i, aPoint, aUV);
    const gp_Pnt&   aPosition = aPoint.At(0);
    const gp_Pnt2d& aBezierUV = aUV.At(theReversed ? 1 : 0);
    const gp_Pnt2d& aPlaneUV  = aUV.At(theReversed ? 0 : 1);
    EXPECT_GT(aPosition.X(), aPreviousX);
    EXPECT_NEAR(aPosition.Y(), 0.0, Precision::Confusion());
    EXPECT_NEAR(aPosition.Z(), 0.0, Precision::Confusion());
    EXPECT_NEAR(aPosition.X(), 100.0 * aBezierUV.X(), Precision::Confusion());
    EXPECT_NEAR(aBezierUV.Y(),
                4.0 * aBezierUV.X() * (1.0 - aBezierUV.X()),
                Precision::PConfusion());
    EXPECT_NEAR(aPlaneUV.X(), aPosition.X(), Precision::PConfusion());
    EXPECT_NEAR(aPlaneUV.Y(), 0.0, Precision::PConfusion());
    for (size_t j = 0; j < hasOriginal.size(); ++j)
    {
      hasOriginal[j] = hasOriginal[j] || aPosition.X() == 50.0 * j;
    }
    aPreviousX = aPosition.X();
  }
  EXPECT_DOUBLE_EQ(aPreviousX, 100.0);
  for (const bool hasPoint : hasOriginal)
  {
    EXPECT_TRUE(hasPoint);
  }
}
} // namespace

TEST(ApproxInt_MultiLineTest, SurfaceResamplingUsesSpatialTurning)
{
  const occ::handle<Adaptor3d_Surface> aBezier =
    new GeomAdaptor_Surface(makeResamplingBezierSurface());
  const occ::handle<Adaptor3d_Surface> aPlane =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp_Ax3(gp::Origin(), gp::DY(), gp::DX()))));
  GeomInt_ThePrmPrmSvSurfacesOfWLApprox aForward(aBezier, aPlane);
  GeomInt_ThePrmPrmSvSurfacesOfWLApprox aReversed(aPlane, aBezier);
  checkResamplingCurvedParameters<IntPatch_WLine, GeomInt_TheMultiLineOfWLApprox>(aForward, false);
  checkResamplingCurvedParameters<IntPatch_WLine, GeomInt_TheMultiLineOfWLApprox>(aReversed, true);
}

TEST(ApproxInt_MultiLineTest, FaceResamplingUsesSpatialTurning)
{
  BRepBuilderAPI_MakeFace aBezierFace(makeResamplingBezierSurface(), Precision::Confusion());
  BRepBuilderAPI_MakeFace aPlaneFace(gp_Pln(gp_Ax3(gp::Origin(), gp::DY(), gp::DX())),
                                     0.0,
                                     100.0,
                                     -1.0,
                                     1.0);
  ASSERT_TRUE(aBezierFace.IsDone());
  ASSERT_TRUE(aPlaneFace.IsDone());
  const BRepAdaptor_Surface              aBezier(aBezierFace.Face());
  const BRepAdaptor_Surface              aPlane(aPlaneFace.Face());
  BRepApprox_ThePrmPrmSvSurfacesOfApprox aForward(aBezier, aPlane);
  BRepApprox_ThePrmPrmSvSurfacesOfApprox aReversed(aPlane, aBezier);
  checkResamplingCurvedParameters<BRepApprox_ApproxLine, BRepApprox_TheMultiLineOfApprox>(aForward,
                                                                                          false);
  checkResamplingCurvedParameters<BRepApprox_ApproxLine, BRepApprox_TheMultiLineOfApprox>(aReversed,
                                                                                          true);
}
