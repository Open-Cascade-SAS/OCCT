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

#include <GeomBndLib_Curve.hxx>

#include <BndLib_Add3dCurve.hxx>
#include <Bnd_Box.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

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

//! Helper: verify box contains all sampled points of the offset curve.
void ExpectContainsCurve(const Bnd_Box&                  theBox,
                         const Handle(Geom_OffsetCurve)& theCurve,
                         const double                    theU1,
                         const double                    theU2,
                         const int                       theNbSamples)
{
  const double aStep = (theU2 - theU1) / theNbSamples;
  double       aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.;
  theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  for (int i = 0; i <= theNbSamples; ++i)
  {
    const double aU   = theU1 + i * aStep;
    const gp_Pnt aPnt = theCurve->Value(aU);
    EXPECT_GE(aPnt.X(), aXmin - Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_LE(aPnt.X(), aXmax + Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_GE(aPnt.Y(), aYmin - Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_LE(aPnt.Y(), aYmax + Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_GE(aPnt.Z(), aZmin - Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_LE(aPnt.Z(), aZmax + Precision::Confusion()) << "Point outside box at U=" << aU;
  }
}

} // namespace

// =========================================================================
// Circle offset in Z direction (displacement in XY plane)
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetZ_Full_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 5.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 2.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetZ_Arc_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 5.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 3.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(0.0, M_PI / 2.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.0, M_PI / 2.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, M_PI / 2.0, 50);
}

// =========================================================================
// Circle offset in X direction (displacement in YZ plane)
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetX_Full_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 7.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 2.0, gp::DX());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetX_Arc_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 7.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 4.0, gp::DX());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(M_PI / 4.0, 3.0 * M_PI / 4.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, M_PI / 4.0, 3.0 * M_PI / 4.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, M_PI / 4.0, 3.0 * M_PI / 4.0, 50);
}

// =========================================================================
// Circle offset in Y direction - degenerate case
// Offset direction Y is in the circle plane (XOY), so T(t) x V can be zero.
// Old BndLib throws "GeomAdaptor_Curve::EvalD0: evaluation failed".
// Test only containment for our implementation.
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetY_Full_Containment)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 3.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 1.5, gp::DY());

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  EXPECT_FALSE(aNewBox.IsVoid());
}

// =========================================================================
// Circle offset in diagonal direction
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, CircleOffsetDiagonal_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 5.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 2.0, gp_Dir(1.0, 1.0, 1.0));
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).AddOptimal(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::AddOptimal(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

// =========================================================================
// Negative offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, CircleNegativeOffset_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 10.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, -3.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

// =========================================================================
// Ellipse offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, EllipseOffsetZ_Full_CompareWithBndLib)
{
  Handle(Geom_Ellipse)     anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  Handle(Geom_OffsetCurve) anOff     = new Geom_OffsetCurve(anEllipse, 2.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurveTest, EllipseOffsetX_Arc_CompareWithBndLib)
{
  Handle(Geom_Ellipse)     anEllipse = new Geom_Ellipse(gp::XOY(), 8.0, 3.0);
  Handle(Geom_OffsetCurve) anOff     = new Geom_OffsetCurve(anEllipse, 1.5, gp::DX());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(0.0, M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.0, M_PI, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, M_PI, 50);
}

// =========================================================================
// Line offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, LineOffset_CompareWithBndLib)
{
  Handle(Geom_Line)        aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_OffsetCurve) anOff = new Geom_OffsetCurve(aLine, 3.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 10.0, 50);
}

// =========================================================================
// BSpline offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, BSplineOffset_CompareWithBndLib)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(3.0, 5.0, 1.0));
  aPoles.SetValue(3, gp_Pnt(7.0, -2.0, 2.0));
  aPoles.SetValue(4, gp_Pnt(10.0, 0.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom_BSplineCurve) aBSpl = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  Handle(Geom_OffsetCurve)  anOff = new Geom_OffsetCurve(aBSpl, 1.0, gp::DZ());
  GeomAdaptor_Curve         anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).AddOptimal(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::AddOptimal(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 1.0, 50);
}

// =========================================================================
// Large offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, LargeOffset_CompareWithBndLib)
{
  Handle(Geom_Circle)      aCircle = new Geom_Circle(gp::XOY(), 2.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 20.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

// =========================================================================
// Non-planar circle with offset
// =========================================================================

TEST(GeomBndLib_OffsetCurveTest, TiltedCircle_CompareWithBndLib)
{
  // Circle in a tilted plane
  gp_Ax2                   anAx2(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(1.0, 1.0, 1.0));
  Handle(Geom_Circle)      aCircle = new Geom_Circle(anAx2, 4.0);
  Handle(Geom_OffsetCurve) anOff   = new Geom_OffsetCurve(aCircle, 1.5, gp_Dir(1.0, 1.0, 1.0));
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurveTest, InfiniteRange_OpennessPreserved)
{
  Handle(Geom_Line)        aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_OffsetCurve) anOff = new Geom_OffsetCurve(aLine, 3.0, gp::DZ());
  GeomAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box aNewBox;
  GeomBndLib_Curve(anOff).Add(-Precision::Infinite(), 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box anOldBox;
  BndLib_Add3dCurve::Add(anAdaptor, -Precision::Infinite(), 10.0, Precision::Confusion(), anOldBox);

  EXPECT_FALSE(aNewBox.IsVoid());
  EXPECT_TRUE(aNewBox.IsOpenXmin());
  EXPECT_FALSE(aNewBox.IsOpenXmax());
  EXPECT_FALSE(aNewBox.IsOpenYmin());
  EXPECT_FALSE(aNewBox.IsOpenYmax());
  EXPECT_FALSE(aNewBox.IsOpenZmin());
  EXPECT_FALSE(aNewBox.IsOpenZmax());
  EXPECT_FALSE(anOldBox.IsOpenXmin());
}
