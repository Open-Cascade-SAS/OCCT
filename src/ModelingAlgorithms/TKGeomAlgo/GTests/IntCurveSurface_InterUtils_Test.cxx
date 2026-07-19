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

#include <IntCurveSurface_InterUtils.pxx>

#include <Intf_SectionPoint.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{

class TestPolyhedron
{
public:
  TestPolyhedron()
  {
    myPoints[0] = gp_Pnt(0.0, 0.0, 0.0);
    myPoints[1] = gp_Pnt(2.0, 0.0, 0.0);
    myPoints[2] = gp_Pnt(2.0, 0.0, 0.0);

    myU[0] = 0.0;
    myU[1] = 1.0;
    myU[2] = 2.0;

    myV[0] = 0.0;
    myV[1] = 0.0;
    myV[2] = 0.0;
  }

  void Triangle(const int, int& theP1, int& theP2, int& theP3) const
  {
    theP1 = 1;
    theP2 = 2;
    theP3 = 3;
  }

  const gp_Pnt& Point(const int theIndex) const { return myPoints[theIndex - 1]; }

  void Parameters(const int theIndex, double& theU, double& theV) const
  {
    theU = myU[theIndex - 1];
    theV = myV[theIndex - 1];
  }

private:
  gp_Pnt myPoints[3];
  double myU[3];
  double myV[3];
};

class TestPolygon
{
public:
  double ApproxParamOnCurve(const int theIndex, const double theParamOnLine) const
  {
    return 10.0 * theIndex + theParamOnLine;
  }
};

} // namespace

TEST(IntCurveSurface_InterUtils, SectionPointToParameters_DegenerateFaceFallsBackToLongestEdge)
{
  const TestPolyhedron aPolyhedron;
  const TestPolygon    aPolygon;

  const Intf_SectionPoint
    aSectionPoint(gp_Pnt(0.5, 0.0, 0.0), Intf_EDGE, 0, 2, 0.4, Intf_FACE, 1, 0, 0.0, 0.0);

  double aU = 0.0;
  double aV = 0.0;
  double aW = 0.0;
  IntCurveSurface_InterUtils::SectionPointToParameters(aSectionPoint,
                                                       aPolyhedron,
                                                       aPolygon,
                                                       aU,
                                                       aV,
                                                       aW);

  EXPECT_NEAR(aU, 0.25, 1.0e-12);
  EXPECT_NEAR(aV, 0.0, 1.0e-12);
  EXPECT_NEAR(aW, 20.4, 1.0e-12);
}