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

#include <GeomTools_Curve2dSet.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <sstream>

// Regression test for issue #1434: Add() must drop a null handle rather than binding it, matching
// GeomTools_CurveSet::Add's existing behavior. Add() accepting the null used to defer the crash
// to Write(), which dereferences it.
TEST(GeomTools_Curve2dSetTest, Add_NullHandle_IsDropped)
{
  GeomTools_Curve2dSet      aSet;
  occ::handle<Geom2d_Curve> aNullCurve;
  EXPECT_EQ(aSet.Add(aNullCurve), 0);
}

TEST(GeomTools_Curve2dSetTest, Write_AfterAddingNullHandle_DoesNotCrash)
{
  GeomTools_Curve2dSet      aSet;
  occ::handle<Geom2d_Curve> aNullCurve;
  aSet.Add(aNullCurve);

  std::ostringstream anOStream;
  EXPECT_NO_THROW(aSet.Write(anOStream));
}

TEST(GeomTools_Curve2dSetTest, Index_NullHandle_ReturnsZero)
{
  GeomTools_Curve2dSet      aSet;
  occ::handle<Geom2d_Curve> aNullCurve;
  aSet.Add(aNullCurve);
  EXPECT_EQ(aSet.Index(aNullCurve), 0);
}

TEST(GeomTools_Curve2dSetTest, Add_ValidHandle_IsUnaffected)
{
  GeomTools_Curve2dSet      aSet;
  occ::handle<Geom2d_Curve> aLine   = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  const int                 anIndex = aSet.Add(aLine);
  EXPECT_EQ(anIndex, 1);
  EXPECT_EQ(aSet.Index(aLine), 1);

  std::ostringstream anOStream;
  EXPECT_NO_THROW(aSet.Write(anOStream));
}
