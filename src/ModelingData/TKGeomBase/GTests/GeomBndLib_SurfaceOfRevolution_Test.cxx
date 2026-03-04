// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomBndLib_Surface.hxx>

#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper: verify new box is no larger than old box (tighter or equal).
void ExpectNoLarger(const Bnd_Box& theNew, const Bnd_Box& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN, aZminN, aZmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO, aZminO, aZmaxO] = theOld.Get();
  EXPECT_GE(aXminN, aXminO - theTol) << "New Xmin is smaller than old";
  EXPECT_LE(aXmaxN, aXmaxO + theTol) << "New Xmax is larger than old";
  EXPECT_GE(aYminN, aYminO - theTol) << "New Ymin is smaller than old";
  EXPECT_LE(aYmaxN, aYmaxO + theTol) << "New Ymax is larger than old";
  EXPECT_GE(aZminN, aZminO - theTol) << "New Zmin is smaller than old";
  EXPECT_LE(aZmaxN, aZmaxO + theTol) << "New Zmax is larger than old";
}

//! Helper: verify box contains all sampled points of the surface.
void ExpectContainsSurface(const Bnd_Box&                          theBox,
                           const Handle(Geom_SurfaceOfRevolution)& theSurf,
                           const double                            theUMin,
                           const double                            theUMax,
                           const double                            theVMin,
                           const double                            theVMax,
                           const int                               theNbU,
                           const int                               theNbV)
{
  double aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.;
  theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  const double aUStep = (theUMax - theUMin) / theNbU;
  const double aVStep = (theVMax - theVMin) / theNbV;
  for (int i = 0; i <= theNbU; ++i)
  {
    const double aU = theUMin + i * aUStep;
    for (int j = 0; j <= theNbV; ++j)
    {
      const double aV  = theVMin + j * aVStep;
      const gp_Pnt aPnt = theSurf->Value(aU, aV);
      EXPECT_GE(aPnt.X(), aXmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.X(), aXmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Y(), aYmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Y(), aYmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_GE(aPnt.Z(), aZmin - Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
      EXPECT_LE(aPnt.Z(), aZmax + Precision::Confusion())
        << "Point outside box at U=" << aU << " V=" << aV;
    }
  }
}

} // namespace

// =========================================================================
// Line around Z axis (creates cylinder-like surface)
// =========================================================================

TEST(GeomBndLib_RevolutionTest, LineAroundZ_Full_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 10.0, 36, 10);
}

TEST(GeomBndLib_RevolutionTest, LineAroundZ_HalfTurn_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, M_PI, 0.0, 5.0, 18, 5);
}

TEST(GeomBndLib_RevolutionTest, InfiniteV_ConservativeWholeBox)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0,
                                   2.0 * M_PI,
                                   -Precision::Infinite(),
                                   Precision::Infinite(),
                                   Precision::Confusion(),
                                   aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor,
                         0.0,
                         2.0 * M_PI,
                         -Precision::Infinite(),
                         Precision::Infinite(),
                         Precision::Confusion(),
                         anOldBox);

  EXPECT_TRUE(aNewBox.IsWhole());
  EXPECT_EQ(aNewBox.IsWhole(), anOldBox.IsWhole());
}

TEST(GeomBndLib_RevolutionTest, LineAroundZ_QuarterTurn_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, M_PI / 2.0, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI / 2.0, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, M_PI / 2.0, 0.0, 5.0, 9, 5);
}

// =========================================================================
// Line around X axis
// =========================================================================

TEST(GeomBndLib_RevolutionTest, LineAroundX_Full_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0.0, 3.0, 0.0), gp::DX());
  gp_Ax1 anAxis(gp::Origin(), gp::DX());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 8.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 8.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 8.0, 36, 10);
}

// =========================================================================
// Line around tilted axis
// =========================================================================

TEST(GeomBndLib_RevolutionTest, LineAroundTiltedAxis_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(4.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp_Dir(1.0, 1.0, 0.0));
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 5.0, 36, 10);
}

