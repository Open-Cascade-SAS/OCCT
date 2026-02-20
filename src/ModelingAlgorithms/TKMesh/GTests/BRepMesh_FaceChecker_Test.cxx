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

#include <BRepMesh_GeomTool.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Vector.hxx>

#include <cmath>
#include <gtest/gtest.h>

namespace
{
constexpr double THE_MAX_TANGENT_ANGLE = 5. * M_PI / 180.;

bool acceptSegmentIntersection(const gp_Pnt2d&                     theRefPnt1,
                               const gp_Pnt2d&                     theRefPnt2,
                               const gp_Pnt2d&                     theCandidatePnt1,
                               const gp_Pnt2d&                     theCandidatePnt2,
                               const NCollection_Vector<gp_Pnt2d>& theLoopPoint2,
                               const double                        theTolerance,
                               const bool                          theUseSelfLoopCheck)
{
  gp_Pnt2d                         anIntPnt;
  const BRepMesh_GeomTool::IntFlag anIntStatus =
    BRepMesh_GeomTool::IntSegSeg(theRefPnt1.XY(),
                                 theRefPnt2.XY(),
                                 theCandidatePnt1.XY(),
                                 theCandidatePnt2.XY(),
                                 false,
                                 false,
                                 anIntPnt);
  if (anIntStatus != BRepMesh_GeomTool::Cross)
  {
    return false;
  }

  const gp_Vec2d aRefDir(theRefPnt1.XY(), theRefPnt2.XY());
  const double   anAngle = aRefDir.Angle(gp_Vec2d(theCandidatePnt1.XY(), theCandidatePnt2.XY()));
  if (std::abs(anAngle) < THE_MAX_TANGENT_ANGLE)
  {
    return false;
  }

  if (!theUseSelfLoopCheck)
  {
    return true;
  }

  const double aMaxLoopSize = M_PI * theTolerance * theTolerance;
  gp_XY        aPrevVec;
  double       aSumS   = 0.;
  const gp_XY& aRefPnt = anIntPnt.Coord();
  for (int i = 0; i < theLoopPoint2.Size(); ++i)
  {
    const gp_XY aCurVec = theLoopPoint2(i).XY() - aRefPnt;
    if (aCurVec.SquareModulus() < gp::Resolution())
    {
      continue;
    }
    if (aPrevVec.SquareModulus() > gp::Resolution())
    {
      aSumS += aPrevVec ^ aCurVec;
    }
    aPrevVec = aCurVec;
  }

  return std::abs(aSumS / 2.) >= aMaxLoopSize;
}
} // namespace

TEST(BRepMesh_FaceCheckerTest, NearlyTangentCrossingRejected)
{
  const bool isAccepted = acceptSegmentIntersection(gp_Pnt2d(-10.0, 0.0),
                                                    gp_Pnt2d(10.0, 0.0),
                                                    gp_Pnt2d(-10.0, -0.1),
                                                    gp_Pnt2d(10.0, 0.1),
                                                    NCollection_Vector<gp_Pnt2d>(),
                                                    0.1,
                                                    false);
  EXPECT_FALSE(isAccepted);
}

TEST(BRepMesh_FaceCheckerTest, ProperCrossingAccepted)
{
  const bool isAccepted = acceptSegmentIntersection(gp_Pnt2d(-10.0, 0.0),
                                                    gp_Pnt2d(10.0, 0.0),
                                                    gp_Pnt2d(-10.0, -10.0),
                                                    gp_Pnt2d(10.0, 10.0),
                                                    NCollection_Vector<gp_Pnt2d>(),
                                                    0.1,
                                                    false);
  EXPECT_TRUE(isAccepted);
}

TEST(BRepMesh_FaceCheckerTest, DegenerateCandidateRejectedWithoutCrash)
{
  const bool isAccepted = acceptSegmentIntersection(gp_Pnt2d(-10.0, 0.0),
                                                    gp_Pnt2d(10.0, 0.0),
                                                    gp_Pnt2d(0.0, 0.0),
                                                    gp_Pnt2d(0.0, 0.0),
                                                    NCollection_Vector<gp_Pnt2d>(),
                                                    0.1,
                                                    false);
  EXPECT_FALSE(isAccepted);
}

TEST(BRepMesh_FaceCheckerTest, SmallSelfLoopRejected)
{
  NCollection_Vector<gp_Pnt2d> aLoopPoint2;
  aLoopPoint2.Append(gp_Pnt2d(1.0e-4, 0.0));
  aLoopPoint2.Append(gp_Pnt2d(0.0, 1.0e-4));
  aLoopPoint2.Append(gp_Pnt2d(-1.0e-4, 0.0));

  const bool isAccepted = acceptSegmentIntersection(gp_Pnt2d(-10.0, 0.0),
                                                    gp_Pnt2d(10.0, 0.0),
                                                    gp_Pnt2d(0.0, -10.0),
                                                    gp_Pnt2d(0.0, 10.0),
                                                    aLoopPoint2,
                                                    0.1,
                                                    true);
  EXPECT_FALSE(isAccepted);
}
