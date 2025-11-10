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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>

// Test BUC60828: TopoDS_Edge Infinite flag getter/setter
// Migrated from QABugs_16.cxx
TEST(TopoDS_Edge_Test, BUC60828_InfiniteFlag)
{
  // Create a simple edge
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(0., 0., 1.));

  // Check initial flag value (should be false by default)
  Standard_Boolean anInitialValue = anEdge.Infinite();
  EXPECT_FALSE(anInitialValue) << "Initial Infinite flag should be false";

  // Set the flag to true
  anEdge.Infinite(Standard_True);

  // Verify the flag was set correctly
  Standard_Boolean aCurrentValue = anEdge.Infinite();
  EXPECT_TRUE(aCurrentValue) << "Infinite flag should be true after setting";
}
