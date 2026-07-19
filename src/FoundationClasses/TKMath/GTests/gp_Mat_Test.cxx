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

#include <gp_Mat.hxx>

#include <gtest/gtest.h>

TEST(gp_MatTest, OCC22595_DefaultConstructor)
{
  gp_Mat aM0;

  // Bug OCC22595: gp_Mat's constructors incompletely initialize memory
  // Test that default constructor properly initializes all matrix elements to zero
  EXPECT_DOUBLE_EQ(0.0, aM0(1, 1));
  EXPECT_DOUBLE_EQ(0.0, aM0(1, 2));
  EXPECT_DOUBLE_EQ(0.0, aM0(1, 3));

  EXPECT_DOUBLE_EQ(0.0, aM0(2, 1));
  EXPECT_DOUBLE_EQ(0.0, aM0(2, 2));
  EXPECT_DOUBLE_EQ(0.0, aM0(2, 3));

  EXPECT_DOUBLE_EQ(0.0, aM0(3, 1));
  EXPECT_DOUBLE_EQ(0.0, aM0(3, 2));
  EXPECT_DOUBLE_EQ(0.0, aM0(3, 3));
}
