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

#include <Geom_BSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;

gp_Trsf createTranslation(const double theDx, const double theDy, const double theDz)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(theDx, theDy, theDz));
  return aTrsf;
}

gp_Trsf createRotationY(const double theAngle)
{
  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), theAngle);
  return aTrsf;
}

occ::handle<Geom_BSplineSurface> createBSplineSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(1.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(1.0, 1.0, 1.0));

  NCollection_Array1<double> aUKnots(1, 2);
  NCollection_Array1<double> aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, 2);
  NCollection_Array1<int>    aVMults(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  aUMults.SetValue(1, 2);
  aUMults.SetValue(2, 2);
  aVMults.SetValue(1, 2);
  aVMults.SetValue(2, 2);
  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
}
} // namespace

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, IdentityTransformUsesOriginalSurface)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp::XOY()));

  GeomAdaptor_TransformedSurface anAdaptor(aPlane, gp_Trsf());

  EXPECT_EQ(anAdaptor.GeomSurfaceOriginal(), aPlane);
  EXPECT_EQ(anAdaptor.GeomSurfaceTransformed(), aPlane);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, PlaneCachesTransformedSurface)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp::XOY()));

  GeomAdaptor_TransformedSurface anAdaptor(aPlane, createTranslation(0.0, 0.0, 5.0));

  const occ::handle<Geom_Surface>& aFirst  = anAdaptor.GeomSurfaceTransformed();
  const occ::handle<Geom_Surface>& aSecond = anAdaptor.GeomSurfaceTransformed();
  EXPECT_EQ(aFirst, aSecond);

  const gp_Pln aCachedPlane = anAdaptor.Plane();
  EXPECT_NEAR(aCachedPlane.Location().Z(), 5.0, THE_TOLERANCE);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, BSplineCachePreservedByShallowCopy)
{
  GeomAdaptor_TransformedSurface anAdaptor(createBSplineSurface(),
                                           createTranslation(4.0, 0.0, 0.0));

  const occ::handle<Geom_Surface>&       aSurface = anAdaptor.GeomSurfaceTransformed();
  const occ::handle<Geom_BSplineSurface> aBSpline = anAdaptor.BSpline();

  const occ::handle<Adaptor3d_Surface>              aCopyBase = anAdaptor.ShallowCopy();
  const occ::handle<GeomAdaptor_TransformedSurface> aCopy =
    occ::down_cast<GeomAdaptor_TransformedSurface>(aCopyBase);
  ASSERT_FALSE(aCopy.IsNull());

  EXPECT_EQ(aCopy->GeomSurfaceTransformed(), aSurface);
  EXPECT_EQ(aCopy->BSpline(), aBSpline);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, SetTrsfRebuildsCache)
{
  occ::handle<Geom_Plane>        aPlane = new Geom_Plane(gp_Pln(gp::XOY()));
  GeomAdaptor_TransformedSurface anAdaptor(aPlane, createTranslation(0.0, 0.0, 1.0));

  const occ::handle<Geom_Surface> aFirst = anAdaptor.GeomSurfaceTransformed();
  anAdaptor.SetTrsf(createTranslation(0.0, 0.0, 3.0));
  const occ::handle<Geom_Surface> aSecond = anAdaptor.GeomSurfaceTransformed();

  EXPECT_NE(aFirst, aSecond);
  EXPECT_NEAR(anAdaptor.Plane().Location().Z(), 3.0, THE_TOLERANCE);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, LoadRebuildsCache)
{
  occ::handle<Geom_Plane>        aPlane = new Geom_Plane(gp_Pln(gp::XOY()));
  GeomAdaptor_TransformedSurface anAdaptor(aPlane, createTranslation(0.0, 0.0, 2.0));

  const occ::handle<Geom_Surface> aFirst = anAdaptor.GeomSurfaceTransformed();

  gp_Ax3                             aSphereAx(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(aSphereAx, 3.0);
  anAdaptor.Load(aSphere);

  const occ::handle<Geom_Surface> aSecond = anAdaptor.GeomSurfaceTransformed();
  EXPECT_NE(aFirst, aSecond);
  EXPECT_EQ(anAdaptor.GetType(), GeomAbs_Sphere);
  EXPECT_NEAR(anAdaptor.Sphere().Radius(), 3.0, THE_TOLERANCE);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, ExtrusionCachesDirectionAndBasisCurve)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  occ::handle<Geom_SurfaceOfLinearExtrusion> anExt =
    new Geom_SurfaceOfLinearExtrusion(aLine, gp_Dir(0.0, 0.0, 1.0));

  GeomAdaptor_TransformedSurface anAdaptor(anExt, createRotationY(M_PI / 2.0));

  const gp_Dir aDirection = anAdaptor.Direction();
  EXPECT_NEAR(aDirection.X(), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aDirection.Y(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aDirection.Z(), 0.0, THE_TOLERANCE);

  const occ::handle<Adaptor3d_Curve> aFirstCurve  = anAdaptor.BasisCurve();
  const occ::handle<Adaptor3d_Curve> aSecondCurve = anAdaptor.BasisCurve();
  EXPECT_EQ(aFirstCurve, aSecondCurve);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, RevolutionCachesAxisAndBasisCurve)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_SurfaceOfRevolution> aRev =
    new Geom_SurfaceOfRevolution(aLine, gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));

  GeomAdaptor_TransformedSurface anAdaptor(aRev, createRotationY(M_PI / 2.0));

  const gp_Ax1 aAxis = anAdaptor.AxeOfRevolution();
  EXPECT_NEAR(aAxis.Direction().X(), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aAxis.Direction().Y(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aAxis.Direction().Z(), 0.0, THE_TOLERANCE);

  EXPECT_EQ(anAdaptor.BasisCurve(), anAdaptor.BasisCurve());
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, OffsetCachesBasisSurfaceAndOffsetValue)
{
  occ::handle<Geom_Plane>         aPlane   = new Geom_Plane(gp_Pln(gp::XOY()));
  occ::handle<Geom_OffsetSurface> anOffset = new Geom_OffsetSurface(aPlane, 2.5);

  GeomAdaptor_TransformedSurface anAdaptor(anOffset, createTranslation(0.0, 0.0, 3.0));

  const occ::handle<Adaptor3d_Surface> aFirstBasis  = anAdaptor.BasisSurface();
  const occ::handle<Adaptor3d_Surface> aSecondBasis = anAdaptor.BasisSurface();
  EXPECT_EQ(aFirstBasis, aSecondBasis);
  EXPECT_NEAR(anAdaptor.OffsetValue(), 2.5, THE_TOLERANCE);
}

//=================================================================================================

TEST(GeomAdaptor_TransformedSurfaceTest, GridEvalUsesTransformedGeometryOnlyOnce)
{
  occ::handle<Geom_Plane>        aPlane = new Geom_Plane(gp_Pln(gp::XOY()));
  GeomAdaptor_TransformedSurface anAdaptor(aPlane, createTranslation(0.0, 0.0, 5.0));

  GeomGridEval_Surface anEval(anAdaptor);

  NCollection_Array1<double> aUParams(1, 2);
  NCollection_Array1<double> aVParams(1, 2);
  aUParams.SetValue(1, 0.0);
  aUParams.SetValue(2, 1.0);
  aVParams.SetValue(1, 0.0);
  aVParams.SetValue(2, 1.0);

  const NCollection_Array2<gp_Pnt> aGrid = anEval.EvaluateGrid(aUParams, aVParams);
  EXPECT_NEAR(aGrid.Value(1, 1).Z(), 5.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2, 2).Z(), 5.0, THE_TOLERANCE);
}
