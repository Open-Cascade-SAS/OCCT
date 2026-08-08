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

#include <gtest/gtest.h>

#include <NCollection_List.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>

TEST(TDataStd_List_Test, CafBug_23465_ListInsertRemove)
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("XmlOcaf");
  const TDF_Label aLabel1 = aDocument->Main().FindChild(1, true);
  const TDF_Label aLabel2 = aDocument->Main().FindChild(2, true);
  const TDF_Label aLabel3 = aDocument->Main().FindChild(3, true);
  const TDF_Label aLabel4 = aDocument->Main().FindChild(4, true);
  const TDF_Label aLabel5 = aDocument->Main().FindChild(5, true);

  occ::handle<TDataStd_BooleanList> aBooleanList = TDataStd_BooleanList::Set(aLabel1);
  aBooleanList->Append(false);
  aBooleanList->Append(false);
  aBooleanList->Append(true);
  aBooleanList->Append(false);

  occ::handle<TDataStd_IntegerList> anIntegerList = TDataStd_IntegerList::Set(aLabel2);
  anIntegerList->Append(0);
  anIntegerList->Append(0);
  anIntegerList->Append(1);
  anIntegerList->Append(0);

  occ::handle<TDataStd_RealList> aRealList = TDataStd_RealList::Set(aLabel3);
  aRealList->Append(0.0);
  aRealList->Append(0.0);
  aRealList->Append(1.0);
  aRealList->Append(0.0);

  occ::handle<TDataStd_ExtStringList> anExtStringList = TDataStd_ExtStringList::Set(aLabel4);
  anExtStringList->Append(TCollection_ExtendedString("0"));
  anExtStringList->Append(TCollection_ExtendedString("0"));
  anExtStringList->Append(TCollection_ExtendedString("1"));
  anExtStringList->Append(TCollection_ExtendedString("0"));

  occ::handle<TDataStd_ReferenceList> aReferenceList = TDataStd_ReferenceList::Set(aLabel5);
  aReferenceList->Append(aLabel1);
  aReferenceList->Append(aLabel2);
  aReferenceList->Append(aLabel4);

  ASSERT_TRUE(aBooleanList->InsertBefore(3, true));
  ASSERT_TRUE(anIntegerList->InsertBeforeByIndex(3, 1));
  ASSERT_TRUE(aRealList->InsertBeforeByIndex(3, 1.0));
  ASSERT_TRUE(anExtStringList->InsertBefore(3, TCollection_ExtendedString("1")));
  ASSERT_TRUE(aReferenceList->InsertBefore(3, aLabel3));

  ASSERT_TRUE(aBooleanList->InsertAfter(4, true));
  ASSERT_TRUE(anIntegerList->InsertAfterByIndex(4, 1));
  ASSERT_TRUE(aRealList->InsertAfterByIndex(4, 1.0));
  ASSERT_TRUE(anExtStringList->InsertAfter(4, TCollection_ExtendedString("1")));
  ASSERT_TRUE(aReferenceList->InsertAfter(4, aLabel5));

  ASSERT_TRUE(aBooleanList->Remove(6));
  ASSERT_TRUE(anIntegerList->RemoveByIndex(6));
  ASSERT_TRUE(aRealList->RemoveByIndex(6));
  ASSERT_TRUE(anExtStringList->Remove(6));
  ASSERT_TRUE(aReferenceList->Remove(1));

  ASSERT_EQ(aBooleanList->Extent(), 5);
  const uint8_t anExpectedBooleanValues[5] = {0, 0, 1, 1, 1};
  int           anIndex                  = 0;
  for (NCollection_List<uint8_t>::Iterator anIterator(aBooleanList->List()); anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 5);
    EXPECT_EQ(anIterator.Value(), anExpectedBooleanValues[anIndex]);
  }
  EXPECT_EQ(anIndex, 5);

  ASSERT_EQ(anIntegerList->Extent(), 5);
  const int anExpectedIntegerValues[5] = {0, 0, 1, 1, 1};
  anIndex                              = 0;
  for (NCollection_List<int>::Iterator anIterator(anIntegerList->List()); anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 5);
    EXPECT_EQ(anIterator.Value(), anExpectedIntegerValues[anIndex]);
  }
  EXPECT_EQ(anIndex, 5);

  ASSERT_EQ(aRealList->Extent(), 5);
  anIndex = 0;
  for (NCollection_List<double>::Iterator anIterator(aRealList->List()); anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 5);
    EXPECT_DOUBLE_EQ(anIterator.Value(), double(anExpectedIntegerValues[anIndex]));
  }
  EXPECT_EQ(anIndex, 5);

  ASSERT_EQ(anExtStringList->Extent(), 5);
  const TCollection_ExtendedString anExpectedStringValues[5] = {
    TCollection_ExtendedString("0"),
    TCollection_ExtendedString("0"),
    TCollection_ExtendedString("1"),
    TCollection_ExtendedString("1"),
    TCollection_ExtendedString("1")};
  anIndex = 0;
  for (NCollection_List<TCollection_ExtendedString>::Iterator anIterator(anExtStringList->List());
       anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 5);
    EXPECT_TRUE(anIterator.Value().IsEqual(anExpectedStringValues[anIndex]));
  }
  EXPECT_EQ(anIndex, 5);

  ASSERT_EQ(aReferenceList->Extent(), 4);
  const TDF_Label anExpectedReferenceValues[4] = {aLabel2, aLabel3, aLabel4, aLabel5};
  anIndex                                       = 0;
  for (NCollection_List<TDF_Label>::Iterator anIterator(aReferenceList->List());
       anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 4);
    EXPECT_EQ(anIterator.Value(), anExpectedReferenceValues[anIndex]);
  }
  EXPECT_EQ(anIndex, 4);
}

// caf/bugs/bug23912: an extended-string array keeps each assigned value at its
// own index.
TEST(TDataStd_List_Test, CafBug_23912_ExtStringArrayValues)
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  occ::handle<TDataStd_ExtStringArray> anArray =
    TDataStd_ExtStringArray::Set(aDocument->Main(), 1, 5);
  ASSERT_FALSE(anArray.IsNull());

  const TCollection_ExtendedString anExpectedValues[5] = {
    TCollection_ExtendedString("A"),
    TCollection_ExtendedString("B"),
    TCollection_ExtendedString("C"),
    TCollection_ExtendedString("D"),
    TCollection_ExtendedString("E")};
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    anArray->SetValue(anIndex, anExpectedValues[anIndex - 1]);
  }
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    EXPECT_TRUE(anArray->Value(anIndex).IsEqual(anExpectedValues[anIndex - 1]));
  }
}
