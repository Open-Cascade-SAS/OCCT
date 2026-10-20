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

#include <BinMDataStd_BooleanArrayDriver.hxx>
#include <BinMDataStd_BooleanListDriver.hxx>
#include <BinMDataStd_ByteArrayDriver.hxx>
#include <BinMDataStd_ExtStringArrayDriver.hxx>
#include <BinMDataStd_IntegerArrayDriver.hxx>
#include <BinMDataStd_IntegerListDriver.hxx>
#include <BinMDataStd_NamedDataDriver.hxx>
#include <BinMDataStd_RealArrayDriver.hxx>
#include <BinMDataStd_RealListDriver.hxx>
#include <BinMDataStd_ReferenceArrayDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Storage_HeaderData.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDF_Data.hxx>
#include <TDocStd_FormatVersion.hxx>

#include <gtest/gtest.h>

namespace
{
//! A binary record and the relocation table a driver reads it with.
struct Fixture
{
  Fixture()
  {
    occ::handle<Storage_HeaderData> aHeader = new Storage_HeaderData();
    aHeader->SetStorageVersion(static_cast<int>(TDocStd_FormatVersion_VERSION_3));
    myTable.SetHeaderData(aHeader);
    myRecord.Init();
    myStart = myRecord.Position();
  }

  //! Rewinds the record, to read what was put into it.
  void rewind() { myRecord.SetPosition(myStart); }

  BinObjMgt_Persistent       myRecord;
  BinObjMgt_RRelocationTable myTable;
  int                        myStart = 0;
};

//! A record declaring the span [theFirst, theLast] with no element after it.
void declareSpan(Fixture& theFixture, const int theFirst, const int theLast)
{
  theFixture.myRecord << theFirst << theLast;
  theFixture.rewind();
}

const occ::handle<Message_Messenger> THE_MESSENGER = new Message_Messenger();
} // namespace

// An array or list attribute declares its span in the file, and the driver must not size memory
// by it before it checks the record holds that many elements.

TEST(BinMDataStd_Drivers_Test, IntegerArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 26);
  const occ::handle<TDataStd_IntegerArray> anAtt = new TDataStd_IntegerArray();
  EXPECT_FALSE(BinMDataStd_IntegerArrayDriver(THE_MESSENGER)
                 .Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->Array().IsNull());
}

TEST(BinMDataStd_Drivers_Test, RealArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 25);
  const occ::handle<TDataStd_RealArray> anAtt = new TDataStd_RealArray();
  EXPECT_FALSE(
    BinMDataStd_RealArrayDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->Array().IsNull());
}

TEST(BinMDataStd_Drivers_Test, ExtStringArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 22);
  const occ::handle<TDataStd_ExtStringArray> anAtt = new TDataStd_ExtStringArray();
  EXPECT_FALSE(BinMDataStd_ExtStringArrayDriver(THE_MESSENGER)
                 .Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->Array().IsNull());
}

TEST(BinMDataStd_Drivers_Test, ReferenceArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 24);
  const occ::handle<TDataStd_ReferenceArray> anAtt = new TDataStd_ReferenceArray();
  EXPECT_FALSE(BinMDataStd_ReferenceArrayDriver(THE_MESSENGER)
                 .Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->InternalArray().IsNull());
}

TEST(BinMDataStd_Drivers_Test, ByteArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 24);
  const occ::handle<TDataStd_ByteArray> anAtt = new TDataStd_ByteArray();
  EXPECT_FALSE(
    BinMDataStd_ByteArrayDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->InternalArray().IsNull());
}

TEST(BinMDataStd_Drivers_Test, BooleanArrayDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 27);
  const occ::handle<TDataStd_BooleanArray> anAtt = new TDataStd_BooleanArray();
  EXPECT_FALSE(BinMDataStd_BooleanArrayDriver(THE_MESSENGER)
                 .Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_TRUE(anAtt->InternalArray().IsNull());
}

TEST(BinMDataStd_Drivers_Test, IntegerListDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 20);
  const occ::handle<TDataStd_IntegerList> anAtt = new TDataStd_IntegerList();
  EXPECT_FALSE(
    BinMDataStd_IntegerListDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_EQ(anAtt->Extent(), 0);
}

TEST(BinMDataStd_Drivers_Test, RealListDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 20);
  const occ::handle<TDataStd_RealList> anAtt = new TDataStd_RealList();
  EXPECT_FALSE(
    BinMDataStd_RealListDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_EQ(anAtt->Extent(), 0);
}

