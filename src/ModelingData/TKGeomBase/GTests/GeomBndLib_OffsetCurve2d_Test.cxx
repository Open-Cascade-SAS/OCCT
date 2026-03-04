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

#include <GeomBndLib_Curve2d.hxx>

#include <BndLib_Add2dCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{

//! Helper: verify new 2D box is no larger than old box (tighter or equal).
void ExpectNoLarger2d(const Bnd_Box2d& theNew, const Bnd_Box2d& theOld, const double theTol)
{
  const auto [aXminN, aXmaxN, aYminN, aYmaxN] = theNew.Get();
  const auto [aXminO, aXmaxO, aYminO, aYmaxO] = theOld.Get();
  EXPECT_GE(aXminN, aXminO - theTol) << "New Xmin is smaller than old";
  EXPECT_LE(aXmaxN, aXmaxO + theTol) << "New Xmax is larger than old";
  EXPECT_GE(aYminN, aYminO - theTol) << "New Ymin is smaller than old";
  EXPECT_LE(aYmaxN, aYmaxO + theTol) << "New Ymax is larger than old";
}

//! Helper: verify 2D box contains all sampled points of the offset curve.
void ExpectContainsCurve2d(const Bnd_Box2d&                  theBox,
                           const Handle(Geom2d_OffsetCurve)& theCurve,
                           const double                      theU1,
                           const double                      theU2,
                           const int                         theNbSamples)
{
  const double aStep = (theU2 - theU1) / theNbSamples;
  double       aXmin = 0., aYmin = 0., aXmax = 0., aYmax = 0.;
  theBox.Get(aXmin, aYmin, aXmax, aYmax);
  for (int i = 0; i <= theNbSamples; ++i)
  {
    const double   aU   = theU1 + i * aStep;
    const gp_Pnt2d aPnt = theCurve->Value(aU);
    EXPECT_GE(aPnt.X(), aXmin - Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_LE(aPnt.X(), aXmax + Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_GE(aPnt.Y(), aYmin - Precision::Confusion()) << "Point outside box at U=" << aU;
    EXPECT_LE(aPnt.Y(), aYmax + Precision::Confusion()) << "Point outside box at U=" << aU;
  }
}

} // namespace

// =========================================================================
// Circle offset (positive)
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, Circle_PositiveOffset_Full_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, 2.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurve2dTest, Circle_PositiveOffset_Arc_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, 2.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(0.0, M_PI / 2.0, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, 0.0, M_PI / 2.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, M_PI / 2.0, 50);
}

// =========================================================================
// Circle offset (negative)
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, Circle_NegativeOffset_Full_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, -3.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurve2dTest, Circle_NegativeOffset_Arc_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, -3.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(M_PI / 4.0, M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, M_PI / 4.0, M_PI, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, M_PI / 4.0, M_PI, 50);
}

// =========================================================================
// Ellipse offset
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, Ellipse_Full_CompareWithBndLib)
{
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                       8.0,
                       3.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(anEllipse, 1.5);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

TEST(GeomBndLib_OffsetCurve2dTest, Ellipse_Arc_OptimalCompareWithBndLib)
{
  Handle(Geom2d_Ellipse) anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)),
                       8.0,
                       3.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(anEllipse, 1.5);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).AddOptimal(0.0, M_PI, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::AddOptimal(anOff, 0.0, M_PI, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, M_PI, 50);
}

// =========================================================================
// Line offset
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, Line_CompareWithBndLib)
{
  Handle(Geom2d_Line)        aLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aLine, 3.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(0.0, 10.0, Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, 0.0, 10.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 10.0, 50);
}

// =========================================================================
// BSpline offset
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, BSpline_OptimalCompareWithBndLib)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(3.0, 5.0));
  aPoles.SetValue(3, gp_Pnt2d(7.0, -2.0));
  aPoles.SetValue(4, gp_Pnt2d(10.0, 0.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 1);
  aMults.SetValue(3, 3);

  Handle(Geom2d_BSplineCurve) aBSpl = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
  Handle(Geom2d_OffsetCurve)  anOff = new Geom2d_OffsetCurve(aBSpl, 1.0);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).AddOptimal(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::AddOptimal(anOff, 0.0, 1.0, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 1.0, 50);
}

// =========================================================================
// Large offset
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, LargeOffset_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, 20.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}

// =========================================================================
// Off-center circle with offset
// =========================================================================

TEST(GeomBndLib_OffsetCurve2dTest, OffCenterCircle_CompareWithBndLib)
{
  Handle(Geom2d_Circle) aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(5.0, 3.0), gp_Dir2d(1.0, 0.0)), 4.0);
  Handle(Geom2d_OffsetCurve) anOff = new Geom2d_OffsetCurve(aCircle, 1.0);
  Geom2dAdaptor_Curve        anAdaptor(anOff);

  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(anOff).Add(Precision::Confusion(), aNewBox);

  Bnd_Box2d anOldBox;
  BndLib_Add2dCurve::Add(anAdaptor, Precision::Confusion(), anOldBox);

  ExpectNoLarger2d(aNewBox, anOldBox, Precision::Confusion());
  ExpectContainsCurve2d(aNewBox, anOff, 0.0, 2.0 * M_PI, 100);
}
