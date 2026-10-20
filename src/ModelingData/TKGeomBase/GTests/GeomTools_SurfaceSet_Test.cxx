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

#include <GeomTools_SurfaceSet.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <sstream>

// Regression test for issue #1434: Add() must drop a null handle rather than binding it, matching
// GeomTools_CurveSet::Add's existing behavior. Add() accepting the null used to defer the crash
// to Write(), which dereferences it.
TEST(GeomTools_SurfaceSetTest, Add_NullHandle_IsDropped)
{
  GeomTools_SurfaceSet      aSet;
  occ::handle<Geom_Surface> aNullSurface;
  EXPECT_EQ(aSet.Add(aNullSurface), 0);
}

TEST(GeomTools_SurfaceSetTest, Write_AfterAddingNullHandle_DoesNotCrash)
{
  GeomTools_SurfaceSet      aSet;
  occ::handle<Geom_Surface> aNullSurface;
  aSet.Add(aNullSurface);

  std::ostringstream anOStream;
  EXPECT_NO_THROW(aSet.Write(anOStream));
}

TEST(GeomTools_SurfaceSetTest, Index_NullHandle_ReturnsZero)
{
  GeomTools_SurfaceSet      aSet;
  occ::handle<Geom_Surface> aNullSurface;
  aSet.Add(aNullSurface);
  EXPECT_EQ(aSet.Index(aNullSurface), 0);
}

TEST(GeomTools_SurfaceSetTest, Add_ValidHandle_IsUnaffected)
{
  GeomTools_SurfaceSet      aSet;
  occ::handle<Geom_Surface> aPlane  = new Geom_Plane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  const int                 anIndex = aSet.Add(aPlane);
  EXPECT_EQ(anIndex, 1);
  EXPECT_EQ(aSet.Index(aPlane), 1);

  std::ostringstream anOStream;
  EXPECT_NO_THROW(aSet.Write(anOStream));
}
