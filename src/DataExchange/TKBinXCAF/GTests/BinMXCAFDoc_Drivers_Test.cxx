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

#include <BinMXCAFDoc_DimTolDriver.hxx>
#include <BinMXCAFDoc_NoteBinDataDriver.hxx>
#include <BinMXCAFDoc_VisMaterialDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_NoteBinData.hxx>
#include <XCAFDoc_VisMaterial.hxx>

#include <gtest/gtest.h>

// An XCAF attribute declares the size of an array or a buffer in the file, and the driver must
// not size memory by it before it checks the record holds that many elements.

TEST(BinMXCAFDoc_Drivers_Test, DimTolDeclaringMoreValuesThanTheRecordHolds)
{
  BinObjMgt_Persistent       aRecord;
  BinObjMgt_RRelocationTable aTable;
  aRecord.Init();
  const int aStart = aRecord.Position();
  // kind, name, description, then the first and last index of the values
  aRecord << 0 << TCollection_AsciiString("") << TCollection_AsciiString("") << 1 << (1 << 26);
  aRecord.SetPosition(aStart);

  const occ::handle<XCAFDoc_DimTol> anAtt = new XCAFDoc_DimTol();
  EXPECT_FALSE(BinMXCAFDoc_DimTolDriver(new Message_Messenger()).Paste(aRecord, anAtt, aTable));
}

TEST(BinMXCAFDoc_Drivers_Test, NoteBinDataDeclaringMoreBytesThanTheRecordHolds)
{
  BinObjMgt_Persistent       aRecord;
  BinObjMgt_RRelocationTable aTable;
  aRecord.Init();
  const int aStart = aRecord.Position();
  // user name and time stamp of the note, then title, MIME type and the size of the data
  aRecord << TCollection_ExtendedString("user") << TCollection_ExtendedString("stamp")
          << TCollection_ExtendedString("title") << TCollection_AsciiString("text/plain")
          << (1 << 26);
  aRecord.SetPosition(aStart);

  const occ::handle<XCAFDoc_NoteBinData> anAtt = new XCAFDoc_NoteBinData();
  EXPECT_FALSE(
    BinMXCAFDoc_NoteBinDataDriver(new Message_Messenger()).Paste(aRecord, anAtt, aTable));
  EXPECT_EQ(anAtt->Size(), 0);
}

TEST(BinMXCAFDoc_Drivers_Test, VisMaterialDeclaringMoreTextureBytesThanTheRecordHolds)
{
  BinObjMgt_Persistent       aRecord;
  BinObjMgt_RRelocationTable aTable;
  aRecord.Init();
  const int aStart = aRecord.Position();
  // version, face culling, alpha mode and cut off, no PBR material, a common material
  aRecord.PutByte(1);
  aRecord.PutByte(0);
  aRecord.PutByte('0');
  aRecord.PutByte('A');
  aRecord.PutShortReal(0.5f);
  aRecord.PutBoolean(false);
  aRecord.PutBoolean(true);
  // ambient, diffuse, specular and emissive colors, shininess and transparency
  for (int anIter = 0; anIter < 14; ++anIter)
  {
    aRecord.PutShortReal(0.5f);
  }
  // the diffuse texture is a buffer of 2^26 bytes that the record does not hold
  aRecord.PutAsciiString("texture");
  aRecord.PutBoolean(true);
  aRecord.PutInteger(1 << 26);
  aRecord.SetPosition(aStart);

  const occ::handle<XCAFDoc_VisMaterial> anAtt = new XCAFDoc_VisMaterial();
  BinMXCAFDoc_VisMaterialDriver(new Message_Messenger()).Paste(aRecord, anAtt, aTable);
  EXPECT_TRUE(anAtt->CommonMaterial().DiffuseTexture.IsNull());
}

TEST(BinMXCAFDoc_Drivers_Test, ArraysStillRead)
{
  BinObjMgt_RRelocationTable aTable;

  BinObjMgt_Persistent aDimTolRecord;
  aDimTolRecord.Init();
  const int aDimTolStart = aDimTolRecord.Position();
  aDimTolRecord << 3 << TCollection_AsciiString("name") << TCollection_AsciiString("descr") << 1
                << 2 << 1.5 << 2.5;
  aDimTolRecord.SetPosition(aDimTolStart);
  const occ::handle<XCAFDoc_DimTol> aDimTol = new XCAFDoc_DimTol();
  ASSERT_TRUE(
    BinMXCAFDoc_DimTolDriver(new Message_Messenger()).Paste(aDimTolRecord, aDimTol, aTable));
  ASSERT_FALSE(aDimTol->GetVal().IsNull());
  EXPECT_EQ(aDimTol->GetVal()->Length(), 2);
  EXPECT_EQ(aDimTol->GetVal()->Value(2), 2.5);

  BinObjMgt_Persistent aNoteRecord;
  aNoteRecord.Init();
  const int aNoteStart = aNoteRecord.Position();
  uint8_t   aBytes[3]  = {1, 2, 3};
  aNoteRecord << TCollection_ExtendedString("user") << TCollection_ExtendedString("stamp")
              << TCollection_ExtendedString("title") << TCollection_AsciiString("text/plain") << 3;
  aNoteRecord.PutByteArray(aBytes, 3);
  aNoteRecord.SetPosition(aNoteStart);
  const occ::handle<XCAFDoc_NoteBinData> aNote = new XCAFDoc_NoteBinData();
  ASSERT_TRUE(
    BinMXCAFDoc_NoteBinDataDriver(new Message_Messenger()).Paste(aNoteRecord, aNote, aTable));
  EXPECT_EQ(aNote->Size(), 3);
}
