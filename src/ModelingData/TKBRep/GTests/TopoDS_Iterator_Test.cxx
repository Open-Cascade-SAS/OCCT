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

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(TopoDS_Iterator_Test, OCC30708_1_InitializeWithNullShape)
{
  TopoDS_Iterator it;
  TopoDS_Shape    empty;

  // Should not throw exception when initializing with null shape
  EXPECT_NO_THROW(it.Initialize(empty));

  // More() should return false on null shape
  EXPECT_FALSE(it.More());
}
