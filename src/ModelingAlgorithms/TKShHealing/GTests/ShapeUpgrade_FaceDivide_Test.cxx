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

#include <BRepPrimAPI_MakeBox.hxx>
#include <OSD_Parallel.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <atomic>

// Test for issue #1179: ShapeUpgrade_FaceDivide crashes with null context.
TEST(ShapeUpgrade_FaceDivideTest, Perform_WithoutContext_DoesNotCrash)
{
  for (int i = 0; i < 10; ++i)
  {
    const double       aSize = 10.0 + i * 0.5;
    const TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(aSize, aSize + 5, aSize + 10).Shape();
    TopExp_Explorer    anExp(aBox, TopAbs_FACE);
    ASSERT_TRUE(anExp.More()) << "No faces found in box";

    ShapeUpgrade_FaceDivide aDivider(TopoDS::Face(anExp.Current()));
    EXPECT_NO_THROW(aDivider.Perform());
  }
}

// Test for issue #1179: ShapeUpgrade_FaceDivide is safe to call concurrently on independent faces.
TEST(ShapeUpgrade_FaceDivideTest, Perform_Parallel_DoesNotCrash)
{
  std::atomic<bool> anError{false};
  OSD_Parallel::For(0, 100, [&](int theIndex) {
    const double       aSize = 10.0 + theIndex * 0.1;
    const TopoDS_Shape aBox  = BRepPrimAPI_MakeBox(aSize, aSize + 5, aSize + 10).Shape();
    TopExp_Explorer    anExp(aBox, TopAbs_FACE);
    if (!anExp.More())
    {
      anError.store(true, std::memory_order_relaxed);
      return;
    }
    try
    {
      ShapeUpgrade_FaceDivide aDivider(TopoDS::Face(anExp.Current()));
      aDivider.Perform();
    }
    catch (...)
    {
      anError.store(true, std::memory_order_relaxed);
    }
  });

  EXPECT_FALSE(anError.load()) << "FaceDivide failed in parallel execution";
}
