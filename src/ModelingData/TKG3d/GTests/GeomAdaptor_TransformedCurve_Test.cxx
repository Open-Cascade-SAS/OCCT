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

#include <gtest/gtest.h>

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
  const double aT = 5.0;
  gp_Pnt       aPnt = aCurve.Value(aT);
  gp_Pnt       aExpected = aLine->Value(aT);
  EXPECT_NEAR(aPnt.Distance(aExpected), 0.0, THE_TOLERANCE);
}

TEST(GeomAdaptor_TransformedCurveTest, Circle_IdentityTransform)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf; // identity

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
    gp_Pnt aPnt = aCurve.Value(aParams.Value(i));
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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aPnt = aCurve.Value(aParams.Value(i));
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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 20, 30);

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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 20, 30);

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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 4);

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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 3);

  GeomAdaptor_TransformedCurve aCurve(aCircle, aTrsf);

  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    gp_Vec aResult = aCurve.DN(1.0, aOrder);
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
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 0, 0);

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

//==================================================================================================
// Tests for GeomGridEval_Curve integration with TransformedCurve
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, EvalGrid_3DCurve_Identity)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf; // identity

  GeomAdaptor_TransformedCurve aAdaptor(aLine, 0.0, 10.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);
  EXPECT_FALSE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aLine->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGrid_3DCurve_Translation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aAdaptor(aLine, 0.0, 10.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    // Evaluate via TransformedCurve for reference
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGrid_Circle_Rotation)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 4);

  GeomAdaptor_TransformedCurve aAdaptor(aCircle, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGrid_BSpline_Translation)
{
  occ::handle<Geom_BSplineCurve> aBSpline = CreateSimpleBSpline();
  gp_Trsf                        aTrsf    = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aAdaptor(aBSpline, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineCurve);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for derivative grid evaluation with transform
//==================================================================================================

TEST(GeomAdaptor_TransformedCurveTest, EvalGridD1_Circle_Translation)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 20, 30);

  GeomAdaptor_TransformedCurve aAdaptor(aCircle, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aAdaptor.D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGridD2_BSpline_Rotation)
{
  occ::handle<Geom_BSplineCurve> aBSpline = CreateSimpleBSpline();
  gp_Trsf                        aTrsf    = CreateRotationZ(M_PI / 3);

  GeomAdaptor_TransformedCurve aAdaptor(aBSpline, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aAdaptor.D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGridD3_Circle_Rotation)
{
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 6);

  GeomAdaptor_TransformedCurve aAdaptor(aCircle, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomAdaptor_TransformedCurveTest, EvalGridDN_Line_Translation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aAdaptor(aLine, 0.0, 10.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 6);

  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 6; ++i)
    {
      gp_Vec aExpected = aAdaptor.DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Tests for GeomGridEval_Curve with all curve types via TransformedCurve
//==================================================================================================

TEST(GeomGridEval_CurveTest, TransformedEllipse_RotationTranslation)
{
  occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 3.0);

  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 6);
  gp_Trsf aTrans = CreateTranslation(10, -5, 7);
  aTrsf.Multiply(aTrans);

  GeomAdaptor_TransformedCurve aAdaptor(anEllipse, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Ellipse);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 25);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 25; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedHyperbola_Translation)
{
  occ::handle<Geom_Hyperbola> aHypr =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  gp_Trsf aTrsf = CreateTranslation(5, 10, 15);

  GeomAdaptor_TransformedCurve aAdaptor(aHypr, -2.0, 2.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Hyperbola);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedParabola_Rotation)
{
  occ::handle<Geom_Parabola> aParab =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.5);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 4);

  GeomAdaptor_TransformedCurve aAdaptor(aParab, -3.0, 3.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Parabola);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(-3.0, 3.0, 13);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedBezier_RotationTranslation)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);

  gp_Trsf aTrsf = CreateRotationZ(M_PI / 3);
  gp_Trsf aTrans = CreateTranslation(0, 0, 10);
  aTrsf.Multiply(aTrans);

  GeomAdaptor_TransformedCurve aAdaptor(aBezier, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BezierCurve);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedOffsetCurve_Translation)
{
  occ::handle<Geom_Circle>      aCircle  = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aCircle, 0.5, gp::DZ());
  gp_Trsf                       aTrsf    = CreateTranslation(5, 5, 5);

  GeomAdaptor_TransformedCurve aAdaptor(anOffset, 0.0, 2 * M_PI, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_OffsetCurve);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedBSpline_CompoundTransform)
{
  // Test with a compound rotation + translation + scale transform
  occ::handle<Geom_BSplineCurve> aBSpline = CreateSimpleBSpline();

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(1, 1, 0), gp_Dir(0, 1, 0)), M_PI / 5);
  gp_Trsf aTranslation = CreateTranslation(-3, 7, 2);
  gp_Trsf aTrsf        = aRotation;
  aTrsf.Multiply(aTranslation);

  GeomAdaptor_TransformedCurve aAdaptor(aBSpline, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineCurve);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 31);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 31; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Derivative tests for all curve types with TransformedCurve