TEST(BinMDataStd_Drivers_Test, BooleanListDeclaringMoreThanTheRecordHolds)
{
  Fixture aFixture;
  declareSpan(aFixture, 1, 1 << 20);
  const occ::handle<TDataStd_BooleanList> anAtt = new TDataStd_BooleanList();
  EXPECT_FALSE(
    BinMDataStd_BooleanListDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_EQ(anAtt->Extent(), 0);
}

TEST(BinMDataStd_Drivers_Test, NamedDataArrayDeclaringMoreThanTheRecordHolds)
{
  // empty integer, real, string and byte maps, then one integer array of 2^26 elements
  Fixture aFixture;
  for (int aMap = 0; aMap < 4; ++aMap)
  {
    aFixture.myRecord << 0 << 0;
  }
  aFixture.myRecord << 1 << 1 << TCollection_ExtendedString("key") << 1 << (1 << 26);
  aFixture.rewind();

  const occ::handle<TDataStd_NamedData> anAtt = new TDataStd_NamedData();
  EXPECT_FALSE(
    BinMDataStd_NamedDataDriver(THE_MESSENGER).Paste(aFixture.myRecord, anAtt, aFixture.myTable));
  EXPECT_FALSE(anAtt->HasArraysOfIntegers());
}

TEST(BinMDataStd_Drivers_Test, ArraysStillRoundTrip)
{
  NCollection_IndexedMap<occ::handle<Standard_Transient>> aStoreTable;
  const occ::handle<TDF_Data>                             aData = new TDF_Data();

  Fixture                                  aIntegerFixture;
  const occ::handle<TDataStd_IntegerArray> anIntegers =
    TDataStd_IntegerArray::Set(aData->Root().FindChild(1), 2, 4);
  anIntegers->SetValue(2, 20);
  anIntegers->SetValue(3, 30);
  anIntegers->SetValue(4, 40);
  const BinMDataStd_IntegerArrayDriver anIntegerDriver(THE_MESSENGER);
  anIntegerDriver.Paste(anIntegers, aIntegerFixture.myRecord, aStoreTable);
  aIntegerFixture.rewind();
  const occ::handle<TDataStd_IntegerArray> anIntegersBack = new TDataStd_IntegerArray();
  ASSERT_TRUE(
    anIntegerDriver.Paste(aIntegerFixture.myRecord, anIntegersBack, aIntegerFixture.myTable));
  ASSERT_FALSE(anIntegersBack->Array().IsNull());
  EXPECT_EQ(anIntegersBack->Lower(), 2);
  EXPECT_EQ(anIntegersBack->Upper(), 4);
  EXPECT_EQ(anIntegersBack->Value(3), 30);

  Fixture                               aBytesFixture;
  const occ::handle<TDataStd_ByteArray> aBytes =
    TDataStd_ByteArray::Set(aData->Root().FindChild(2), 1, 3);
  aBytes->SetValue(1, 7);
  aBytes->SetValue(3, 9);
  const BinMDataStd_ByteArrayDriver aByteDriver(THE_MESSENGER);
  aByteDriver.Paste(aBytes, aBytesFixture.myRecord, aStoreTable);
  aBytesFixture.rewind();
  const occ::handle<TDataStd_ByteArray> aBytesBack = new TDataStd_ByteArray();
  ASSERT_TRUE(aByteDriver.Paste(aBytesFixture.myRecord, aBytesBack, aBytesFixture.myTable));
  EXPECT_EQ(aBytesBack->Value(1), 7);
  EXPECT_EQ(aBytesBack->Value(3), 9);

  Fixture                                  aBooleansFixture;
  const occ::handle<TDataStd_BooleanArray> aBooleans =
    TDataStd_BooleanArray::Set(aData->Root().FindChild(3), 1, 10);
  aBooleans->SetValue(2, true);
  aBooleans->SetValue(9, true);
  const BinMDataStd_BooleanArrayDriver aBooleanDriver(THE_MESSENGER);
  aBooleanDriver.Paste(aBooleans, aBooleansFixture.myRecord, aStoreTable);
  aBooleansFixture.rewind();
  const occ::handle<TDataStd_BooleanArray> aBooleansBack = new TDataStd_BooleanArray();
  ASSERT_TRUE(
    aBooleanDriver.Paste(aBooleansFixture.myRecord, aBooleansBack, aBooleansFixture.myTable));
  EXPECT_TRUE(aBooleansBack->Value(2));
  EXPECT_FALSE(aBooleansBack->Value(3));
  EXPECT_TRUE(aBooleansBack->Value(9));
}
