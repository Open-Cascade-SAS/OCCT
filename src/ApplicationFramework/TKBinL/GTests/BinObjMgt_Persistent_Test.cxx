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

#include <BinObjMgt_Persistent.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

#include <gtest/gtest.h>

#include <climits>

namespace
{
//! Starts a record holding one int, the way a driver finds it after the header.
int startRecord(BinObjMgt_Persistent& theRecord, const int theValue)
{
  theRecord.Init();
  const int aStart = theRecord.Position();
  theRecord << theValue;
  theRecord.SetPosition(aStart);
  return aStart;
}
} // namespace

TEST(BinObjMgt_Persistent_Test, HasItemsComparesCountAgainstDataLeft)
{
  BinObjMgt_Persistent aRecord;
  const int            aStart = startRecord(aRecord, 7);

  // the record holds one int
  EXPECT_TRUE(aRecord.HasItems(0, 4));
  EXPECT_TRUE(aRecord.HasItems(1, 4));
  EXPECT_FALSE(aRecord.HasItems(2, 4));
  EXPECT_TRUE(aRecord.HasItems(4, 1));
  EXPECT_FALSE(aRecord.HasItems(5, 1));

  // a count whose product with the item size does not fit an int
  EXPECT_FALSE(aRecord.HasItems(1 << 30, 4));
  EXPECT_FALSE(aRecord.HasItems(1 << 29, 8));
  EXPECT_FALSE(aRecord.HasItems(INT_MAX, 1));
  EXPECT_FALSE(aRecord.HasItems(INT_MAX, INT_MAX));

  // not a count or not an item size
  EXPECT_FALSE(aRecord.HasItems(-1, 4));
  EXPECT_FALSE(aRecord.HasItems(1, 0));
  EXPECT_FALSE(aRecord.HasItems(1, -4));
  EXPECT_EQ(aRecord.Position(), aStart);
}

TEST(BinObjMgt_Persistent_Test, GetArrayRejectsLengthWhoseByteCountWraps)
{
  // each length times its item size is 2^32, which wraps to 0
  BinObjMgt_Persistent aRecord;
  startRecord(aRecord, 7);

  int aInts[1] = {0};
  aRecord.GetIntArray(aInts, 1 << 30);
  EXPECT_TRUE(aRecord.IsError());
  aRecord.SetPosition(aRecord.Position());

  double aReals[1] = {0.0};
  aRecord.GetRealArray(aReals, 1 << 29);
  EXPECT_TRUE(aRecord.IsError());
  aRecord.SetPosition(aRecord.Position());

  float aShortReals[1] = {0.0f};
  aRecord.GetShortRealArray(aShortReals, 1 << 30);
  EXPECT_TRUE(aRecord.IsError());
}

TEST(BinObjMgt_Persistent_Test, GetArrayStillReadsWhatTheRecordHolds)
{
  BinObjMgt_Persistent aRecord;
  aRecord.Init();
  const int aStart  = aRecord.Position();
  int       aPut[3] = {1, 2, 3};
  aRecord.PutIntArray(aPut, 3);
  aRecord.SetPosition(aStart);

  int aGot[3] = {0, 0, 0};
  aRecord.GetIntArray(aGot, 3);
  EXPECT_TRUE(aRecord.IsOK());
  EXPECT_EQ(aGot[0], 1);
  EXPECT_EQ(aGot[1], 2);
  EXPECT_EQ(aGot[2], 3);

  // one more than the record holds
  aRecord.SetPosition(aStart);
  int aMore[4] = {0, 0, 0, 0};
  aRecord.GetIntArray(aMore, 4);
  EXPECT_TRUE(aRecord.IsError());
}

TEST(BinObjMgt_Persistent_Test, GetByteArrayRejectsLengthPastTheEnd)
{
  // Position() + INT_MAX overflows int
  BinObjMgt_Persistent aRecord;
  startRecord(aRecord, 7);

  uint8_t aBytes[1] = {0};
  aRecord.GetByteArray(aBytes, INT_MAX);
  EXPECT_TRUE(aRecord.IsError());
  aRecord.SetPosition(aRecord.Position());
  aRecord.GetByteArray(aBytes, -1);
  EXPECT_TRUE(aRecord.IsError());
}

TEST(BinObjMgt_Persistent_Test, GetLabelRejectsTagCountThatWraps)
{
  // 2^30 tags times 4 bytes wraps to 0
  BinObjMgt_Persistent aRecord;
  startRecord(aRecord, 1 << 30);

  const occ::handle<TDF_Data> aData = new TDF_Data();
  TDF_Label                   aLabel;
  aRecord.GetLabel(aData, aLabel);
  EXPECT_TRUE(aRecord.IsError());
  EXPECT_TRUE(aLabel.IsNull());
}

TEST(BinObjMgt_Persistent_Test, GetLabelStillReadsATagList)
{
  BinObjMgt_Persistent aRecord;
  aRecord.Init();
  const int                   aStart = aRecord.Position();
  const occ::handle<TDF_Data> aData  = new TDF_Data();
  const TDF_Label             aChild = aData->Root().FindChild(3);
  aRecord << aChild;
  aRecord.SetPosition(aStart);

  TDF_Label aLabel;
  aRecord.GetLabel(aData, aLabel);
  EXPECT_TRUE(aRecord.IsOK());
  EXPECT_TRUE(aLabel.IsEqual(aChild));
}
