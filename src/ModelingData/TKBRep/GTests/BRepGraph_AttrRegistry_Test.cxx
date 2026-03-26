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

#include <BRepGraph_UserAttribute.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_AttrRegistryTest, Register_SameGUID_SameKey)
{
  const Standard_GUID aGUID("a1b2c3d4-1111-2222-3333-444455556666");
  const int           aKey1 = BRepGraph_AttrRegistry::Register(aGUID);
  const int           aKey2 = BRepGraph_AttrRegistry::Register(aGUID);
  EXPECT_EQ(aKey1, aKey2);
}

TEST(BRepGraph_AttrRegistryTest, Register_DifferentGUID_DifferentKey)
{
  const Standard_GUID aGUID1("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0001");
  const Standard_GUID aGUID2("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0002");
  const int           aKey1 = BRepGraph_AttrRegistry::Register(aGUID1);
  const int           aKey2 = BRepGraph_AttrRegistry::Register(aGUID2);
  EXPECT_NE(aKey1, aKey2);
}

TEST(BRepGraph_AttrRegistryTest, Find_ByGUID_ReturnsCorrectKey)
{
  const Standard_GUID aGUID("c1c2c3c4-1111-2222-3333-aabbccddeeff");
  const int           aExpectedKey = BRepGraph_AttrRegistry::Register(aGUID);

  int        aFoundKey = -1;
  const bool aOk       = BRepGraph_AttrRegistry::Find(aGUID, aFoundKey);
  EXPECT_TRUE(aOk);
  EXPECT_EQ(aFoundKey, aExpectedKey);
}

TEST(BRepGraph_AttrRegistryTest, Find_ByKey_ReturnsCorrectGUID)
{
  const Standard_GUID aGUID("d1d2d3d4-5555-6666-7777-888899990000");
  const int           aKey = BRepGraph_AttrRegistry::Register(aGUID);

  Standard_GUID aFoundGUID;
  const bool    aOk = BRepGraph_AttrRegistry::Find(aKey, aFoundGUID);
  EXPECT_TRUE(aOk);
  EXPECT_TRUE(aFoundGUID == aGUID);
}
