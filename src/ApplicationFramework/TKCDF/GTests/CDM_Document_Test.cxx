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

#include <CDM_Document.hxx>
#include <CDM_Reference.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <gtest/gtest.h>

class CDM_DocumentTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myApp = new TDocStd_Application();
    myApp->NewDocument("BinOcaf", myDoc1);
    myApp->NewDocument("BinOcaf", myDoc2);
    myApp->NewDocument("BinOcaf", myDoc3);
  }

  occ::handle<TDocStd_Application> myApp;
  occ::handle<TDocStd_Document>    myDoc1;
  occ::handle<TDocStd_Document>    myDoc2;
  occ::handle<TDocStd_Document>    myDoc3;
};

TEST_F(CDM_DocumentTest, CreateReference_ReturnsUniqueId)
{
  int aRefId1 = myDoc1->CreateReference(myDoc2);
  int aRefId2 = myDoc1->CreateReference(myDoc3);
  EXPECT_NE(aRefId1, aRefId2);
  EXPECT_EQ(2, myDoc1->ToReferencesNumber());
}

TEST_F(CDM_DocumentTest, CreateReference_DuplicateReturnsSameId)
{
  int aRefId1 = myDoc1->CreateReference(myDoc2);
  int aRefId2 = myDoc1->CreateReference(myDoc2);
  EXPECT_EQ(aRefId1, aRefId2);
  EXPECT_EQ(1, myDoc1->ToReferencesNumber());
}

TEST_F(CDM_DocumentTest, RemoveReference_DecreasesCount)
{
  int aRefId1 = myDoc1->CreateReference(myDoc2);
  myDoc1->CreateReference(myDoc3);
  EXPECT_EQ(2, myDoc1->ToReferencesNumber());

  myDoc1->RemoveReference(aRefId1);
  EXPECT_EQ(1, myDoc1->ToReferencesNumber());
}

TEST_F(CDM_DocumentTest, RemoveAllReferences_ClearsAll)
{
  myDoc1->CreateReference(myDoc2);
  myDoc1->CreateReference(myDoc3);
  EXPECT_EQ(2, myDoc1->ToReferencesNumber());

  myDoc1->RemoveAllReferences();
  EXPECT_EQ(0, myDoc1->ToReferencesNumber());
}

TEST_F(CDM_DocumentTest, Reference_LookupById)
{
  int aRefId = myDoc1->CreateReference(myDoc2);

  occ::handle<CDM_Reference> aRef = myDoc1->Reference(aRefId);
  ASSERT_FALSE(aRef.IsNull());
  EXPECT_EQ(aRefId, aRef->ReferenceIdentifier());
}

TEST_F(CDM_DocumentTest, Reference_InvalidIdReturnsNull)
{
  occ::handle<CDM_Reference> aRef = myDoc1->Reference(9999);
  EXPECT_TRUE(aRef.IsNull());
}

TEST_F(CDM_DocumentTest, Document_ReturnsReferencedDoc)
{
  int aRefId = myDoc1->CreateReference(myDoc2);

  occ::handle<CDM_Document> aDoc = myDoc1->Document(aRefId);
  ASSERT_FALSE(aDoc.IsNull());
  EXPECT_EQ(myDoc2.get(), aDoc.get());
}

TEST_F(CDM_DocumentTest, Document_ZeroReturnsSelf)
{
  occ::handle<CDM_Document> aDoc = myDoc1->Document(0);
  ASSERT_FALSE(aDoc.IsNull());
  EXPECT_EQ(myDoc1.get(), aDoc.get());
}

TEST_F(CDM_DocumentTest, FromReferencesNumber_UpdatedOnCreate)
{
  EXPECT_EQ(0, myDoc2->FromReferencesNumber());

  myDoc1->CreateReference(myDoc2);
  EXPECT_EQ(1, myDoc2->FromReferencesNumber());

  myDoc3->CreateReference(myDoc2);
  EXPECT_EQ(2, myDoc2->FromReferencesNumber());
}

TEST_F(CDM_DocumentTest, ShallowReferences_FindsDirect)
{
  myDoc1->CreateReference(myDoc2);
  EXPECT_TRUE(myDoc1->ShallowReferences(myDoc2));
  EXPECT_FALSE(myDoc1->ShallowReferences(myDoc3));
  EXPECT_FALSE(myDoc2->ShallowReferences(myDoc1));
}

TEST_F(CDM_DocumentTest, DeepReferences_FindsTransitive)
{
  // Doc1 -> Doc2 -> Doc3
  myDoc1->CreateReference(myDoc2);
  myDoc2->CreateReference(myDoc3);

  EXPECT_TRUE(myDoc1->DeepReferences(myDoc3));
  EXPECT_TRUE(myDoc1->DeepReferences(myDoc2));
  EXPECT_FALSE(myDoc3->DeepReferences(myDoc1));
}

TEST_F(CDM_DocumentTest, DeepReferences_NoCycleInfiniteLoop)
{
  // Create a cycle: Doc1 -> Doc2 -> Doc3 -> Doc1
  myDoc1->CreateReference(myDoc2);
  myDoc2->CreateReference(myDoc3);
  myDoc3->CreateReference(myDoc1);

  // This should not hang or crash due to cycle detection
  occ::handle<TDocStd_Document> myDoc4;
  myApp->NewDocument("BinOcaf", myDoc4);
  EXPECT_FALSE(myDoc1->DeepReferences(myDoc4));
}

TEST_F(CDM_DocumentTest, Modifications_IncrementOnModify)
{
  int aMod1 = myDoc1->Modifications();
  myDoc1->Modify();
  int aMod2 = myDoc1->Modifications();
  EXPECT_EQ(aMod1 + 1, aMod2);
}

TEST_F(CDM_DocumentTest, IsModified_TrueForNewDocument)
{
  // A new unsaved document is always considered modified
  // (no storage version set yet).
  EXPECT_TRUE(myDoc1->IsModified());
}

TEST_F(CDM_DocumentTest, ReferenceIterator_IteratesAll)
{
  myDoc1->CreateReference(myDoc2);
  myDoc1->CreateReference(myDoc3);

  int aCount = 0;
  for (CDM_ReferenceIterator anIt(myDoc1); anIt.More(); anIt.Next())
  {
    aCount++;
    EXPECT_FALSE(anIt.Document().IsNull());
  }
  EXPECT_EQ(2, aCount);
}
