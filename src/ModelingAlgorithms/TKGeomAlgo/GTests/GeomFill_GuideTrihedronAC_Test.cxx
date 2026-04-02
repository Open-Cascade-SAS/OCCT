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

#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomFill_GuideTrihedronAC.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

// Helper: create a cubic BSpline curve suitable for C2 evaluation.
static occ::handle<GeomAdaptor_Curve> makeBSplineCurve(const gp_Pnt& theP1,
                                                       const gp_Pnt& theP2,
                                                       const gp_Pnt& theP3,
                                                       const gp_Pnt& theP4)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = theP1;
  aPoles(2) = theP2;
  aPoles(3) = theP3;
  aPoles(4) = theP4;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  return new GeomAdaptor_Curve(aCurve);
}

TEST(GeomFill_GuideTrihedronAC, D0_ReturnsTrue)
{
  occ::handle<Adaptor3d_Curve> aGuide =
    makeBSplineCurve(gp_Pnt(0, 0, 1), gp_Pnt(1, 0, 1), gp_Pnt(1, 1, 1), gp_Pnt(0, 1, 1));
  occ::handle<Adaptor3d_Curve> aPath =
    makeBSplineCurve(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0));

  GeomFill_GuideTrihedronAC aTrihedron(aGuide);
  aTrihedron.SetCurve(aPath);

  gp_Vec aTangent, aNormal, aBiNormal;
  const bool isOk = aTrihedron.D0(0.5, aTangent, aNormal, aBiNormal);
  EXPECT_TRUE(isOk);
  EXPECT_GT(aTangent.Magnitude(), Precision::Confusion());
  EXPECT_GT(aNormal.Magnitude(), Precision::Confusion());
  EXPECT_GT(aBiNormal.Magnitude(), Precision::Confusion());
}

TEST(GeomFill_GuideTrihedronAC, D1_ReturnsTrue)
{
  occ::handle<Adaptor3d_Curve> aGuide =
    makeBSplineCurve(gp_Pnt(0, 0, 1), gp_Pnt(1, 0, 1), gp_Pnt(1, 1, 1), gp_Pnt(0, 1, 1));
  occ::handle<Adaptor3d_Curve> aPath =
    makeBSplineCurve(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0));

  GeomFill_GuideTrihedronAC aTrihedron(aGuide);
  aTrihedron.SetCurve(aPath);

  gp_Vec aTangent, aDTangent, aNormal, aDNormal, aBiNormal, aDBiNormal;
  const bool isOk =
    aTrihedron.D1(0.5, aTangent, aDTangent, aNormal, aDNormal, aBiNormal, aDBiNormal);
  EXPECT_TRUE(isOk);
  EXPECT_GT(aTangent.Magnitude(), Precision::Confusion());
  EXPECT_GT(aNormal.Magnitude(), Precision::Confusion());
  EXPECT_GT(aBiNormal.Magnitude(), Precision::Confusion());
}

// Regression test for bug #7: D2 was returning false despite completing computation.
TEST(GeomFill_GuideTrihedronAC, D2_ReturnsTrue)
{
  occ::handle<Adaptor3d_Curve> aGuide =
    makeBSplineCurve(gp_Pnt(0, 0, 1), gp_Pnt(1, 0, 1), gp_Pnt(1, 1, 1), gp_Pnt(0, 1, 1));
  occ::handle<Adaptor3d_Curve> aPath =
    makeBSplineCurve(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0));

  GeomFill_GuideTrihedronAC aTrihedron(aGuide);
  aTrihedron.SetCurve(aPath);

  gp_Vec aTangent, aDTangent, aD2Tangent;
  gp_Vec aNormal, aDNormal, aD2Normal;
  gp_Vec aBiNormal, aDBiNormal, aD2BiNormal;

  const bool isOk = aTrihedron.D2(0.5,
                                  aTangent,
                                  aDTangent,
                                  aD2Tangent,
                                  aNormal,
                                  aDNormal,
                                  aD2Normal,
                                  aBiNormal,
                                  aDBiNormal,
                                  aD2BiNormal);
  EXPECT_TRUE(isOk);

  // Verify frame vectors are non-degenerate.
  EXPECT_GT(aTangent.Magnitude(), Precision::Confusion());
  EXPECT_GT(aNormal.Magnitude(), Precision::Confusion());
  EXPECT_GT(aBiNormal.Magnitude(), Precision::Confusion());
}

// Verify D2 produces consistent results at multiple parameters.
TEST(GeomFill_GuideTrihedronAC, D2_ConsistentAtMultipleParams)
{
  occ::handle<Adaptor3d_Curve> aGuide =
    makeBSplineCurve(gp_Pnt(0, 0, 2), gp_Pnt(2, 0, 2), gp_Pnt(2, 2, 2), gp_Pnt(0, 2, 2));
  occ::handle<Adaptor3d_Curve> aPath =
    makeBSplineCurve(gp_Pnt(0, 0, 0), gp_Pnt(2, 0, 0), gp_Pnt(2, 2, 0), gp_Pnt(0, 2, 0));

  GeomFill_GuideTrihedronAC aTrihedron(aGuide);
  aTrihedron.SetCurve(aPath);

  const double aParams[] = {0.2, 0.4, 0.6, 0.8};
  for (const double aParam : aParams)
  {
    gp_Vec aTangent, aDTangent, aD2Tangent;
    gp_Vec aNormal, aDNormal, aD2Normal;
    gp_Vec aBiNormal, aDBiNormal, aD2BiNormal;

    const bool isOk = aTrihedron.D2(aParam,
                                    aTangent,
                                    aDTangent,
                                    aD2Tangent,
                                    aNormal,
                                    aDNormal,
                                    aD2Normal,
                                    aBiNormal,
                                    aDBiNormal,
                                    aD2BiNormal);
    EXPECT_TRUE(isOk) << "D2 failed at param=" << aParam;
  }
}
