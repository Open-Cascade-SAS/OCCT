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

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomGridEval_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
const double THE_TOLERANCE = 1e-10;

//! Helper function to create uniform parameters.
NCollection_Array1<double> CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  NCollection_Array1<double> aParams(1, theNbPoints);
  const double               aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}

//! Helper function to create a simple B-spline curve.
occ::handle<Geom_BSplineCurve> CreateSimpleBSpline()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
}

//! Helper function to create a translation transformation.
gp_Trsf CreateTranslation(double theDx, double theDy, double theDz)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(theDx, theDy, theDz));
  return aTrsf;
}

//! Helper function to create a rotation transformation around Z axis.
gp_Trsf CreateRotationZ(double theAngleRad)
{
  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), theAngleRad);
  return aTrsf;
}
} // namespace

//==================================================================================================
// Tests for GeomAdaptor_TransformedCurve construction
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, DefaultConstructor)
{
  GeomAdaptor_TransformedCurve aCurve;
  EXPECT_TRUE(aCurve.Is3DCurve());
  EXPECT_FALSE(aCurve.IsCurveOnSurface());
  EXPECT_EQ(aCurve.Trsf().Form(), gp_Identity);
}

TEST(GeomAdaptor_TransformedCurveTest, ConstructWithCurveAndTrsf)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);
  EXPECT_TRUE(aCurve.Is3DCurve());
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Line);
  EXPECT_EQ(aCurve.Trsf().Form(), gp_Translation);
}

TEST(GeomAdaptor_TransformedCurveTest, ConstructWithBounds)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aCurve(aLine, 0.0, 10.0, aTrsf);
  EXPECT_NEAR(aCurve.FirstParameter(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aCurve.LastParameter(), 10.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for evaluation with identity transform
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, Line_IdentityTransform)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(1, 2, 3), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf; // identity

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);

  // With identity transform, results should match direct Geom_Line evaluation
  const double aT        = 5.0;
  gp_Pnt       aPnt      = aCurve.Value(aT);
  gp_Pnt       aExpected = aLine->Value(aT);
  EXPECT_NEAR(aPnt.Distance(aExpected), 0.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, Circle_IdentityTransform)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf; // identity

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);
  EXPECT_EQ(aCurve.GetType(), GeomAbs_Circle);

  NCollection_Array1<double> aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt      = aCurve.Value(aParams.Value(i));
    gp_Pnt aExpected = aCircle->Value(aParams.Value(i));
    EXPECT_NEAR(aPnt.Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for evaluation with non-identity transform
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, Line_Translation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);

  // Line along X at Z=0, translated to Z=5
  gp_Pnt aPnt = aCurve.Value(3.0);
  EXPECT_NEAR(aPnt.X(), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aPnt.Y(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aPnt.Z(), 5.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, Line_Rotation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateRotationZ(M_PI / 2);

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);

  // Line along X rotated 90 degrees -> line along Y
  gp_Pnt aPnt = aCurve.Value(3.0);
  EXPECT_NEAR(aPnt.X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aPnt.Y(), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aPnt.Z(), 0.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, Circle_Translation)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aPnt   = aCurve.Value(aParams.Value(i));
    gp_Pnt aLocal = aCircle->Value(aParams.Value(i));
    gp_Pnt aExpected(aLocal.X() + 10, aLocal.Y() + 20, aLocal.Z() + 30);
    EXPECT_NEAR(aPnt.Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for derivative evaluation with transform
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, D1_Translation)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aCurve.D1(aParams.Value(i), aPnt, aD1);

    // Point should be translated
    gp_Pnt aLocalPnt = aCircle->Value(aParams.Value(i));
    gp_Pnt aExpectedPnt(aLocalPnt.X() + 10, aLocalPnt.Y() + 20, aLocalPnt.Z() + 30);
    EXPECT_NEAR(aPnt.Distance(aExpectedPnt), 0.0, THE_TOLERANCE);

    // D1 vector: translation doesn't change vectors, so should match local D1
    gp_Pnt aLocalP;
    gp_Vec aLocalD1;
    aCircle->D1(aParams.Value(i), aLocalP, aLocalD1);
    EXPECT_NEAR((aD1 - aLocalD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, D1_Rotation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateRotationZ(M_PI / 2);

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);

  gp_Pnt aPnt;
  gp_Vec aD1;
  aCurve.D1(3.0, aPnt, aD1);

  // D1 for line along X is (1,0,0), rotated 90 degrees becomes (0,1,0)
  EXPECT_NEAR(aD1.X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aD1.Y(), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aD1.Z(), 0.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, D2_Translation)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aCurve.D2(aParams.Value(i), aPnt, aD1, aD2);

    gp_Pnt aLocalP;
    gp_Vec aLocalD1, aLocalD2;
    aCircle->D2(aParams.Value(i), aLocalP, aLocalD1, aLocalD2);

    EXPECT_NEAR((aD1 - aLocalD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aD2 - aLocalD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, D3_Rotation)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateRotationZ(M_PI / 4);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aCurve.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);

    // Get local values and transform manually
    gp_Pnt aLocalP;
    gp_Vec aLocalD1, aLocalD2, aLocalD3;
    aCircle->D3(aParams.Value(i), aLocalP, aLocalD1, aLocalD2, aLocalD3);
    aLocalP.Transform(aTrsf);
    aLocalD1.Transform(aTrsf);
    aLocalD2.Transform(aTrsf);
    aLocalD3.Transform(aTrsf);

    EXPECT_NEAR(aPnt.Distance(aLocalP), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aD1 - aLocalD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aD2 - aLocalD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aD3 - aLocalD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, DN_Rotation)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateRotationZ(M_PI / 3);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    gp_Vec aResult   = aCurve.DN(1.0, aOrder);
    gp_Vec aExpected = aCircle->DN(1.0, aOrder);
    aExpected.Transform(aTrsf);
    EXPECT_NEAR((aResult - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for geometry extraction with transform
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, LineExtraction_Transform)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aCurve(aLine, aTrsf);

  gp_Lin aExtracted = aCurve.Line();

  // Line should be translated: origin at (0,0,5), direction unchanged
  EXPECT_NEAR(aExtracted.Location().X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Location().Y(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Location().Z(), 5.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Direction().X(), 1.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, CircleExtraction_Transform)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf                  aTrsf   = CreateTranslation(10, 0, 0);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  gp_Circ aExtracted = aCurve.Circle();

  // Circle center should be translated to (10,0,0)
  EXPECT_NEAR(aExtracted.Location().X(), 10.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Location().Y(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Location().Z(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aExtracted.Radius(), 2.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for ShallowCopy
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, ShallowCopy)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aCurve(aLine, 0.0, 10.0, aTrsf);

  occ::handle<Adaptor3d_Curve> aCopy = aCurve.ShallowCopy();
  ASSERT_FALSE(aCopy.IsNull());

  // Copy should evaluate the same
  gp_Pnt aPnt1 = aCurve.Value(3.0);
  gp_Pnt aPnt2 = aCopy->Value(3.0);
  EXPECT_NEAR(aPnt1.Distance(aPnt2), 0.0, THE_TOLERANCE);
}