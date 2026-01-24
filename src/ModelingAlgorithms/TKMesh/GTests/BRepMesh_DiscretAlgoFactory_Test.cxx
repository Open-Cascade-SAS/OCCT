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

#include <BRepMesh_DiscretAlgoFactory.hxx>
#include <BRepMesh_DiscretFactory.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>

//! Test fixture for BRepMesh_DiscretAlgoFactory tests
class BRepMesh_DiscretAlgoFactoryTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple box shape for testing
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
    myBox = aBoxMaker.Shape();
  }

  TopoDS_Shape myBox;
};

//! Test that at least one factory is registered (FastDiscret from BRepMesh_IncrementalMeshFactory)
TEST_F(BRepMesh_DiscretAlgoFactoryTest, Factories_AtLeastOneRegistered)
{
  const NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>& aFactories =
    BRepMesh_DiscretAlgoFactory::Factories();

  EXPECT_FALSE(aFactories.IsEmpty()) << "No factories registered";
}

//! Test that DefaultFactory returns a valid factory
TEST_F(BRepMesh_DiscretAlgoFactoryTest, DefaultFactory_ReturnsValid)
{
  occ::handle<BRepMesh_DiscretAlgoFactory> aFactory = BRepMesh_DiscretAlgoFactory::DefaultFactory();

  EXPECT_FALSE(aFactory.IsNull()) << "DefaultFactory returned null";
}

//! Test that FastDiscret factory is registered and can be found
TEST_F(BRepMesh_DiscretAlgoFactoryTest, FindFactory_FastDiscret)
{
  occ::handle<BRepMesh_DiscretAlgoFactory> aFactory =
    BRepMesh_DiscretAlgoFactory::FindFactory("FastDiscret");

  EXPECT_FALSE(aFactory.IsNull()) << "FastDiscret factory not found";
  if (!aFactory.IsNull())
  {
    EXPECT_EQ(aFactory->Name(), "FastDiscret");
  }
}

//! Test that FindFactory returns null for non-existent factory
TEST_F(BRepMesh_DiscretAlgoFactoryTest, FindFactory_NonExistent_ReturnsNull)
{
  occ::handle<BRepMesh_DiscretAlgoFactory> aFactory =
    BRepMesh_DiscretAlgoFactory::FindFactory("NonExistentFactory");

  EXPECT_TRUE(aFactory.IsNull()) << "FindFactory should return null for non-existent factory";
}

//! Test that CreateAlgorithm creates a valid algorithm
TEST_F(BRepMesh_DiscretAlgoFactoryTest, CreateAlgorithm_ReturnsValid)
{
  occ::handle<BRepMesh_DiscretAlgoFactory> aFactory = BRepMesh_DiscretAlgoFactory::DefaultFactory();
  ASSERT_FALSE(aFactory.IsNull());

  occ::handle<BRepMesh_DiscretRoot> anAlgo = aFactory->CreateAlgorithm(myBox, 0.1, 0.5);

  EXPECT_FALSE(anAlgo.IsNull()) << "CreateAlgorithm returned null";
}

//! Test that created algorithm can mesh a shape
TEST_F(BRepMesh_DiscretAlgoFactoryTest, CreateAlgorithm_CanMesh)
{
  occ::handle<BRepMesh_DiscretAlgoFactory> aFactory = BRepMesh_DiscretAlgoFactory::DefaultFactory();
  ASSERT_FALSE(aFactory.IsNull());

  occ::handle<BRepMesh_DiscretRoot> anAlgo = aFactory->CreateAlgorithm(myBox, 0.1, 0.5);
  ASSERT_FALSE(anAlgo.IsNull());

  anAlgo->Perform();

  EXPECT_TRUE(anAlgo->IsDone()) << "Meshing failed";
}

//! Test that BRepMesh_DiscretFactory::Discret uses the new registry
TEST_F(BRepMesh_DiscretAlgoFactoryTest, DiscretFactory_UsesRegistry)
{
  BRepMesh_DiscretFactory& aFactory = BRepMesh_DiscretFactory::Get();

  occ::handle<BRepMesh_DiscretRoot> anAlgo = aFactory.Discret(myBox, 0.1, 0.5);

  EXPECT_FALSE(anAlgo.IsNull()) << "Discret returned null";
  if (!anAlgo.IsNull())
  {
    anAlgo->Perform();
    EXPECT_TRUE(anAlgo->IsDone()) << "Meshing failed";
  }
}

//! Test that SetDefaultName works with registry-based factories
TEST_F(BRepMesh_DiscretAlgoFactoryTest, DiscretFactory_SetDefaultName)
{
  BRepMesh_DiscretFactory& aFactory = BRepMesh_DiscretFactory::Get();

  bool isSuccess = aFactory.SetDefaultName("FastDiscret");
  EXPECT_TRUE(isSuccess) << "SetDefaultName(FastDiscret) failed";
  EXPECT_EQ(aFactory.DefaultName(), "FastDiscret");
}

//! Test factory name uniqueness - registering same factory twice should not duplicate
TEST_F(BRepMesh_DiscretAlgoFactoryTest, RegisterFactory_Uniqueness)
{
  const NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>& aFactories =
    BRepMesh_DiscretAlgoFactory::Factories();

  // Count factories with name "FastDiscret"
  int aCount = 0;
  for (NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>::Iterator anIter(aFactories);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->Name() == "FastDiscret")
    {
      ++aCount;
    }
  }

  EXPECT_EQ(aCount, 1) << "FastDiscret factory should be registered exactly once";
}
