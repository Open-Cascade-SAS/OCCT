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

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Parallel.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <cmath>
#include <cstdio>

namespace
{

TCollection_AsciiString BrepPath()
{
  return TCollection_AsciiString("/tmp/occt_brep_parallel_test.brep");
}

void WriteBoxBrep(const TCollection_AsciiString& thePath)
{
  BRepPrimAPI_MakeBox aBox(10.0, 20.0, 30.0);
  ASSERT_FALSE(aBox.Shape().IsNull());
  ASSERT_TRUE(BRepTools::Write(aBox.Shape(), thePath.ToCString()));
}

} // namespace

// Parallel BRep reading must produce shapes with identical volume properties.
// The underlying bug was that Strtod() shared a single Bigint memory pool
// across all threads, causing random mis-parses of numeric values in .brep
// files when read concurrently.
TEST(BRepTools_Test, ParallelRead_IdenticalMass)
{
  const TCollection_AsciiString aPath = BrepPath();
  WriteBoxBrep(aPath);

  // Single-threaded reference
  TopoDS_Shape aRefShape;
  BRep_Builder aRefBuilder;
  ASSERT_TRUE(BRepTools::Read(aRefShape, aPath.ToCString(), aRefBuilder));
  GProp_GProps aRefProps;
  BRepGProp::VolumeProperties(aRefShape, aRefProps);
  const double aRefMass = std::abs(aRefProps.Mass());
  ASSERT_GT(aRefMass, 0.0);

  constexpr int aNbReads = 200;

  NCollection_Array1<TopoDS_Shape> aShapes(0, aNbReads - 1);
  std::atomic<int>                 aFailCount{0};

  OSD_Parallel::For(
    0,
    aNbReads,
    [&](int theIndex) {
      BRep_Builder aBuilder;
      TopoDS_Shape aShape;
      if (!BRepTools::Read(aShape, aPath.ToCString(), aBuilder) || aShape.IsNull())
      {
        aFailCount.fetch_add(1, std::memory_order_relaxed);
        return;
      }
      aShapes(theIndex) = aShape;
    });

  EXPECT_EQ(aFailCount.load(), 0) << "Some parallel reads failed";

  constexpr double aEps = 1.0e-6;
  for (int i = 0; i < aNbReads; ++i)
  {
    ASSERT_FALSE(aShapes(i).IsNull()) << "Shape " << i << " is null";
    GProp_GProps aProps;
    BRepGProp::VolumeProperties(aShapes(i), aProps);
    const double aMass = std::abs(aProps.Mass());
    EXPECT_NEAR(aMass, aRefMass, aEps)
      << "Shape " << i << " mass " << aMass << " != reference " << aRefMass;
  }

  std::remove(aPath.ToCString());
}

// Repeated parallel runs to catch intermittent failures.
TEST(BRepTools_Test, ParallelRead_RepeatedRuns)
{
  const TCollection_AsciiString aPath = BrepPath();
  WriteBoxBrep(aPath);

  TopoDS_Shape aRefShape;
  BRep_Builder aRefBuilder;
  ASSERT_TRUE(BRepTools::Read(aRefShape, aPath.ToCString(), aRefBuilder));
  GProp_GProps aRefProps;
  BRepGProp::VolumeProperties(aRefShape, aRefProps);
  const double aRefMass = std::abs(aRefProps.Mass());

  constexpr int aNbReads = 100;
  constexpr int aNbRuns  = 5;
  constexpr double aEps  = 1.0e-6;

  for (int aRun = 0; aRun < aNbRuns; ++aRun)
  {
    NCollection_Array1<TopoDS_Shape> aShapes(0, aNbReads - 1);
    std::atomic<int>                 aFailCount{0};

    OSD_Parallel::For(
      0,
      aNbReads,
      [&](int theIndex) {
        BRep_Builder aBuilder;
        TopoDS_Shape aShape;
        if (!BRepTools::Read(aShape, aPath.ToCString(), aBuilder) || aShape.IsNull())
        {
          aFailCount.fetch_add(1, std::memory_order_relaxed);
          return;
        }
        aShapes(theIndex) = aShape;
      });

    EXPECT_EQ(aFailCount.load(), 0) << "Run " << aRun << ": some reads failed";

    for (int i = 0; i < aNbReads; ++i)
    {
      ASSERT_FALSE(aShapes(i).IsNull()) << "Run " << aRun << ", shape " << i << " is null";
      GProp_GProps aProps;
      BRepGProp::VolumeProperties(aShapes(i), aProps);
      const double aMass = std::abs(aProps.Mass());
      EXPECT_NEAR(aMass, aRefMass, aEps)
        << "Run " << aRun << ", shape " << i << " mass mismatch";
    }
  }

  std::remove(aPath.ToCString());
}