// =========================================================================
// Circle around Z axis (creates torus-like surface)
// =========================================================================

TEST(GeomBndLib_RevolutionTest, CircleAroundZ_Full_CompareWithBndLib)
{
  // Circle in XZ plane, center at (5, 0, 0), radius 2
  gp_Ax2 aCircAx(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX());
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCircAx, 2.0);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aCircle, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 36, 36);
}

TEST(GeomBndLib_RevolutionTest, CircleAroundZ_HalfTurn_CompareWithBndLib)
{
  gp_Ax2 aCircAx(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX());
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCircAx, 2.0);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aCircle, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, M_PI, 0.0, 2.0 * M_PI,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, 2.0 * M_PI,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, M_PI, 0.0, 2.0 * M_PI, 18, 36);
}

// =========================================================================
// Parabola around Z axis
// =========================================================================

TEST(GeomBndLib_RevolutionTest, ParabolaAroundZ_CompareWithBndLib)
{
  Handle(Geom_Parabola) aParab = new Geom_Parabola(gp::XOY(), 2.0);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aParab, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, -3.0, 3.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, -3.0, 3.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, -3.0, 3.0, 36, 10);
}

// =========================================================================
// Ellipse around Z axis
// =========================================================================

TEST(GeomBndLib_RevolutionTest, EllipseAroundZ_CompareWithBndLib)
{
  // Ellipse in XZ plane
  gp_Ax2 anElAx(gp_Pnt(5.0, 0.0, 0.0), gp::DY(), gp::DX());
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(anElAx, 3.0, 1.5);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(anEllipse, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 36, 36);
}

// =========================================================================
// BSpline curve around Z axis
// =========================================================================

TEST(GeomBndLib_RevolutionTest, BSplineAroundZ_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(3.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(5.0, 0.0, 3.0));
  aPoles.SetValue(3, gp_Pnt(4.0, 0.0, 7.0));
  aPoles.SetValue(4, gp_Pnt(6.0, 0.0, 10.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom_BSplineCurve) aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aBSpl, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 1.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 1.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 1.0, 36, 10);
}

TEST(GeomBndLib_RevolutionTest, BSplineAroundZ_HalfTurn_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(3.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(5.0, 0.0, 3.0));
  aPoles.SetValue(3, gp_Pnt(4.0, 0.0, 7.0));
  aPoles.SetValue(4, gp_Pnt(6.0, 0.0, 10.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom_BSplineCurve) aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aBSpl, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, M_PI, 0.0, 1.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, M_PI, 0.0, 1.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, M_PI, 0.0, 1.0, 18, 10);
}

// =========================================================================
// Offset line from axis (different radius)
// =========================================================================

TEST(GeomBndLib_RevolutionTest, LineSmallRadius_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0.5, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, -5.0, 5.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, -5.0, 5.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, -5.0, 5.0, 36, 10);
}

TEST(GeomBndLib_RevolutionTest, LineLargeRadius_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(100.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 0.0, 10.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 0.0, 10.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 0.0, 10.0, 36, 10);
}

// =========================================================================
// V-range partial
// =========================================================================

TEST(GeomBndLib_RevolutionTest, LineAroundZ_PartialV_CompareWithBndLib)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp::DZ());
  gp_Ax1 anAxis(gp::Origin(), gp::DZ());
  Handle(Geom_SurfaceOfRevolution) aRevSurf = new Geom_SurfaceOfRevolution(aLine, anAxis);
  GeomAdaptor_Surface              anAdaptor(aRevSurf);

  // Only part of V range
  Bnd_Box aNewBox;
  GeomBndLib_Surface(aRevSurf).Add(0.0, 2.0 * M_PI, 3.0, 7.0,
                                   Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_AddSurface::Add(anAdaptor, 0.0, 2.0 * M_PI, 3.0, 7.0,
                         Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, 0.5);
  ExpectContainsSurface(aNewBox, aRevSurf, 0.0, 2.0 * M_PI, 3.0, 7.0, 36, 10);
}
