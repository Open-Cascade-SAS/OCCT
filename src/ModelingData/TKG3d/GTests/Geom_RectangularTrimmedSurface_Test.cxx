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
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cstring>
#include <limits>
#include <new>

namespace
{
constexpr double THE_NaN = std::numeric_limits<double>::quiet_NaN();
constexpr double THE_Inf = std::numeric_limits<double>::infinity();

//! Builds the trimmed surface of theSurface bounded by (U1, U2, V1, V2) = theBounds.
void trimByBounds(const occ::handle<Geom_Surface>& theSurface, const double (&theBounds)[4])
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(theSurface,
                                       theBounds[0],
                                       theBounds[1],
                                       theBounds[2],
                                       theBounds[3]);
}

//! Builds the surface of theSurface trimmed in one direction by (theParam1, theParam2).
void trimByParameters(const occ::handle<Geom_Surface>& theSurface,
                      const double                     theParam1,
                      const double                     theParam2,
                      const bool                       theIsUTrim)
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(theSurface, theParam1, theParam2, theIsUTrim);
}
} // namespace

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

//==================================================================================================

// The U direction of a plane and the V direction of a cylinder are not periodic.
TEST(Geom_RectangularTrimmedSurface, Constructor_NonFiniteBound_NonPeriodicDirection_Throws)
{
  const occ::handle<Geom_Surface> aPlane    = new Geom_Plane(gp_Ax3());
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const double aBad : {THE_NaN, THE_Inf, -THE_Inf})
  {
    for (int aBoundIndex = 0; aBoundIndex < 4; ++aBoundIndex)
    {
      SCOPED_TRACE(aBad);
      SCOPED_TRACE(aBoundIndex);
      double aBounds[4]    = {0.5, 1.5, 2.5, 3.5};
      aBounds[aBoundIndex] = aBad;
      EXPECT_THROW(trimByBounds(aPlane, aBounds), Standard_ConstructionError);
      if (aBoundIndex >= 2)
      {
        EXPECT_THROW(trimByBounds(aCylinder, aBounds), Standard_ConstructionError);
      }
    }
  }
}

//==================================================================================================

// The U direction of a cylinder is periodic.
TEST(Geom_RectangularTrimmedSurface, Constructor_NonFiniteBound_PeriodicDirection_Throws)
{
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const double aBad : {THE_NaN, THE_Inf, -THE_Inf})
  {
    for (int aBoundIndex = 0; aBoundIndex < 2; ++aBoundIndex)
    {
      SCOPED_TRACE(aBad);
      SCOPED_TRACE(aBoundIndex);
      double aBounds[4]    = {0.5, 1.5, 2.5, 3.5};
      aBounds[aBoundIndex] = aBad;
      EXPECT_THROW(trimByBounds(aCylinder, aBounds), Standard_ConstructionError);
    }
  }
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, Constructor_NonFiniteParameter_NonPeriodicDirection_Throws)
{
  const occ::handle<Geom_Surface> aPlane    = new Geom_Plane(gp_Ax3());
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const bool isFirst : {false, true})
  {
    SCOPED_TRACE(isFirst);
    const double aParam1 = isFirst ? THE_NaN : 0.5;
    const double aParam2 = isFirst ? 1.5 : THE_NaN;
    EXPECT_THROW(trimByParameters(aPlane, aParam1, aParam2, true), Standard_ConstructionError);
    EXPECT_THROW(trimByParameters(aPlane, aParam1, aParam2, false), Standard_ConstructionError);
    EXPECT_THROW(trimByParameters(aCylinder, aParam1, aParam2, false), Standard_ConstructionError);
  }
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, Constructor_NonFiniteParameter_PeriodicDirection_Throws)
{
  const occ::handle<Geom_Surface> aCylinder = new Geom_CylindricalSurface(gp_Ax3(), 5.0);
  for (const bool isFirst : {false, true})
  {
    SCOPED_TRACE(isFirst);
    const double aParam1 = isFirst ? THE_NaN : 0.5;
    const double aParam2 = isFirst ? 1.5 : THE_NaN;
    EXPECT_THROW(trimByParameters(aCylinder, aParam1, aParam2, true), Standard_ConstructionError);
  }
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, SetTrim_NonFiniteBound_ThrowsAndKeepsTrim)
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(new Geom_Plane(gp_Ax3()), 0.5, 1.5, 2.5, 3.5);
  // A finite U pair comes with a non-finite V bound: the trim must stay as it was.
  EXPECT_THROW(aTrim->SetTrim(0.0, 1.0, 0.0, THE_NaN), Standard_ConstructionError);
  EXPECT_THROW(aTrim->SetTrim(0.0, 1.0, THE_Inf, 1.0), Standard_ConstructionError);
  EXPECT_THROW(aTrim->SetTrim(THE_NaN, 1.0, true), Standard_ConstructionError);
  EXPECT_THROW(aTrim->SetTrim(0.0, THE_NaN, false), Standard_ConstructionError);
  double aU1, aU2, aV1, aV2;
  aTrim->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, 0.5);
  EXPECT_DOUBLE_EQ(aU2, 1.5);
  EXPECT_DOUBLE_EQ(aV1, 2.5);
  EXPECT_DOUBLE_EQ(aV2, 3.5);
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, SetTrim_NonFiniteBound_PeriodicDirection_ThrowsAndKeepsTrim)
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(new Geom_CylindricalSurface(gp_Ax3(), 5.0),
                                       0.5,
                                       1.5,
                                       2.5,
                                       3.5);
  EXPECT_THROW(aTrim->SetTrim(0.0, THE_NaN, 2.0, 3.0), Standard_ConstructionError);
  EXPECT_THROW(aTrim->SetTrim(THE_Inf, 1.0, true), Standard_ConstructionError);
  double aU1, aU2, aV1, aV2;
  aTrim->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, 0.5);
  EXPECT_DOUBLE_EQ(aU2, 1.5);
  EXPECT_DOUBLE_EQ(aV1, 2.5);
  EXPECT_DOUBLE_EQ(aV2, 3.5);
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, Constructor_ReversedFiniteBounds_ReversesSurface)
{
  const occ::handle<Geom_Surface>                   aPlane = new Geom_Plane(gp_Ax3());
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(aPlane, 1.5, 0.5, 3.5, 2.5);
  double aU1, aU2, aV1, aV2;
  aTrim->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, -1.5);
  EXPECT_DOUBLE_EQ(aU2, -0.5);
  EXPECT_DOUBLE_EQ(aV1, -3.5);
  EXPECT_DOUBLE_EQ(aV2, -2.5);
}

//==================================================================================================

TEST(Geom_RectangularTrimmedSurface, Constructor_UnboundedParameters_KeepsUnboundedTrim)
{
  // Precision::Infinite() is the finite number that stands for an unbounded direction.
  const occ::handle<Geom_Surface>                   aPlane = new Geom_Plane(gp_Ax3());
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    new Geom_RectangularTrimmedSurface(aPlane,
                                       -Precision::Infinite(),
                                       Precision::Infinite(),
                                       -Precision::Infinite(),
                                       Precision::Infinite());
  double aU1, aU2, aV1, aV2;
  aTrim->Bounds(aU1, aU2, aV1, aV2);
  EXPECT_DOUBLE_EQ(aU1, -Precision::Infinite());
  EXPECT_DOUBLE_EQ(aU2, Precision::Infinite());
  EXPECT_DOUBLE_EQ(aV1, -Precision::Infinite());
  EXPECT_DOUBLE_EQ(aV2, Precision::Infinite());
}
