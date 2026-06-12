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

#include <BRepGraph_ItemId.hxx>

#include <gtest/gtest.h>

TEST(BRepGraph_ItemIdTest, Default_IsInvalid)
{
  const BRepGraph_ItemId anItem;

  EXPECT_FALSE(anItem.IsValid());
  EXPECT_FALSE(anItem.IsNode());
  EXPECT_FALSE(anItem.IsReference());
}

TEST(BRepGraph_ItemIdTest, NodeRoundTrip)
{
  const BRepGraph_NodeId aNode(BRepGraph_NodeId::Kind::Face, 3u);
  const BRepGraph_ItemId anItem(aNode);

  EXPECT_TRUE(anItem.IsValid());
  EXPECT_TRUE(anItem.IsNode());
  EXPECT_FALSE(anItem.IsReference());
  EXPECT_EQ(anItem.ItemDomain(), BRepGraph_ItemId::Domain::Node);
  EXPECT_EQ(anItem.NodeKind(), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anItem.NodeId(), aNode);
  EXPECT_FALSE(anItem.RefId().IsValid());
}

TEST(BRepGraph_ItemIdTest, ReferenceRoundTrip)
{
  const BRepGraph_RefId  aRef(BRepGraph_RefId::Kind::Wire, 4u);
  const BRepGraph_ItemId anItem(aRef);

  EXPECT_TRUE(anItem.IsValid());
  EXPECT_FALSE(anItem.IsNode());
  EXPECT_TRUE(anItem.IsReference());
  EXPECT_EQ(anItem.ItemDomain(), BRepGraph_ItemId::Domain::Reference);
  EXPECT_EQ(anItem.RefKind(), BRepGraph_RefId::Kind::Wire);
  EXPECT_EQ(anItem.RefId(), aRef);
  EXPECT_FALSE(anItem.NodeId().IsValid());
}

TEST(BRepGraph_ItemIdTest, InvalidSourceKindProducesInvalidItem)
{
  constexpr uint32_t     THE_RESERVED_KIND = 9u;
  const BRepGraph_NodeId aNode(static_cast<BRepGraph_NodeId::Kind>(THE_RESERVED_KIND), 0u);
  const BRepGraph_ItemId anItem(aNode);

  EXPECT_FALSE(aNode.IsValid());
  EXPECT_FALSE(anItem.IsValid());
  EXPECT_EQ(anItem.ItemDomain(), BRepGraph_ItemId::Domain::None);
}
