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

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Handle.hxx>

//=================================================================================================

// Bug OCC867: calling Init() with explicit UV bounds then Perform() must not throw.
// The bounds passed to Init() are intentionally tighter (U:[0,3]) than the trimmed surface
// (U:[0,4]) to reproduce the original bug scenario.
TEST(GeomAPI_ProjectPointOnSurfTest, Bug867_InitWithTightBoundsNoException)
{
  const occ::handle<Geom_CylindricalSurface> aCyl =
    new Geom_CylindricalSurface(gp::XOY(), 10.0);
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimSurf =
    new Geom_RectangularTrimmedSurface(aCyl, 0.0, 4.0, 0.0, 2.0);

  const gp_Pnt aPoint(30.0, 30.0, 30.0);

  GeomAPI_ProjectPointOnSurf aProjector;
  EXPECT_NO_THROW({
    aProjector.Init(aTrimSurf, 0.0, 3.0, 0.0, 2.0);
    aProjector.Perform(aPoint);
  });
  // The original bug (OCC867) was that Init()+Perform() threw an exception.
  // Whether a projection exists within the given UV bounds is not the concern here.
}
