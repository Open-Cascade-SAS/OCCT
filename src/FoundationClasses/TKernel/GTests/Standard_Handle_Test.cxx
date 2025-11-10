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

#include <Standard_Transient.hxx>
#include <Standard_Handle.hxx>

#include <gtest/gtest.h>

TEST(Standard_Handle_Test, OCC24533_IsNullAndPointerChecks)
{
  Handle(Standard_Transient) aHandle;

  // Test IsNull() for default-constructed handle
  EXPECT_TRUE(aHandle.IsNull());

  // Test pointer access for null handle
  const Standard_Transient* p = aHandle.get();
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_EQ(nullptr, p);

  // Create non-null handle
  aHandle = new Standard_Transient();

  // Test IsNull() for non-null handle
  EXPECT_FALSE(aHandle.IsNull());

  // Test pointer access for non-null handle
  p = aHandle.get();
  EXPECT_TRUE(static_cast<bool>(p));
  EXPECT_NE(nullptr, p);
}
