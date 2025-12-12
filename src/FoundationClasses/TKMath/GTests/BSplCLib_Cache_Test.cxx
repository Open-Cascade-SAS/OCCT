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

#include <BSplCLib.hxx>
#include <BSplCLib_Cache.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_TOLERANCE = 1e-10;
}

//==================================================================================================
// Test fixture for BSplCLib_Cache tests
//==================================================================================================

class BSplCLib_CacheTest : public ::testing::Test
{
protected:
  void SetUp() override {}

  //! Creates flat knots array from knots and multiplicities
  void createFlatKnots(const TColStd_Array1OfReal&    theKnots,
                       const TColStd_Array1OfInteger& theMults,
                       TColStd_Array1OfReal&          theFlatKnots) const
  {
    int aFlatIndex = theFlatKnots.Lower();
    for (int i = theKnots.Lower(); i <= theKnots.Upper(); ++i)
    {
      for (int j = 0; j < theMults(i); ++j)
      {
        theFlatKnots(aFlatIndex++) = theKnots(i);
      }
    }
  }
};

//==================================================================================================
// Non-rational 3D curve tests
//==================================================================================================

TEST_F(BSplCLib_CacheTest, D0_NonRationalCurve3D)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 2, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  TColStd_Array1OfReal aFlatKnots(1, 8);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 3;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, nullptr);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, nullptr);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, nullptr);
    }
    aCache->D0(u, aCachePnt);

    // Direct evaluation using BSplCLib::D0
    BSplCLib::D0(u, 0, aDegree, false, aPoles, nullptr, aKnots, &aMults, aDirectPnt);

    // Compare
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE) << "D0 X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE) << "D0 Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE) << "D0 Z mismatch at u=" << u;
  }
}

TEST_F(BSplCLib_CacheTest, D1_NonRationalCurve3D)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 1);
  aPoles(3) = gp_Pnt(2, 2, 1);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  TColStd_Array1OfReal aFlatKnots(1, 8);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 3;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, nullptr);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, nullptr);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;
    gp_Vec aCacheTan, aDirectTan;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, nullptr);
    }
    aCache->D1(u, aCachePnt, aCacheTan);

    // Direct evaluation using BSplCLib::D1
    BSplCLib::D1(u, 0, aDegree, false, aPoles, nullptr, aKnots, &aMults, aDirectPnt, aDirectTan);

    // Compare point
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE) << "D1 point X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE) << "D1 point Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE) << "D1 point Z mismatch at u=" << u;

    // Compare tangent
    EXPECT_NEAR(aCacheTan.X(), aDirectTan.X(), THE_TOLERANCE) << "D1 tangent X mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Y(), aDirectTan.Y(), THE_TOLERANCE) << "D1 tangent Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Z(), aDirectTan.Z(), THE_TOLERANCE) << "D1 tangent Z mismatch at u=" << u;
  }
}

TEST_F(BSplCLib_CacheTest, D2_NonRationalCurve3D)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 1);
  aPoles(3) = gp_Pnt(2, 2, 1);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  TColStd_Array1OfReal aFlatKnots(1, 8);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 3;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, nullptr);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, nullptr);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;
    gp_Vec aCacheTan, aDirectTan;
    gp_Vec aCacheCurv, aDirectCurv;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, nullptr);
    }
    aCache->D2(u, aCachePnt, aCacheTan, aCacheCurv);

    // Direct evaluation using BSplCLib::D2
    BSplCLib::D2(u,
                 0,
                 aDegree,
                 false,
                 aPoles,
                 nullptr,
                 aKnots,
                 &aMults,
                 aDirectPnt,
                 aDirectTan,
                 aDirectCurv);

    // Compare point
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE) << "D2 point X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE) << "D2 point Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE) << "D2 point Z mismatch at u=" << u;

    // Compare tangent
    EXPECT_NEAR(aCacheTan.X(), aDirectTan.X(), THE_TOLERANCE) << "D2 tangent X mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Y(), aDirectTan.Y(), THE_TOLERANCE) << "D2 tangent Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Z(), aDirectTan.Z(), THE_TOLERANCE) << "D2 tangent Z mismatch at u=" << u;

    // Compare curvature
    EXPECT_NEAR(aCacheCurv.X(), aDirectCurv.X(), THE_TOLERANCE)
      << "D2 curvature X mismatch at u=" << u;
    EXPECT_NEAR(aCacheCurv.Y(), aDirectCurv.Y(), THE_TOLERANCE)
      << "D2 curvature Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheCurv.Z(), aDirectCurv.Z(), THE_TOLERANCE)
      << "D2 curvature Z mismatch at u=" << u;
  }
}

//==================================================================================================
// Rational 3D curve tests
//==================================================================================================

TEST_F(BSplCLib_CacheTest, D0_RationalCurve3D)
{
  // Create a rational quadratic Bezier curve (can represent a circle arc)
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 0.707106781186548; // sqrt(2)/2
  aWeights(3) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  TColStd_Array1OfReal aFlatKnots(1, 6);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 2;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, &aWeights);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, &aWeights);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, &aWeights);
    }
    aCache->D0(u, aCachePnt);

    // Direct evaluation using BSplCLib::D0
    BSplCLib::D0(u, 0, aDegree, false, aPoles, &aWeights, aKnots, &aMults, aDirectPnt);

    // Compare
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE)
      << "Rational D0 X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE)
      << "Rational D0 Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE)
      << "Rational D0 Z mismatch at u=" << u;
  }
}

