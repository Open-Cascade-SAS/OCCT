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

#include <GC_MakeCircle2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Precision.hxx>
#include <Standard_Handle.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(Geom2dConvert_CompCurveToBSplineCurveTest, OCC30747_ClosedContourFromCircleArcs)
{
  // OCC30747: 2d Curves concatenator must properly handle closed contours.
  // Split a full circle into 10 arcs and assemble them into a closed BSpline.
  const occ::handle<Geom2d_Circle> aCirc = GC_MakeCircle2d(gp_Pnt2d(0, 0), 50);
  ASSERT_FALSE(aCirc.IsNull());

  const double aF     = aCirc->FirstParameter();
  const double aL     = aCirc->LastParameter();
  const int    aNb    = 10;
  const double aDelta = (aF + aL) / aNb;

  occ::handle<Geom2d_TrimmedCurve> aFTrim = new Geom2d_TrimmedCurve(aCirc, aF, aDelta);
  Geom2dConvert_CompCurveToBSplineCurve aRes(aFTrim);

  for (int anId = 1; anId < aNb; anId++)
  {
    occ::handle<Geom2d_TrimmedCurve> aLTrim;
    if (anId == (aNb - 1))
    {
      aLTrim = new Geom2d_TrimmedCurve(aCirc, anId * aDelta, aF);
    }
    else
    {
      aLTrim = new Geom2d_TrimmedCurve(aCirc, anId * aDelta, (anId + 1) * aDelta);
    }
    aRes.Add(aLTrim, Precision::PConfusion());
  }

  const occ::handle<Geom2d_BSplineCurve> aBSpline = aRes.BSplineCurve();
  ASSERT_FALSE(aBSpline.IsNull());
  EXPECT_TRUE(aBSpline->IsClosed()) << "Assembled BSpline curve from closed circle arcs must be closed";
}
