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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepFill_PipeShell.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>

namespace
{

//! A spine with a tight corner, so the swept surface is not already C1 in V and the
//! ForceApproxC1 branch of GeomFill_Sweep::BuildAll is reached.
TopoDS_Wire cornerSpine()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.0, 0.0, 4.0);
  aPoles(3) = gp_Pnt(0.0, 0.0, 8.0);
  aPoles(4) = gp_Pnt(4.0, 0.0, 11.0);
  aPoles(5) = gp_Pnt(9.0, 0.0, 13.0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 2;
  aMults(3) = 3;

  BRepBuilderAPI_MakeEdge aEdge(new Geom_BSplineCurve(aPoles, aKnots, aMults, 2));
  return BRepBuilderAPI_MakeWire(aEdge.Edge()).Wire();
}

TopoDS_Wire circleProfile()
{
  const gp_Ax2            anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepBuilderAPI_MakeEdge aEdge(new Geom_Circle(gp_Circ(anAxis, 1.0)));
  return BRepBuilderAPI_MakeWire(aEdge.Edge()).Wire();
}

} // namespace

// ErrorOnSurface() must report the error the approximation achieved, not the one it was asked for.
//
// When ForceApproxC1 is set and the swept surface is not already C1 in V, GeomFill_Sweep::BuildAll
// re-approximates through GeomConvert_ApproxSurface and then overwrote the measured error with the
// requested tolerance. GeomConvert_ApproxSurface::HasResult() is documented as true even for "a
// result that is not NECESSARILY within the required tolerance", so the achieved error is exactly
// the number a caller needs and it was the one being discarded.
//
// For this spine the conversion misses its 1e-4 request by four orders of magnitude, so a reported
// 1e-4 is not a rounding difference from the truth, it is an unrelated number.
TEST(GeomFill_SweepTest, ErrorOnSurfaceReportsTheAchievedConversionError)
{
  BRepFill_PipeShell aPipe(cornerSpine());
  aPipe.Set(true); // Frenet trihedron
  aPipe.SetForceApproxC1(true);
  aPipe.Add(circleProfile());
  aPipe.SetIsBuildHistory(false);

  ASSERT_TRUE(aPipe.Build());

  const double anError = aPipe.ErrorOnSurface();

  // The requested tolerance inside the ForceApproxC1 branch is 1e-4. Reporting exactly that,
  // whatever the geometry, is the defect.
  EXPECT_GT(anError, 1.0e-4) << "ErrorOnSurface() returned " << anError
                             << ", which is the requested tolerance rather than the achieved error";
}

// Without ForceApproxC1 the branch is never entered, and the error reported there was always the
// measured one. Pins that this change did not disturb the ordinary path.
TEST(GeomFill_SweepTest, ErrorOnSurfaceUnaffectedWithoutForcedC1)
{
  BRepFill_PipeShell aPipe(cornerSpine());
  aPipe.Set(true);
  aPipe.SetForceApproxC1(false);
  aPipe.Add(circleProfile());
  aPipe.SetIsBuildHistory(false);

  ASSERT_TRUE(aPipe.Build());

  const double anError = aPipe.ErrorOnSurface();
  EXPECT_GT(anError, 0.0);
  EXPECT_LT(anError, 1.0e-4);
}
