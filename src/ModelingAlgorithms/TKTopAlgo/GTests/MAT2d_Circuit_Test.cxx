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

#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <MAT2d_Circuit.hxx>

namespace
{
int circuitSize(const double theLength, const gp_Dir2d& theSecondDirection, const bool theTrigo)
{
  const gp_Pnt2d                                     aJoin(61.0, 2.0);
  NCollection_Sequence<occ::handle<Geom2d_Geometry>> aSegments;
  aSegments.Append(
    new Geom2d_TrimmedCurve(new Geom2d_Line(aJoin, gp_Dir2d(1.0, 0.0)), -2.0 * theLength, 0.0));
  aSegments.Append(
    new Geom2d_TrimmedCurve(new Geom2d_Line(aJoin, theSecondDirection), 0.0, theLength));
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>> aFigure;
  aFigure.Append(aSegments);
  NCollection_Sequence<bool> aClosed;
  aClosed.Append(false);
  MAT2d_Circuit aCircuit(GeomAbs_Arc, true);
  aCircuit.Perform(aFigure, aClosed, 1, theTrigo);
  return aCircuit.NumberOfItems();
}
} // namespace

TEST(MAT2d_CircuitTest, NearlyReversedLinesHaveOppositeCornerSides)
{
  for (const double aLength : {4.0e-6, 1.0, 1000.0})
  {
    for (const double aTurn : {-1.0e-9, 1.0e-9})
    {
      // Two segments and their endpoints, with a vertex generator only on the convex side.
      EXPECT_EQ(circuitSize(aLength, gp_Dir2d(-1.0, aTurn), true), aTurn < 0.0 ? 5 : 4);
      EXPECT_EQ(circuitSize(aLength, gp_Dir2d(-1.0, aTurn), false), aTurn > 0.0 ? 5 : 4);
    }
  }
}

TEST(MAT2d_CircuitTest, NearlyAlignedLinesRemainFlat)
{
  for (const double aTurn : {-1.0e-9, 0.0, 1.0e-9})
  {
    EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(1.0, aTurn), true), 4);
    EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(1.0, aTurn), false), 4);
  }
}

TEST(MAT2d_CircuitTest, ExactlyReversedLinesHaveACuspOnBothSides)
{
  EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(-1.0, 0.0), true), 5);
  EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(-1.0, 0.0), false), 5);
}
