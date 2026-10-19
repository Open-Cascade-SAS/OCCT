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

#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cstring>
#include <new>

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, SingleDirectionTrimInitializesState)
{
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const bool isOffset : {false, true})
  {
    const occ::handle<Geom_Surface> aBasis =
      isOffset ? occ::handle<Geom_Surface>(new Geom_OffsetSurface(aCylinder, 1.0)) : aCylinder;
    for (const bool isUTrim : {false, true})
    {
      SCOPED_TRACE(isOffset);
      SCOPED_TRACE(isUTrim);
      // Nonzero storage makes uninitialized trim flags observable.
      alignas(Geom_RectangularTrimmedSurface) unsigned char
        aStorage[sizeof(Geom_RectangularTrimmedSurface)];
      std::memset(aStorage, 1, sizeof(aStorage));
      Geom_RectangularTrimmedSurface* aTrim = ::new (static_cast<void*>(aStorage))
        Geom_RectangularTrimmedSurface(aBasis, 0.5, 1.5, isUTrim, true);
      double aU1, aU2, aV1, aV2;
      double aBasisU1, aBasisU2, aBasisV1, aBasisV2;
      aTrim->Bounds(aU1, aU2, aV1, aV2);
      aBasis->Bounds(aBasisU1, aBasisU2, aBasisV1, aBasisV2);
      EXPECT_DOUBLE_EQ(aU1, isUTrim ? 0.5 : aBasisU1);
      EXPECT_DOUBLE_EQ(aU2, isUTrim ? 1.5 : aBasisU2);
      EXPECT_DOUBLE_EQ(aV1, isUTrim ? aBasisV1 : 0.5);
      EXPECT_DOUBLE_EQ(aV2, isUTrim ? aBasisV2 : 1.5);
      EXPECT_NEAR(aTrim->EvalD0(1.0, 1.0).Distance(aBasis->EvalD0(1.0, 1.0)), 0.0, 1.e-12);
      aTrim->~Geom_RectangularTrimmedSurface();
    }
  }
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, ComplementaryTrimAndCopyPreserveBounds)
{
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const bool isUTrim : {false, true})
  {
    const occ::handle<Geom_RectangularTrimmedSurface> aFirst =
      new Geom_RectangularTrimmedSurface(aCylinder, 0.5, 1.5, isUTrim, true);
    const occ::handle<Geom_RectangularTrimmedSurface> aCopy =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aFirst->Copy());
    ASSERT_FALSE(aCopy.IsNull());
    aCopy->SetTrim(2.0, 3.0, !isUTrim, true);
    const occ::handle<Geom_RectangularTrimmedSurface> aNested =
      new Geom_RectangularTrimmedSurface(aFirst, 2.0, 3.0, !isUTrim, true);
    for (const auto& aTrim : {aCopy, aNested})
    {
      double aU1, aU2, aV1, aV2;
      aTrim->Bounds(aU1, aU2, aV1, aV2);
      EXPECT_DOUBLE_EQ(aU1, isUTrim ? 0.5 : 2.0);
      EXPECT_DOUBLE_EQ(aU2, isUTrim ? 1.5 : 3.0);
      EXPECT_DOUBLE_EQ(aV1, isUTrim ? 2.0 : 0.5);
      EXPECT_DOUBLE_EQ(aV2, isUTrim ? 3.0 : 1.5);
    }
  }
}
