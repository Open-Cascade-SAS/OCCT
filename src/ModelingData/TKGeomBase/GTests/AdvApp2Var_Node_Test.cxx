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

#include <AdvApp2Var_Node.hxx>
#include <gp_Pnt.hxx>

TEST(AdvApp2Var_NodeTest, Constructor_InitializesValuesToZero)
{
  AdvApp2Var_Node aNode(1, 2);
  for (int aUOrder = 0; aUOrder <= 1; ++aUOrder)
  {
    for (int aVOrder = 0; aVOrder <= 2; ++aVOrder)
    {
      const gp_Pnt& aPoint = aNode.Point(aUOrder, aVOrder);
      EXPECT_DOUBLE_EQ(aPoint.X(), 0.0);
      EXPECT_DOUBLE_EQ(aPoint.Y(), 0.0);
      EXPECT_DOUBLE_EQ(aPoint.Z(), 0.0);
      EXPECT_DOUBLE_EQ(aNode.Error(aUOrder, aVOrder), 0.0);
    }
  }
}
