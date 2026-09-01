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

#include <Contap_HContTool.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

TEST(Contap_HContToolTest, SurfaceSamplingHasNoCrossCallState)
{
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(gp::XOY()));
  const occ::handle<Adaptor3d_Surface> aFirstSurface =
    new GeomAdaptor_Surface(aPlane, 0.0, 4.0, 10.0, 14.0);
  const occ::handle<Adaptor3d_Surface> aSecondSurface =
    new GeomAdaptor_Surface(aPlane, -100.0, -80.0, -40.0, -20.0);

  EXPECT_EQ(Contap_HContTool::NbSamplePoints(aFirstSurface), 5);
  EXPECT_EQ(Contap_HContTool::NbSamplePoints(aSecondSurface), 5);

  double aU = 0.0;
  double aV = 0.0;
  Contap_HContTool::SamplePoint(aFirstSurface, 1, aU, aV);
  EXPECT_DOUBLE_EQ(aU, 1.0);
  EXPECT_DOUBLE_EQ(aV, 11.0);
}