TEST_F(BSplCLib_CacheTest, D1_RationalCurve3D)
{
  // Create a rational quadratic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 0.707106781186548; // sqrt(2)/2
  aWeights(3) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  TColStd_Array1OfReal aFlatKnots(1, 6);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 2;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, &aWeights);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, &aWeights);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;
    gp_Vec aCacheTan, aDirectTan;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, &aWeights);
    }
    aCache->D1(u, aCachePnt, aCacheTan);

    // Direct evaluation using BSplCLib::D1
    BSplCLib::D1(u, 0, aDegree, false, aPoles, &aWeights, aKnots, &aMults, aDirectPnt, aDirectTan);

    // Compare point
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE)
      << "Rational D1 point X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE)
      << "Rational D1 point Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE)
      << "Rational D1 point Z mismatch at u=" << u;

    // Compare tangent
    EXPECT_NEAR(aCacheTan.X(), aDirectTan.X(), THE_TOLERANCE)
      << "Rational D1 tangent X mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Y(), aDirectTan.Y(), THE_TOLERANCE)
      << "Rational D1 tangent Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Z(), aDirectTan.Z(), THE_TOLERANCE)
      << "Rational D1 tangent Z mismatch at u=" << u;
  }
}

TEST_F(BSplCLib_CacheTest, D2_RationalCurve3D)
{
  // Create a rational quadratic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(1, 0, 0);
  aPoles(2) = gp_Pnt(1, 1, 0);
  aPoles(3) = gp_Pnt(0, 1, 0);

  TColStd_Array1OfReal aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 0.707106781186548; // sqrt(2)/2
  aWeights(3) = 1.0;

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  TColStd_Array1OfReal aFlatKnots(1, 6);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 2;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, &aWeights);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, &aWeights);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;
    gp_Vec aCacheTan, aDirectTan;
    gp_Vec aCacheCurv, aDirectCurv;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, &aWeights);
    }
    aCache->D2(u, aCachePnt, aCacheTan, aCacheCurv);

    // Direct evaluation using BSplCLib::D2
    BSplCLib::D2(u,
                 0,
                 aDegree,
                 false,
                 aPoles,
                 &aWeights,
                 aKnots,
                 &aMults,
                 aDirectPnt,
                 aDirectTan,
                 aDirectCurv);

    // Compare point
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE)
      << "Rational D2 point X mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Y(), aDirectPnt.Y(), THE_TOLERANCE)
      << "Rational D2 point Y mismatch at u=" << u;
    EXPECT_NEAR(aCachePnt.Z(), aDirectPnt.Z(), THE_TOLERANCE)
      << "Rational D2 point Z mismatch at u=" << u;

    // Compare tangent
    EXPECT_NEAR(aCacheTan.X(), aDirectTan.X(), THE_TOLERANCE)
      << "Rational D2 tangent X mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Y(), aDirectTan.Y(), THE_TOLERANCE)
      << "Rational D2 tangent Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheTan.Z(), aDirectTan.Z(), THE_TOLERANCE)
      << "Rational D2 tangent Z mismatch at u=" << u;

    // Compare curvature
    EXPECT_NEAR(aCacheCurv.X(), aDirectCurv.X(), THE_TOLERANCE)
      << "Rational D2 curvature X mismatch at u=" << u;
    EXPECT_NEAR(aCacheCurv.Y(), aDirectCurv.Y(), THE_TOLERANCE)
      << "Rational D2 curvature Y mismatch at u=" << u;
    EXPECT_NEAR(aCacheCurv.Z(), aDirectCurv.Z(), THE_TOLERANCE)
      << "Rational D2 curvature Z mismatch at u=" << u;
  }
}

//==================================================================================================
// Test D3 for completeness
//==================================================================================================

TEST_F(BSplCLib_CacheTest, D3_NonRationalCurve3D)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 1);
  aPoles(3) = gp_Pnt(2, 2, 1);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  TColStd_Array1OfReal aFlatKnots(1, 8);
  createFlatKnots(aKnots, aMults, aFlatKnots);

  const int aDegree = 3;

  // Create cache
  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, false, aFlatKnots, aPoles, nullptr);
  aCache->BuildCache(0.5, aFlatKnots, aPoles, nullptr);

  // Test at several parameter values
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aCachePnt, aDirectPnt;
    gp_Vec aCacheTan, aDirectTan;
    gp_Vec aCacheCurv, aDirectCurv;
    gp_Vec aCacheTors, aDirectTors;

    // Cached evaluation
    if (!aCache->IsCacheValid(u))
    {
      aCache->BuildCache(u, aFlatKnots, aPoles, nullptr);
    }
    aCache->D3(u, aCachePnt, aCacheTan, aCacheCurv, aCacheTors);

    // Direct evaluation using BSplCLib::D3
    BSplCLib::D3(u,
                 0,
                 aDegree,
                 false,
                 aPoles,
                 nullptr,
                 aKnots,
                 &aMults,
                 aDirectPnt,
                 aDirectTan,
                 aDirectCurv,
                 aDirectTors);

    // Compare point
    EXPECT_NEAR(aCachePnt.X(), aDirectPnt.X(), THE_TOLERANCE) << "D3 point X mismatch at u=" << u;

    // Compare tangent
    EXPECT_NEAR(aCacheTan.X(), aDirectTan.X(), THE_TOLERANCE) << "D3 tangent X mismatch at u=" << u;

    // Compare curvature
    EXPECT_NEAR(aCacheCurv.X(), aDirectCurv.X(), THE_TOLERANCE)
      << "D3 curvature X mismatch at u=" << u;

    // Compare torsion
    EXPECT_NEAR(aCacheTors.X(), aDirectTors.X(), THE_TOLERANCE)
      << "D3 torsion X mismatch at u=" << u;
  }
}
