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

#include <BRepGraph.hxx>
#include <BRepGraph_LayerLock.hxx>
#include <BRepGraph_LayerParametric.hxx>
#include <BRepGraph_LayerRegistry.hxx>

#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>
#include <gtest/gtest.h>

namespace
{
class TestParametricLayer : public BRepGraph_LayerParametric
{
public:
  static const Standard_GUID& GetID()
  {
    static const Standard_GUID THE_ID("d7c2f1e8-8a54-462c-8790-f6ff0ecbd8f1");
    return THE_ID;
  }

  const Standard_GUID& ID() const override { return GetID(); }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("TestParametricLayer");
    return THE_NAME;
  }

  BRepGraph* ExposedGraphForMutation() const { return graphForMutation(); }

  occ::handle<BRepGraph_LayerLock> ExposedLockLayer(BRepGraph& theGraph) const
  {
    return lockLayer(theGraph);
  }

  void CopyTo(const BRepGraph_CopyRemap&) const override {}

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override {}
};
} // namespace

TEST(BRepGraph_LayerParametricTest, GenerationFlagsUseStableBitMasks)
{
  using Flag = BRepGraph_LayerParametric::GenerationFlag;

  EXPECT_EQ(BRepGraph_LayerParametric::GenerationMask(Flag::Topology), 0x01u);
  EXPECT_EQ(BRepGraph_LayerParametric::GenerationMask(Flag::Geometry), 0x02u);
  EXPECT_EQ(BRepGraph_LayerParametric::GenerationMask(Flag::Mesh), 0x04u);

  EXPECT_TRUE(BRepGraph_LayerParametric::HasGenerationFlag(
    BRepGraph_LayerParametric::THE_DEFAULT_GENERATION_FLAGS,
    Flag::Topology));
  EXPECT_TRUE(BRepGraph_LayerParametric::HasGenerationFlag(
    BRepGraph_LayerParametric::THE_DEFAULT_GENERATION_FLAGS,
    Flag::Geometry));
  EXPECT_FALSE(BRepGraph_LayerParametric::HasGenerationFlag(
    BRepGraph_LayerParametric::THE_DEFAULT_GENERATION_FLAGS,
    Flag::Mesh));

  const uint32_t aMeshOnly = BRepGraph_LayerParametric::GenerationMask(Flag::Mesh);
  EXPECT_FALSE(BRepGraph_LayerParametric::HasGenerationFlag(aMeshOnly, Flag::Topology));
  EXPECT_TRUE(BRepGraph_LayerParametric::HasGenerationFlag(aMeshOnly, Flag::Mesh));
}

TEST(BRepGraph_LayerParametricTest, MeshQualityValueSelectsQualityLadderValue)
{
  using Quality = BRepGraph_LayerParametric::MeshQuality;

  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(Quality::VeryCoarse, 1u, 2u, 3u, 4u, 5u),
            1u);
  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(Quality::Coarse, 1u, 2u, 3u, 4u, 5u),
            2u);
  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(Quality::Medium, 1u, 2u, 3u, 4u, 5u),
            3u);
  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(Quality::Fine, 1u, 2u, 3u, 4u, 5u),
            4u);
  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(Quality::VeryFine, 1u, 2u, 3u, 4u, 5u),
            5u);
  EXPECT_EQ(BRepGraph_LayerParametric::MeshQualityValue(static_cast<Quality>(255),
                                                        1u,
                                                        2u,
                                                        3u,
                                                        4u,
                                                        5u),
            3u);
}

TEST(BRepGraph_LayerParametricTest, GraphHelpersRequireAttachmentAndCreateLockLayer)
{
  occ::handle<TestParametricLayer> aLayer = new TestParametricLayer();
  EXPECT_THROW(
    {
      BRepGraph* aGraph = aLayer->ExposedGraphForMutation();
      (void)aGraph;
    },
    Standard_ProgramError);

  BRepGraph aGraph;
  aGraph.Clear();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  EXPECT_EQ(aLayer->ExposedGraphForMutation(), &aGraph);
  EXPECT_TRUE(aGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>().IsNull());

  occ::handle<BRepGraph_LayerLock> aLockLayer = aLayer->ExposedLockLayer(aGraph);
  ASSERT_FALSE(aLockLayer.IsNull());
  EXPECT_EQ(aGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>(), aLockLayer);
}