//==================================================================================================

TEST(GeomGridEval_CurveTest, TransformedEllipse_D1)
{
  occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 3.0);
  gp_Trsf aTrsf = CreateTranslation(10, -5, 7);

  GeomAdaptor_TransformedCurve aAdaptor(anEllipse, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aAdaptor.D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedHyperbola_D2)
{
  occ::handle<Geom_Hyperbola> aHypr =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 4);

  GeomAdaptor_TransformedCurve aAdaptor(aHypr, -2.0, 2.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aAdaptor.D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedParabola_D3)
{
  occ::handle<Geom_Parabola> aParab =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.5);
  gp_Trsf aTrsf = CreateRotationZ(M_PI / 3);
  gp_Trsf aTrans = CreateTranslation(1, 2, 3);
  aTrsf.Multiply(aTrans);

  GeomAdaptor_TransformedCurve aAdaptor(aParab, -3.0, 3.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(-3.0, 3.0, 13);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedBezier_DN)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);

  gp_Trsf aTrsf = CreateRotationZ(M_PI / 6);

  GeomAdaptor_TransformedCurve aAdaptor(aBezier, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 11; ++i)
    {
      gp_Vec aExpected = aAdaptor.DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_CurveTest, TransformedBSpline_DN_CompoundTransform)
{
  occ::handle<Geom_BSplineCurve> aBSpline = CreateSimpleBSpline();

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1)), M_PI / 7);
  gp_Trsf aTranslation = CreateTranslation(2, -3, 5);
  gp_Trsf aTrsf        = aRotation;
  aTrsf.Multiply(aTranslation);

  GeomAdaptor_TransformedCurve aAdaptor(aBSpline, 0.0, 1.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 21; ++i)
    {
      gp_Vec aExpected = aAdaptor.DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Tests for re-initialization and edge cases
//==================================================================================================

TEST(GeomGridEval_CurveTest, TransformedCurve_Reinitialize)
{
  // Initialize with a line, then reinitialize with a circle
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf1 = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aAdaptor1(aLine, 0.0, 10.0, aTrsf1);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor1);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);
  EXPECT_TRUE(anEval.HasTransformation());

  // Re-initialize with a transformed circle
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf2 = CreateRotationZ(M_PI / 4);

  GeomAdaptor_TransformedCurve aAdaptor2(aCircle, aTrsf2);

  anEval.Initialize(aAdaptor2);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aExpected = aAdaptor2.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedCurve_ReinitializeToIdentity)
{
  // Initialize with transform, then reinitialize with identity
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Trsf                aTrsf = CreateTranslation(0, 0, 5);

  GeomAdaptor_TransformedCurve aAdaptor1(aLine, 0.0, 10.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor1);
  EXPECT_TRUE(anEval.HasTransformation());

  // Re-initialize with identity transform
  gp_Trsf                      aIdentity;
  GeomAdaptor_TransformedCurve aAdaptor2(aLine, 0.0, 10.0, aIdentity);

  anEval.Initialize(aAdaptor2);
  EXPECT_FALSE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aLine->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedCurve_InitWithGeomHandle)
{
  // After initializing via TransformedCurve, re-initialize with Geom handle
  // to verify that myTrsf is properly cleared
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Trsf aTrsf = CreateTranslation(10, 10, 10);

  GeomAdaptor_TransformedCurve aAdaptor(aCircle, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.HasTransformation());

  // Re-initialize directly with Geom handle (no transform)
  anEval.Initialize(aCircle);
  EXPECT_FALSE(anEval.HasTransformation());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aExpected = aCircle->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedTrimmedCurve)
{
  // TrimmedCurve wrapping a circle, passed through TransformedCurve
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  occ::handle<Geom_TrimmedCurve> aTrimmed = new Geom_TrimmedCurve(aCircle, 0.0, M_PI);
  gp_Trsf                        aTrsf    = CreateTranslation(1, 2, 3);

  GeomAdaptor_TransformedCurve aAdaptor(aTrimmed, 0.0, M_PI, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  // TrimmedCurve wrapping Circle should be detected as Circle
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, M_PI, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedCircle_AllDerivatives)
{
  // Comprehensive test: all derivative levels with a single compound transform
  occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 4.0);

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), M_PI / 5);
  gp_Trsf aTranslation = CreateTranslation(-7, 3, 11);
  gp_Trsf aTrsf        = aRotation;
  aTrsf.Multiply(aTranslation);

  GeomAdaptor_TransformedCurve aAdaptor(aCircle, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  // D0
  {
    NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
    for (int i = 1; i <= 17; ++i)
    {
      gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
      EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
    }
  }
  // D1
  {
    NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);
    for (int i = 1; i <= 17; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1;
      aAdaptor.D1(aParams.Value(i), aPnt, aD1);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // D2
  {
    NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);
    for (int i = 1; i <= 17; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1, aD2;
      aAdaptor.D2(aParams.Value(i), aPnt, aD1, aD2);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // D3
  {
    NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);
    for (int i = 1; i <= 17; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1, aD2, aD3;
      aAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // DN orders 1-3
  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);
    for (int i = 1; i <= 17; ++i)
    {
      gp_Vec aExpected = aAdaptor.DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_CurveTest, TransformedLine_NonOriginAxis)
{
  // Line not at origin, with a rotation around a non-origin axis
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(5, 3, 0), gp_Dir(0, 1, 0));

  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), M_PI / 2);

  GeomAdaptor_TransformedCurve aAdaptor(aLine, -5.0, 5.0, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);
  EXPECT_TRUE(anEval.HasTransformation());

  NCollection_Array1<double> aParams = CreateUniformParams(-5.0, 5.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aAdaptor.Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, TransformedEllipse_AllDerivatives)
{
  occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 3.0);

  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), M_PI / 4);
  gp_Trsf aTrans = CreateTranslation(2, -1, 4);
  aTrsf.Multiply(aTrans);

  GeomAdaptor_TransformedCurve aAdaptor(anEllipse, aTrsf);

  GeomGridEval_Curve anEval;
  anEval.Initialize(aAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  // D1
  {
    NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);
    for (int i = 1; i <= 13; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1;
      aAdaptor.D1(aParams.Value(i), aPnt, aD1);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // D2
  {
    NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);
    for (int i = 1; i <= 13; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1, aD2;
      aAdaptor.D2(aParams.Value(i), aPnt, aD1, aD2);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // D3
  {
    NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);
    for (int i = 1; i <= 13; ++i)
    {
      gp_Pnt aPnt;
      gp_Vec aD1, aD2, aD3;
      aAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
      EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
  // DN
  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);
    for (int i = 1; i <= 13; ++i)
    {
      gp_Vec aExpected = aAdaptor.DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}
