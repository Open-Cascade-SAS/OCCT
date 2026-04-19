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

#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerIterator.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_NodeId.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

namespace
{
//! Minimal concrete layer for testing the iterator.
class TestLayer : public BRepGraph_Layer
{
public:
  TestLayer(const Standard_GUID& theId, const TCollection_AsciiString& theName)
      : myId(theId),
        myName(theName)
  {
  }

  const Standard_GUID& ID() const override { return myId; }

  const TCollection_AsciiString& Name() const override { return myName; }

  void OnNodeRemoved(const BRepGraph_NodeId /*theNode*/,
                     const BRepGraph_NodeId /*theReplacement*/) noexcept override
  {
  }

  void OnCompact(const NCollection_DataMap<BRepGraph_NodeId,
                                           BRepGraph_NodeId>& /*theRemapMap*/) noexcept override
  {
  }

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override {}

private:
  Standard_GUID           myId;
  TCollection_AsciiString myName;
};
} // namespace

TEST(BRepGraph_LayerIteratorTest, EmptyRegistry_IsEmpty)
{
  BRepGraph_LayerRegistry aRegistry;
  BRepGraph_LayerIterator anIt(aRegistry);
  EXPECT_FALSE(anIt.More());
  EXPECT_EQ(anIt.NbLayers(), 0);
}

TEST(BRepGraph_LayerIteratorTest, SingleLayer_IteratesOnce)
{
  BRepGraph_LayerRegistry aRegistry;
  occ::handle<TestLayer>  aLayer =
    new TestLayer(Standard_GUID("aaaaaaaa-1111-2222-3333-444444444444"), "TestA");
  aRegistry.RegisterLayer(aLayer);

  BRepGraph_LayerIterator anIt(aRegistry);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.NbLayers(), 1);
  EXPECT_EQ(anIt.Slot(), 0);
  EXPECT_FALSE(anIt.Value().IsNull());
  EXPECT_EQ(anIt.Value()->Name(), TCollection_AsciiString("TestA"));

  anIt.Next();
  EXPECT_FALSE(anIt.More());
}

TEST(BRepGraph_LayerIteratorTest, MultipleLayers_IteratesAll)
{
  BRepGraph_LayerRegistry aRegistry;
  occ::handle<TestLayer>  aLayerA =
    new TestLayer(Standard_GUID("aaaaaaaa-1111-2222-3333-444444444444"), "LayerA");
  occ::handle<TestLayer> aLayerB =
    new TestLayer(Standard_GUID("bbbbbbbb-1111-2222-3333-444444444444"), "LayerB");
  occ::handle<TestLayer> aLayerC =
    new TestLayer(Standard_GUID("cccccccc-1111-2222-3333-444444444444"), "LayerC");
  aRegistry.RegisterLayer(aLayerA);
  aRegistry.RegisterLayer(aLayerB);
  aRegistry.RegisterLayer(aLayerC);

  int aCount = 0;
  for (BRepGraph_LayerIterator anIt(aRegistry); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Value().IsNull());
    EXPECT_EQ(anIt.Slot(), aCount);
    ++aCount;
  }
  EXPECT_EQ(aCount, 3);
}

TEST(BRepGraph_LayerIteratorTest, RangeFor_WorksCorrectly)
{
  BRepGraph_LayerRegistry aRegistry;
  occ::handle<TestLayer>  aLayerA =
    new TestLayer(Standard_GUID("aaaaaaaa-1111-2222-3333-444444444444"), "LayerA");
  occ::handle<TestLayer> aLayerB =
    new TestLayer(Standard_GUID("bbbbbbbb-1111-2222-3333-444444444444"), "LayerB");
  aRegistry.RegisterLayer(aLayerA);
  aRegistry.RegisterLayer(aLayerB);

  int aCount = 0;
  for (const occ::handle<BRepGraph_Layer>& aLayer : BRepGraph_LayerIterator(aRegistry))
  {
    EXPECT_FALSE(aLayer.IsNull());
    ++aCount;
  }
  EXPECT_EQ(aCount, 2);
}
