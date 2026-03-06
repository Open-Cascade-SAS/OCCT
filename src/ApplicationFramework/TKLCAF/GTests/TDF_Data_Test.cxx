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

#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <gtest/gtest.h>

class TDF_DataTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myApp = new TDocStd_Application();
    myApp->NewDocument("BinOcaf", myDoc);
    myDoc->SetUndoLimit(10);
  }

  occ::handle<TDocStd_Application> myApp;
  occ::handle<TDocStd_Document>    myDoc;
};

TEST_F(TDF_DataTest, Transaction_OpenCommit)
{
  TDF_Label aRoot = myDoc->Main();

  myDoc->NewCommand();
  TDF_Label                     aLabel = aRoot.FindChild(1);
  occ::handle<TDataStd_Integer> anInt  = new TDataStd_Integer();
  aLabel.AddAttribute(anInt);
  anInt->Set(42);
  EXPECT_TRUE(myDoc->CommitCommand());

  EXPECT_EQ(1, myDoc->GetAvailableUndos());
}

TEST_F(TDF_DataTest, Transaction_UndoRedo)
{
  TDF_Label aRoot = myDoc->Main();

  // Transaction 1: add attribute
  myDoc->NewCommand();
  TDF_Label                     aLabel = aRoot.FindChild(1);
  occ::handle<TDataStd_Integer> anInt  = new TDataStd_Integer();
  aLabel.AddAttribute(anInt);
  anInt->Set(42);
  myDoc->CommitCommand();

  // Verify attribute exists
  occ::handle<TDataStd_Integer> aFound;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound));
  EXPECT_EQ(42, aFound->Get());

  // Undo
  EXPECT_TRUE(myDoc->Undo());

  // Attribute should be gone after undo
  EXPECT_FALSE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound));

  // Redo
  EXPECT_TRUE(myDoc->Redo());

  // Attribute should be back
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound));
  EXPECT_EQ(42, aFound->Get());
}

TEST_F(TDF_DataTest, Transaction_ModifyAttribute)
{
  TDF_Label aRoot = myDoc->Main();

  // Transaction 1: create attribute
  myDoc->NewCommand();
  TDF_Label                     aLabel = aRoot.FindChild(1);
  occ::handle<TDataStd_Integer> anInt  = new TDataStd_Integer();
  aLabel.AddAttribute(anInt);
  anInt->Set(10);
  myDoc->CommitCommand();

  // Transaction 2: modify attribute
  myDoc->NewCommand();
  occ::handle<TDataStd_Integer> aFound;
  aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound);
  aFound->Set(20);
  myDoc->CommitCommand();

  // Verify modified value
  aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound);
  EXPECT_EQ(20, aFound->Get());

  // Undo should restore original value
  myDoc->Undo();
  aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound);
  EXPECT_EQ(10, aFound->Get());
}

TEST_F(TDF_DataTest, Transaction_AbortDiscardsChanges)
{
  TDF_Label aRoot = myDoc->Main();

  myDoc->NewCommand();
  TDF_Label                     aLabel = aRoot.FindChild(1);
  occ::handle<TDataStd_Integer> anInt  = new TDataStd_Integer();
  aLabel.AddAttribute(anInt);
  anInt->Set(42);
  myDoc->AbortCommand();

  // Attribute should not exist after abort
  occ::handle<TDataStd_Integer> aFound;
  EXPECT_FALSE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aFound));
}

TEST_F(TDF_DataTest, Transaction_MultipleLabels)
{
  TDF_Label aRoot = myDoc->Main();

  myDoc->NewCommand();
  TDF_Label aLabel1 = aRoot.FindChild(1);
  TDF_Label aLabel2 = aRoot.FindChild(2);

  occ::handle<TDataStd_Integer> anInt1 = new TDataStd_Integer();
  aLabel1.AddAttribute(anInt1);
  anInt1->Set(100);

  occ::handle<TDataStd_Real> aReal = new TDataStd_Real();
  aLabel2.AddAttribute(aReal);
  aReal->Set(3.14);

  myDoc->CommitCommand();

  // Verify both attributes exist
  occ::handle<TDataStd_Integer> aFoundInt;
  occ::handle<TDataStd_Real>    aFoundReal;
  ASSERT_TRUE(aLabel1.FindAttribute(TDataStd_Integer::GetID(), aFoundInt));
  ASSERT_TRUE(aLabel2.FindAttribute(TDataStd_Real::GetID(), aFoundReal));
  EXPECT_EQ(100, aFoundInt->Get());
  EXPECT_NEAR(3.14, aFoundReal->Get(), 1e-10);

  // Undo should remove both
  myDoc->Undo();
  EXPECT_FALSE(aLabel1.FindAttribute(TDataStd_Integer::GetID(), aFoundInt));
  EXPECT_FALSE(aLabel2.FindAttribute(TDataStd_Real::GetID(), aFoundReal));
}

TEST_F(TDF_DataTest, UndoLimit_LimitsStackSize)
{
  myDoc->SetUndoLimit(2);
  TDF_Label aRoot = myDoc->Main();

  // 3 transactions, but limit is 2
  for (int i = 0; i < 3; i++)
  {
    myDoc->NewCommand();
    TDF_Label                     aLabel = aRoot.FindChild(i + 1);
    occ::handle<TDataStd_Integer> anInt  = new TDataStd_Integer();
    aLabel.AddAttribute(anInt);
    anInt->Set(i);
    myDoc->CommitCommand();
  }

  EXPECT_EQ(2, myDoc->GetAvailableUndos());
}
