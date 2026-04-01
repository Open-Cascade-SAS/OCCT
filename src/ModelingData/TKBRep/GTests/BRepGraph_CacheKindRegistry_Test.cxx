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

#include <BRepGraph_TransientCache.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_CacheKindRegistryTest, Register_SameGUID_SameSlot)
{
  const Standard_GUID                    aGUID("a1b2c3d4-1111-2222-3333-444455556666");
  const occ::handle<BRepGraph_CacheKind> aKind1 = new BRepGraph_CacheKind(aGUID, "SameGUID");
  const occ::handle<BRepGraph_CacheKind> aKind2 = new BRepGraph_CacheKind(aGUID, "SameGUID");
  const int                              aSlot1 = BRepGraph_CacheKindRegistry::Register(aKind1);
  const int                              aSlot2 = BRepGraph_CacheKindRegistry::Register(aKind2);
  EXPECT_EQ(aSlot1, aSlot2);
}

TEST(BRepGraph_CacheKindRegistryTest, Register_DifferentGUID_DifferentSlot)
{
  const occ::handle<BRepGraph_CacheKind> aKind1 =
    new BRepGraph_CacheKind(Standard_GUID("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0001"), "Kind1");
  const occ::handle<BRepGraph_CacheKind> aKind2 =
    new BRepGraph_CacheKind(Standard_GUID("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0002"), "Kind2");
  const int aSlot1 = BRepGraph_CacheKindRegistry::Register(aKind1);
  const int aSlot2 = BRepGraph_CacheKindRegistry::Register(aKind2);
  EXPECT_NE(aSlot1, aSlot2);
}

TEST(BRepGraph_CacheKindRegistryTest, FindSlot_ByGUID_ReturnsCorrectSlot)
{
  const occ::handle<BRepGraph_CacheKind> aKind =
    new BRepGraph_CacheKind(Standard_GUID("c1c2c3c4-1111-2222-3333-aabbccddeeff"), "FindByGUID");
  const int aExpectedSlot = BRepGraph_CacheKindRegistry::Register(aKind);

  int        aFoundSlot = -1;
  const bool aOk        = BRepGraph_CacheKindRegistry::FindSlot(aKind->ID(), aFoundSlot);
  EXPECT_TRUE(aOk);
  EXPECT_EQ(aFoundSlot, aExpectedSlot);
}

TEST(BRepGraph_CacheKindRegistryTest, FindKind_BySlot_ReturnsCorrectDescriptor)
{
  const occ::handle<BRepGraph_CacheKind> aKind =
    new BRepGraph_CacheKind(Standard_GUID("d1d2d3d4-5555-6666-7777-888899990000"), "FindBySlot");
  const int aSlot = BRepGraph_CacheKindRegistry::Register(aKind);

  const occ::handle<BRepGraph_CacheKind> aFound = BRepGraph_CacheKindRegistry::FindKind(aSlot);
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_TRUE(aFound->ID() == aKind->ID());
  EXPECT_TRUE(aFound->Name().IsEqual("FindBySlot"));
}
