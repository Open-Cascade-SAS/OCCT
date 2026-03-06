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

#include <TDataStd_ByteArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <gtest/gtest.h>

class TDataStd_DeltaArrayTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myApp = new TDocStd_Application();
    myApp->NewDocument("BinOcaf", myDoc);
    myDoc->SetUndoLimit(10);
    myLabel = myDoc->Main().FindChild(1);
  }

  occ::handle<TDocStd_Application> myApp;
  occ::handle<TDocStd_Document>    myDoc;
  TDF_Label                        myLabel;
};

TEST_F(TDataStd_DeltaArrayTest, RealArray_ModifyAndUndo)
{
  // Create array
  myDoc->NewCommand();
  occ::handle<TDataStd_RealArray> anArr = TDataStd_RealArray::Set(myLabel, 1, 3);
  anArr->SetValue(1, 1.0);
  anArr->SetValue(2, 2.0);
  anArr->SetValue(3, 3.0);
  myDoc->CommitCommand();

  // Modify array
  myDoc->NewCommand();
  anArr->SetValue(2, 20.0);
  anArr->SetValue(3, 30.0);
  myDoc->CommitCommand();

  // Verify modification
  EXPECT_NEAR(1.0, anArr->Value(1), 1e-10);
  EXPECT_NEAR(20.0, anArr->Value(2), 1e-10);
  EXPECT_NEAR(30.0, anArr->Value(3), 1e-10);

  // Undo modification
  myDoc->Undo();

  occ::handle<TDataStd_RealArray> aFound;
  ASSERT_TRUE(myLabel.FindAttribute(TDataStd_RealArray::GetID(), aFound));
  EXPECT_NEAR(1.0, aFound->Value(1), 1e-10);
  EXPECT_NEAR(2.0, aFound->Value(2), 1e-10);
  EXPECT_NEAR(3.0, aFound->Value(3), 1e-10);
}

TEST_F(TDataStd_DeltaArrayTest, IntegerArray_ModifyAndUndo)
{
  myDoc->NewCommand();
  occ::handle<TDataStd_IntegerArray> anArr = TDataStd_IntegerArray::Set(myLabel, 1, 3);
  anArr->SetValue(1, 10);
  anArr->SetValue(2, 20);
  anArr->SetValue(3, 30);
  myDoc->CommitCommand();

  myDoc->NewCommand();
  anArr->SetValue(1, 100);
  myDoc->CommitCommand();

  EXPECT_EQ(100, anArr->Value(1));

  myDoc->Undo();

  occ::handle<TDataStd_IntegerArray> aFound;
  ASSERT_TRUE(myLabel.FindAttribute(TDataStd_IntegerArray::GetID(), aFound));
  EXPECT_EQ(10, aFound->Value(1));
  EXPECT_EQ(20, aFound->Value(2));
  EXPECT_EQ(30, aFound->Value(3));
}

TEST_F(TDataStd_DeltaArrayTest, ByteArray_ModifyAndUndo)
{
  myDoc->NewCommand();
  occ::handle<TDataStd_ByteArray> anArr = TDataStd_ByteArray::Set(myLabel, 1, 3);
  anArr->SetValue(1, 0x01);
  anArr->SetValue(2, 0x02);
  anArr->SetValue(3, 0x03);
  myDoc->CommitCommand();

  myDoc->NewCommand();
  anArr->SetValue(2, 0xFF);
  myDoc->CommitCommand();

  EXPECT_EQ(0xFF, anArr->Value(2));

  myDoc->Undo();

  occ::handle<TDataStd_ByteArray> aFound;
  ASSERT_TRUE(myLabel.FindAttribute(TDataStd_ByteArray::GetID(), aFound));
  EXPECT_EQ(0x01, aFound->Value(1));
  EXPECT_EQ(0x02, aFound->Value(2));
  EXPECT_EQ(0x03, aFound->Value(3));
}

TEST_F(TDataStd_DeltaArrayTest, ExtStringArray_ModifyAndUndo)
{
  myDoc->NewCommand();
  occ::handle<TDataStd_ExtStringArray> anArr = TDataStd_ExtStringArray::Set(myLabel, 1, 2);
  anArr->SetValue(1, "Hello");
  anArr->SetValue(2, "World");
  myDoc->CommitCommand();

  myDoc->NewCommand();
  anArr->SetValue(1, "Changed");
  myDoc->CommitCommand();

  EXPECT_TRUE(anArr->Value(1).IsEqual("Changed"));

  myDoc->Undo();

  occ::handle<TDataStd_ExtStringArray> aFound;
  ASSERT_TRUE(myLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aFound));
  EXPECT_TRUE(aFound->Value(1).IsEqual("Hello"));
  EXPECT_TRUE(aFound->Value(2).IsEqual("World"));
}

TEST_F(TDataStd_DeltaArrayTest, RealArray_ResizeAndUndo)
{
  // Create array with 3 elements
  myDoc->NewCommand();
  occ::handle<TDataStd_RealArray> anArr = TDataStd_RealArray::Set(myLabel, 1, 3);
  anArr->SetValue(1, 1.0);
  anArr->SetValue(2, 2.0);
  anArr->SetValue(3, 3.0);
  myDoc->CommitCommand();

  // Resize array to 5 elements
  myDoc->NewCommand();
  anArr->Init(1, 5);
  anArr->SetValue(1, 1.0);
  anArr->SetValue(2, 2.0);
  anArr->SetValue(3, 3.0);
  anArr->SetValue(4, 4.0);
  anArr->SetValue(5, 5.0);
  myDoc->CommitCommand();

  EXPECT_EQ(5, anArr->Upper());

  // Undo should restore 3-element array
  myDoc->Undo();

  occ::handle<TDataStd_RealArray> aFound;
  ASSERT_TRUE(myLabel.FindAttribute(TDataStd_RealArray::GetID(), aFound));
  EXPECT_EQ(3, aFound->Upper());
  EXPECT_NEAR(1.0, aFound->Value(1), 1e-10);
  EXPECT_NEAR(2.0, aFound->Value(2), 1e-10);
  EXPECT_NEAR(3.0, aFound->Value(3), 1e-10);
}
