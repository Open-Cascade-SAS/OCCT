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

#include <TDataStd_AsciiString.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDataStd_UAttribute.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
static occ::handle<TDocStd_Document> NewDocument()
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  aDocument->SetUndoLimit(10);
  return aDocument;
}

static TDF_Label NewAttributeLabel(const occ::handle<TDocStd_Document>& theDocument)
{
  return theDocument->Main().FindChild(2, true);
}

static void ExpectRealArray(const occ::handle<TDataStd_RealArray>& theArray,
                            const double                           theFirst,
                            const double                           theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_DOUBLE_EQ(theArray->Value(1), theFirst);
  EXPECT_DOUBLE_EQ(theArray->Value(2), theSecond);
}

static void ExpectIntegerArray(const occ::handle<TDataStd_IntegerArray>& theArray,
                               const int                                 theFirst,
                               const int                                 theSecond)
{
  ASSERT_FALSE(theArray.IsNull());
  EXPECT_EQ(theArray->Lower(), 1);
  EXPECT_EQ(theArray->Upper(), 2);
  EXPECT_EQ(theArray->Length(), 2);
  EXPECT_EQ(theArray->Value(1), theFirst);
  EXPECT_EQ(theArray->Value(2), theSecond);
}

} // namespace

TEST(TDataStd_Attribute_Test, OCC29371_AttributeGUIDsNotNull)
{
  // Create document and label
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDoc;
  anApp->NewDocument("BinOcaf", aDoc);
  TDF_Label aLab = aDoc->Main();

  // Null GUID for comparison
  Standard_GUID aNullGuid("00000000-0000-0000-0000-000000000000");

  // Test each TDataStd attribute type has non-null GUID
  occ::handle<TDataStd_AsciiString> aStrAtt = new TDataStd_AsciiString();
  aLab.AddAttribute(aStrAtt);
  EXPECT_NE(aNullGuid, aStrAtt->ID());

  occ::handle<TDataStd_BooleanArray> aBArAtt = new TDataStd_BooleanArray();
  aLab.AddAttribute(aBArAtt);
  EXPECT_NE(aNullGuid, aBArAtt->ID());

  occ::handle<TDataStd_BooleanList> aBListAtt = new TDataStd_BooleanList();
  aLab.AddAttribute(aBListAtt);
  EXPECT_NE(aNullGuid, aBListAtt->ID());

  occ::handle<TDataStd_ByteArray> aByteArAtt = new TDataStd_ByteArray();
  aLab.AddAttribute(aByteArAtt);
  EXPECT_NE(aNullGuid, aByteArAtt->ID());

  occ::handle<TDataStd_ExtStringArray> anExtStrArAtt = new TDataStd_ExtStringArray();
  aLab.AddAttribute(anExtStrArAtt);
  EXPECT_NE(aNullGuid, anExtStrArAtt->ID());

  occ::handle<TDataStd_ExtStringList> anExtStrListAtt = new TDataStd_ExtStringList();
  aLab.AddAttribute(anExtStrListAtt);
  EXPECT_NE(aNullGuid, anExtStrListAtt->ID());

  occ::handle<TDataStd_Integer> anIntAtt = new TDataStd_Integer();
  aLab.AddAttribute(anIntAtt);
  EXPECT_NE(aNullGuid, anIntAtt->ID());

  occ::handle<TDataStd_IntegerArray> anIntArrAtt = new TDataStd_IntegerArray();
  aLab.AddAttribute(anIntArrAtt);
  EXPECT_NE(aNullGuid, anIntArrAtt->ID());

  occ::handle<TDataStd_IntegerList> anIntListAtt = new TDataStd_IntegerList();
  aLab.AddAttribute(anIntListAtt);
  EXPECT_NE(aNullGuid, anIntListAtt->ID());

  occ::handle<TDataStd_Name> aNameAtt = new TDataStd_Name();
  aLab.AddAttribute(aNameAtt);
  EXPECT_NE(aNullGuid, aNameAtt->ID());

  occ::handle<TDataStd_Real> aRealAtt = new TDataStd_Real();
  aLab.AddAttribute(aRealAtt);
  EXPECT_NE(aNullGuid, aRealAtt->ID());

  occ::handle<TDataStd_RealArray> aRealArrAtt = new TDataStd_RealArray();
  aLab.AddAttribute(aRealArrAtt);
  EXPECT_NE(aNullGuid, aRealArrAtt->ID());

  occ::handle<TDataStd_RealList> aRealListAtt = new TDataStd_RealList();
  aLab.AddAttribute(aRealListAtt);
  EXPECT_NE(aNullGuid, aRealListAtt->ID());

  occ::handle<TDataStd_ReferenceArray> aRefArrAtt = new TDataStd_ReferenceArray();
  aLab.AddAttribute(aRefArrAtt);
  EXPECT_NE(aNullGuid, aRefArrAtt->ID());

  occ::handle<TDataStd_ReferenceList> aRefListAtt = new TDataStd_ReferenceList();
  aLab.AddAttribute(aRefListAtt);
  EXPECT_NE(aNullGuid, aRefListAtt->ID());

  anApp->Close(aDoc);
}

// caf/basic/A1: setting and retrieving the default integer attribute.
TEST(TDataStd_Attribute_Test, CafBasic_A1_IntegerSetGet)
{
  occ::handle<TDocStd_Document> aDoc      = NewDocument();
  const TDF_Label               aLabel    = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Integer> anInteger = TDataStd_Integer::Set(aLabel, 100);

  ASSERT_FALSE(anInteger.IsNull());
  EXPECT_EQ(anInteger->Get(), 100);
}

// caf/basic/A2: undoing a replacement restores the previous integer value.
TEST(TDataStd_Attribute_Test, CafBasic_A2_IntegerUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  TDF_Label                     aLabel = NewAttributeLabel(aDoc);
  TDataStd_Integer::Set(aLabel, 100);
  aDoc->NewCommand();
  TDataStd_Integer::Set(aLabel, 200);
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_Integer> anInteger;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), anInteger));
  EXPECT_EQ(anInteger->Get(), 100);
}

// caf/basic/A4-A5: two user GUIDs identify independent integer attributes.
TEST(TDataStd_Attribute_Test, CafBasic_A4_IntegerUserGuids)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94542-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_Integer::Set(aLabel, aGuid1, 100);
  TDataStd_Integer::Set(aLabel, aGuid2, 200);

  occ::handle<TDataStd_Integer> anInteger1;
  occ::handle<TDataStd_Integer> anInteger2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, anInteger1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, anInteger2));
  EXPECT_EQ(anInteger1->Get(), 100);
  EXPECT_EQ(anInteger2->Get(), 200);
}

TEST(TDataStd_Attribute_Test, CafBasic_A5_IntegerUserGuidUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94542-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_Integer::Set(aLabel, aGuid1, 100);
  TDataStd_Integer::Set(aLabel, aGuid2, 200);
  aDoc->NewCommand();
  TDataStd_Integer::Set(aLabel, aGuid1, 199);
  TDataStd_Integer::Set(aLabel, aGuid2, 299);
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_Integer> anInteger1;
  occ::handle<TDataStd_Integer> anInteger2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, anInteger1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, anInteger2));
  EXPECT_EQ(anInteger1->Get(), 100);
  EXPECT_EQ(anInteger2->Get(), 200);
}

// caf/basic/B1-B2: scalar real values support replacement and undo.
TEST(TDataStd_Attribute_Test, CafBasic_B1_RealSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Real>    aReal  = TDataStd_Real::Set(aLabel, 100.0);

  ASSERT_FALSE(aReal.IsNull());
  EXPECT_DOUBLE_EQ(aReal->Get(), 100.0);
}

TEST(TDataStd_Attribute_Test, CafBasic_B2_RealUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  TDataStd_Real::Set(aLabel, 100.0);
  aDoc->NewCommand();
  TDataStd_Real::Set(aLabel, 200.0);
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_Real> aReal;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 100.0);
}

TEST(TDataStd_Attribute_Test, CafBasic_B4_RealUserGuids)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94552-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_Real::Set(aLabel, aGuid1, 100.11);
  TDataStd_Real::Set(aLabel, aGuid2, 200.11);

  occ::handle<TDataStd_Real> aReal1;
  occ::handle<TDataStd_Real> aReal2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, aReal1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, aReal2));
  EXPECT_DOUBLE_EQ(aReal1->Get(), 100.11);
  EXPECT_DOUBLE_EQ(aReal2->Get(), 200.11);
}

TEST(TDataStd_Attribute_Test, CafBasic_B5_RealUserGuidUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94552-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_Real::Set(aLabel, aGuid1, 100.11);
  TDataStd_Real::Set(aLabel, aGuid2, 200.11);
  aDoc->NewCommand();
  TDataStd_Real::Set(aLabel, aGuid1, 199.11);
  TDataStd_Real::Set(aLabel, aGuid2, 299.11);
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_Real> aReal1;
  occ::handle<TDataStd_Real> aReal2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, aReal1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, aReal2));
  EXPECT_DOUBLE_EQ(aReal1->Get(), 100.11);
  EXPECT_DOUBLE_EQ(aReal2->Get(), 200.11);
}

// caf/basic/C1-C2: real arrays retain bounds, values, and undo state.
TEST(TDataStd_Attribute_Test, CafBasic_C1_RealArraySetGet)
{
  occ::handle<TDocStd_Document>   aDoc    = NewDocument();
  const TDF_Label                 aLabel  = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);

  ExpectRealArray(anArray, 3.0, 4.0);
}

TEST(TDataStd_Attribute_Test, CafBasic_C2_RealArrayUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  aDoc->NewCommand();
  anArray->SetValue(1, 33.0);
  anArray->SetValue(2, 44.0);
  aDoc->NewCommand();
  aDoc->Undo();

  ExpectRealArray(anArray, 3.0, 4.0);
}

TEST(TDataStd_Attribute_Test, CafBasic_C4_RealArrayUserGuid)
{
  occ::handle<TDocStd_Document>   aDoc   = NewDocument();
  const TDF_Label                 aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID             aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  occ::handle<TDataStd_RealArray> aGuidArray = TDataStd_RealArray::Set(aLabel, aGuid, 1, 2);
  aGuidArray->SetValue(1, 3.0);
  aGuidArray->SetValue(2, 4.0);

  ExpectRealArray(anArray, 3.0, 4.0);
  ExpectRealArray(aGuidArray, 3.0, 4.0);
  EXPECT_EQ(aGuidArray->ID(), aGuid);
}

// caf/basic/D1-D2: integer arrays mirror real-array behavior.
TEST(TDataStd_Attribute_Test, CafBasic_D1_IntegerArraySetGet)
{
  occ::handle<TDocStd_Document>      aDoc    = NewDocument();
  const TDF_Label                    aLabel  = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);

  ExpectIntegerArray(anArray, 3, 4);
}

TEST(TDataStd_Attribute_Test, CafBasic_D2_IntegerArrayUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  aDoc->NewCommand();
  anArray->SetValue(1, 33);
  anArray->SetValue(2, 44);
  aDoc->NewCommand();
  aDoc->Undo();

  ExpectIntegerArray(anArray, 3, 4);
}

TEST(TDataStd_Attribute_Test, CafBasic_D4_IntegerArrayDefaultGuid)
{
  occ::handle<TDocStd_Document>      aDoc    = NewDocument();
  const TDF_Label                    aLabel  = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);

  EXPECT_EQ(anArray->ID(), TDataStd_IntegerArray::GetID());
  ExpectIntegerArray(anArray, 3, 4);
}

TEST(TDataStd_Attribute_Test, CafBasic_D5_IntegerArrayUserGuid)
{
  occ::handle<TDocStd_Document>      aDoc   = NewDocument();
  const TDF_Label                    aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID                aGuid("12e94511-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  occ::handle<TDataStd_IntegerArray> aGuidArray = TDataStd_IntegerArray::Set(aLabel, aGuid, 1, 2);
  aGuidArray->SetValue(1, 3);
  aGuidArray->SetValue(2, 4);

  ExpectIntegerArray(anArray, 3, 4);
  ExpectIntegerArray(aGuidArray, 3, 4);
  EXPECT_EQ(aGuidArray->ID(), aGuid);
}

// caf/basic/E1-E2: names support setting and undoing a replacement.
TEST(TDataStd_Attribute_Test, CafBasic_E1_NameSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Name>    aName =
    TDataStd_Name::Set(aLabel, TCollection_ExtendedString("New Attribute"));

  ASSERT_FALSE(aName.IsNull());
  EXPECT_TRUE(aName->Get().IsEqual(TCollection_ExtendedString("New Attribute")));
}

TEST(TDataStd_Attribute_Test, CafBasic_E2_NameUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Name> aName =
    TDataStd_Name::Set(aLabel, TCollection_ExtendedString("New Attribute"));
  aDoc->NewCommand();
  aName->Set(TCollection_ExtendedString("Old Attribute"));
  aDoc->NewCommand();
  aDoc->Undo();

  EXPECT_TRUE(aName->Get().IsEqual(TCollection_ExtendedString("New Attribute")));
}

TEST(TDataStd_Attribute_Test, CafBasic_E4_NameUserGuids)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94562-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_Name>    aName1 =
    TDataStd_Name::Set(aLabel, aGuid1, TCollection_ExtendedString("New Attribute1"));
  occ::handle<TDataStd_Name> aName2 =
    TDataStd_Name::Set(aLabel, aGuid2, TCollection_ExtendedString("New Attribute2"));

  ASSERT_FALSE(aName1.IsNull());
  ASSERT_FALSE(aName2.IsNull());
  EXPECT_TRUE(aName1->Get().IsEqual(TCollection_ExtendedString("New Attribute1")));
  EXPECT_TRUE(aName2->Get().IsEqual(TCollection_ExtendedString("New Attribute2")));
}

TEST(TDataStd_Attribute_Test, CafBasic_E5_NameUserGuidUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94562-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_Name> aName1 =
    TDataStd_Name::Set(aLabel, aGuid1, TCollection_ExtendedString("New Attribute1"));
  occ::handle<TDataStd_Name> aName2 =
    TDataStd_Name::Set(aLabel, aGuid2, TCollection_ExtendedString("New Attribute2"));
  aDoc->NewCommand();
  aName1->Set(TCollection_ExtendedString("Modified Attribute1"));
  aName2->Set(TCollection_ExtendedString("Modified Attribute2"));
  aDoc->NewCommand();
  aDoc->Undo();

  EXPECT_TRUE(aName1->Get().IsEqual(TCollection_ExtendedString("New Attribute1")));
  EXPECT_TRUE(aName2->Get().IsEqual(TCollection_ExtendedString("New Attribute2")));
}

// caf/basic/F1-F2: comments use the same generic extended-string behavior.
TEST(TDataStd_Attribute_Test, CafBasic_F1_CommentSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Comment> aComment =
    TDataStd_Comment::Set(aLabel, TCollection_ExtendedString("New Attribute"));

  ASSERT_FALSE(aComment.IsNull());
  EXPECT_TRUE(aComment->Get().IsEqual(TCollection_ExtendedString("New Attribute")));
}

TEST(TDataStd_Attribute_Test, CafBasic_F2_CommentUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_Comment> aComment =
    TDataStd_Comment::Set(aLabel, TCollection_ExtendedString("New Attribute"));
  aDoc->NewCommand();
  aComment->Set(TCollection_ExtendedString("Old Attribute"));
  aDoc->NewCommand();
  aDoc->Undo();

  EXPECT_TRUE(aComment->Get().IsEqual(TCollection_ExtendedString("New Attribute")));
}

// caf/basic/J1-J5: UAttributes are identified by their local GUID.
TEST(TDataStd_Attribute_Test, CafBasic_J1_UAttributeSetGet)
{
  occ::handle<TDocStd_Document>    aDoc   = NewDocument();
  const TDF_Label                  aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID              aGuid("c73bd075-22ee-11d2-acde-080009dc4422");
  occ::handle<TDataStd_UAttribute> anAttribute = TDataStd_UAttribute::Set(aLabel, aGuid);

  ASSERT_FALSE(anAttribute.IsNull());
  EXPECT_EQ(anAttribute->ID(), aGuid);
}

TEST(TDataStd_Attribute_Test, CafBasic_J2_UAttributeUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("c73bd075-22ee-11d2-acde-080009dc4422");
  const Standard_GUID           aGuid2("425a8e02-8934-11d2-b3f0-0060b0f1d69e");
  TDataStd_UAttribute::Set(aLabel, aGuid1);
  aDoc->NewCommand();
  aLabel.ForgetAttribute(aGuid1);
  TDataStd_UAttribute::Set(aLabel, aGuid2);
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_UAttribute> anAttribute;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, anAttribute));
  EXPECT_EQ(anAttribute->ID(), aGuid1);
}

TEST(TDataStd_Attribute_Test, CafBasic_J4_UAttributeMultipleGuids)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("c73bd075-22ee-11d2-acde-080009dc4422");
  const Standard_GUID           aGuid2("c73bd076-22ee-11d2-acde-080009dc4422");
  TDataStd_UAttribute::Set(aLabel, aGuid1);
  TDataStd_UAttribute::Set(aLabel, aGuid2);

  EXPECT_TRUE(aLabel.IsAttribute(aGuid1));
  EXPECT_TRUE(aLabel.IsAttribute(aGuid2));
}

TEST(TDataStd_Attribute_Test, CafBasic_J5_UAttributeCopy)
{
  occ::handle<TDocStd_Document> aDoc         = NewDocument();
  const TDF_Label               aSourceLabel = NewAttributeLabel(aDoc);
  const TDF_Label               aTargetLabel = aDoc->Main().FindChild(3, true);
  const Standard_GUID           aGuid1("c73bd075-22ee-11d2-acde-080009dc4422");
  const Standard_GUID           aGuid2("c73bd076-22ee-11d2-acde-080009dc4422");
  TDataStd_UAttribute::Set(aSourceLabel, aGuid1);
  TDataStd_UAttribute::Set(aSourceLabel, aGuid2);

  occ::handle<TDataStd_UAttribute> aSource1;
  occ::handle<TDataStd_UAttribute> aSource2;
  ASSERT_TRUE(aSourceLabel.FindAttribute(aGuid1, aSource1));
  ASSERT_TRUE(aSourceLabel.FindAttribute(aGuid2, aSource2));
  aTargetLabel.AddAttribute(aSource1->NewEmpty());
  aTargetLabel.AddAttribute(aSource2->NewEmpty());
  EXPECT_TRUE(aTargetLabel.IsAttribute(aGuid1));
  EXPECT_TRUE(aTargetLabel.IsAttribute(aGuid2));
}

// caf/basic/L1-L2: references store and restore target labels.
TEST(TDataStd_Attribute_Test, CafBasic_L1_ReferenceSetGet)
{
  occ::handle<TDocStd_Document> aDoc            = NewDocument();
  const TDF_Label               aTarget         = aDoc->Main().FindChild(3, true);
  const TDF_Label               aReferenceLabel = aDoc->Main().FindChild(5, true);
  occ::handle<TDF_Reference>    aReference      = TDF_Reference::Set(aReferenceLabel, aTarget);

  ASSERT_FALSE(aReference.IsNull());
  EXPECT_EQ(aReference->Get(), aTarget);
}

TEST(TDataStd_Attribute_Test, CafBasic_L2_ReferenceUndo)
{
  occ::handle<TDocStd_Document> aDoc            = NewDocument();
  const TDF_Label               aTarget1        = aDoc->Main().FindChild(3, true);
  const TDF_Label               aTarget2        = aDoc->Main().FindChild(4, true);
  const TDF_Label               aReferenceLabel = aDoc->Main().FindChild(5, true);
  occ::handle<TDF_Reference> aReference = TDF_Reference::Set(aReferenceLabel, aTarget1);
  aDoc->NewCommand();
  aReference->Set(aTarget2);
  aDoc->NewCommand();
  aDoc->Undo();

  EXPECT_EQ(aReference->Get(), aTarget1);
}

// caf/basic/M5: extension-string arrays support a second user-defined GUID.
TEST(TDataStd_Attribute_Test, CafBasic_M5_ExtStringArrayUserGuid)
{
  occ::handle<TDocStd_Document>        aDoc   = NewDocument();
  const TDF_Label                      aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID                  aGuid("12e94515-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ExtStringArray> anArray = TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, TCollection_ExtendedString("aaaaa"));
  anArray->SetValue(2, TCollection_ExtendedString("bbbbbbb"));
  occ::handle<TDataStd_ExtStringArray> aGuidArray =
    TDataStd_ExtStringArray::Set(aLabel, aGuid, 1, 2);
  aGuidArray->SetValue(1, TCollection_ExtendedString("aaaaa"));
  aGuidArray->SetValue(2, TCollection_ExtendedString("bbbbbbb"));

  ASSERT_EQ(anArray->Length(), 2);
  ASSERT_EQ(aGuidArray->Length(), 2);
  EXPECT_TRUE(anArray->Value(1).IsEqual(aGuidArray->Value(1)));
  EXPECT_TRUE(anArray->Value(2).IsEqual(aGuidArray->Value(2)));
  EXPECT_EQ(aGuidArray->ID(), aGuid);
}

// caf/basic/K1: a named shape survives ForgetAll/Undo/Redo/Undo.
TEST(TDataStd_Attribute_Test, CafBasic_K1_ShapeUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  TDF_Label                     aLabel = NewAttributeLabel(aDoc);
  const TopoDS_Shape            aBox   = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  TDataXtd_Shape::Set(aLabel, aBox);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  occ::handle<TDataXtd_Shape> anAttribute;
  aLabel = aDoc->Main().FindChild(2, false);
  if (aLabel.IsNull())
  {
    ADD_FAILURE() << "Shape label was not restored after undo";
    return;
  }
  ASSERT_TRUE(TDataXtd_Shape::Find(aLabel, anAttribute));
  EXPECT_TRUE(TDataXtd_Shape::Get(aLabel).IsSame(aBox));
  aDoc->ClearUndos();
  aDoc->ClearRedos();
}

// caf/basic/K2: undoing a replacement restores the first named shape.
TEST(TDataStd_Attribute_Test, CafBasic_K2_ShapeReplacementUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  TDF_Label                     aLabel = NewAttributeLabel(aDoc);
  const TopoDS_Shape            aBox1  = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  const TopoDS_Shape            aBox2  = BRepPrimAPI_MakeBox(gp_Pnt(-10.0, -20.0, -30.0),
                                                              100.0,
                                                              200.0,
                                                              300.0)
                                           .Shape();
  TDataXtd_Shape::Set(aLabel, aBox1);
  aDoc->NewCommand();
  TDataXtd_Shape::Set(aLabel, aBox2);
  aDoc->NewCommand();
  aDoc->Undo();

  aLabel = aDoc->Main().FindChild(2, false);
  if (aLabel.IsNull())
  {
    ADD_FAILURE() << "Shape label was not restored after undo";
    return;
  }
  occ::handle<TDataXtd_Shape> anAttribute;
  ASSERT_TRUE(TDataXtd_Shape::Find(aLabel, anAttribute));
  EXPECT_TRUE(TDataXtd_Shape::Get(aLabel).IsSame(aBox1));
  aDoc->ClearUndos();
  aDoc->ClearRedos();
}

// caf/basic/M1-M2: extension-string arrays preserve values and undo changes.
TEST(TDataStd_Attribute_Test, CafBasic_M1_ExtStringArrayUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_ExtStringArray> anArray =
    TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, TCollection_ExtendedString("TDataStd"));
  anArray->SetValue(2, TCollection_ExtendedString("ExtStringArray"));
  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  ASSERT_EQ(anArray->Length(), 2);
  EXPECT_TRUE(anArray->Value(1).IsEqual(TCollection_ExtendedString("TDataStd")));
  EXPECT_TRUE(anArray->Value(2).IsEqual(TCollection_ExtendedString("ExtStringArray")));
}

TEST(TDataStd_Attribute_Test, CafBasic_M2_ExtStringArrayReplacementUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  occ::handle<TDataStd_ExtStringArray> anArray =
    TDataStd_ExtStringArray::Set(aLabel, 1, 2);
  anArray->SetValue(1, TCollection_ExtendedString("TDataStd"));
  anArray->SetValue(2, TCollection_ExtendedString("ExtStringArray"));
  aDoc->NewCommand();
  anArray->SetValue(1, TCollection_ExtendedString("NEW TDataStd"));
  anArray->SetValue(2, TCollection_ExtendedString("NEW ExtStringArray"));
  aDoc->NewCommand();
  aDoc->Undo();

  EXPECT_TRUE(anArray->Value(1).IsEqual(TCollection_ExtendedString("TDataStd")));
  EXPECT_TRUE(anArray->Value(2).IsEqual(TCollection_ExtendedString("ExtStringArray")));
}

// caf/basic/N4-N5: user-defined ASCII-string attributes are independent and undoable.
TEST(TDataStd_Attribute_Test, CafBasic_N4_AsciiStringUserGuids)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94571-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94572-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_AsciiString::Set(aLabel, aGuid1, TCollection_AsciiString("New Attribute1"));
  TDataStd_AsciiString::Set(aLabel, aGuid2, TCollection_AsciiString("New Attribute2"));

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  occ::handle<TDataStd_AsciiString> anAttribute1;
  occ::handle<TDataStd_AsciiString> anAttribute2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, anAttribute1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, anAttribute2));
  EXPECT_EQ(anAttribute1->Get(), TCollection_AsciiString("New Attribute1"));
  EXPECT_EQ(anAttribute2->Get(), TCollection_AsciiString("New Attribute2"));
}

TEST(TDataStd_Attribute_Test, CafBasic_N5_AsciiStringReplacementUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid1("12e94571-6dbc-11d4-b9c8-0060b0ee281b");
  const Standard_GUID           aGuid2("12e94572-6dbc-11d4-b9c8-0060b0ee281b");
  TDataStd_AsciiString::Set(aLabel, aGuid1, TCollection_AsciiString("Attribute_1"));
  TDataStd_AsciiString::Set(aLabel, aGuid2, TCollection_AsciiString("Attribute_2"));
  aDoc->NewCommand();
  TDataStd_AsciiString::Set(aLabel, aGuid1, TCollection_AsciiString("Modified_1"));
  TDataStd_AsciiString::Set(aLabel, aGuid2, TCollection_AsciiString("Modified_2"));
  aDoc->NewCommand();
  aDoc->Undo();

  occ::handle<TDataStd_AsciiString> anAttribute1;
  occ::handle<TDataStd_AsciiString> anAttribute2;
  ASSERT_TRUE(aLabel.FindAttribute(aGuid1, anAttribute1));
  ASSERT_TRUE(aLabel.FindAttribute(aGuid2, anAttribute2));
  EXPECT_EQ(anAttribute1->Get(), TCollection_AsciiString("Attribute_1"));
  EXPECT_EQ(anAttribute2->Get(), TCollection_AsciiString("Attribute_2"));
}

// caf/basic/O5-P5: array attributes can be restored under a second GUID.
TEST(TDataStd_Attribute_Test, CafBasic_O5_BooleanArrayUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94516-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_BooleanArray> aDefault = TDataStd_BooleanArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_BooleanArray> aCustom = TDataStd_BooleanArray::Set(aLabel, aGuid, 1, 2);
  aDefault->SetValue(1, false);
  aDefault->SetValue(2, true);
  aCustom->SetValue(1, false);
  aCustom->SetValue(2, true);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Length(), 2);
  EXPECT_EQ(aCustom->Length(), 2);
  EXPECT_EQ(aDefault->Value(1), aCustom->Value(1));
  EXPECT_EQ(aDefault->Value(2), aCustom->Value(2));
}

TEST(TDataStd_Attribute_Test, CafBasic_P5_ByteArrayUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94517-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ByteArray> aDefault = TDataStd_ByteArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ByteArray> aCustom = TDataStd_ByteArray::Set(aLabel, aGuid, 1, 2);
  aDefault->SetValue(1, 10);
  aDefault->SetValue(2, 12);
  aCustom->SetValue(1, 10);
  aCustom->SetValue(2, 12);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Value(1), aCustom->Value(1));
  EXPECT_EQ(aDefault->Value(2), aCustom->Value(2));
}

TEST(TDataStd_Attribute_Test, CafBasic_Q5_ReferenceArrayUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94518-6dbc-11d4-b9c8-0060b0ee281b");
  const TDF_Label               aTarget1 = aDoc->Main().FindChild(3, true);
  const TDF_Label               aTarget2 = aDoc->Main().FindChild(4, true);
  occ::handle<TDataStd_ReferenceArray> aDefault =
    TDataStd_ReferenceArray::Set(aLabel, 1, 2);
  occ::handle<TDataStd_ReferenceArray> aCustom =
    TDataStd_ReferenceArray::Set(aLabel, aGuid, 1, 2);
  aDefault->SetValue(1, aTarget1);
  aDefault->SetValue(2, aTarget2);
  aCustom->SetValue(1, aTarget1);
  aCustom->SetValue(2, aTarget2);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Value(1), aCustom->Value(1));
  EXPECT_EQ(aDefault->Value(2), aCustom->Value(2));
}

// caf/basic/R5-V5: list attributes preserve values for default and custom GUIDs.
TEST(TDataStd_Attribute_Test, CafBasic_R5_RealListUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94521-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_RealList> aDefault = TDataStd_RealList::Set(aLabel);
  occ::handle<TDataStd_RealList> aCustom = TDataStd_RealList::Set(aLabel, aGuid);
  aDefault->Append(3.0);
  aDefault->Append(4.0);
  aCustom->Append(3.0);
  aCustom->Append(4.0);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Extent(), aCustom->Extent());
  EXPECT_DOUBLE_EQ(aDefault->First(), aCustom->First());
  EXPECT_DOUBLE_EQ(aDefault->Last(), aCustom->Last());
}

TEST(TDataStd_Attribute_Test, CafBasic_S5_IntegerListUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94531-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_IntegerList> aDefault = TDataStd_IntegerList::Set(aLabel);
  occ::handle<TDataStd_IntegerList> aCustom = TDataStd_IntegerList::Set(aLabel, aGuid);
  aDefault->Append(33);
  aDefault->Append(44);
  aCustom->Append(33);
  aCustom->Append(44);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Extent(), aCustom->Extent());
  EXPECT_EQ(aDefault->First(), aCustom->First());
  EXPECT_EQ(aDefault->Last(), aCustom->Last());
}

TEST(TDataStd_Attribute_Test, CafBasic_T5_ExtStringListUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94541-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_ExtStringList> aDefault = TDataStd_ExtStringList::Set(aLabel);
  occ::handle<TDataStd_ExtStringList> aCustom = TDataStd_ExtStringList::Set(aLabel, aGuid);
  aDefault->Append(TCollection_ExtendedString("aaaaa"));
  aDefault->Append(TCollection_ExtendedString("bbbbbb"));
  aCustom->Append(TCollection_ExtendedString("aaaaa"));
  aCustom->Append(TCollection_ExtendedString("bbbbbb"));

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Extent(), aCustom->Extent());
  EXPECT_TRUE(aDefault->First().IsEqual(aCustom->First()));
  EXPECT_TRUE(aDefault->Last().IsEqual(aCustom->Last()));
}

TEST(TDataStd_Attribute_Test, CafBasic_U5_BooleanListUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94551-6dbc-11d4-b9c8-0060b0ee281b");
  occ::handle<TDataStd_BooleanList> aDefault = TDataStd_BooleanList::Set(aLabel);
  occ::handle<TDataStd_BooleanList> aCustom = TDataStd_BooleanList::Set(aLabel, aGuid);
  aDefault->Append(false);
  aDefault->Append(true);
  aCustom->Append(false);
  aCustom->Append(true);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Extent(), aCustom->Extent());
  EXPECT_EQ(aDefault->First(), aCustom->First());
  EXPECT_EQ(aDefault->Last(), aCustom->Last());
}

TEST(TDataStd_Attribute_Test, CafBasic_V5_ReferenceListUserGuid)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewAttributeLabel(aDoc);
  const Standard_GUID           aGuid("12e94561-6dbc-11d4-b9c8-0060b0ee281b");
  const TDF_Label               aTarget1 = aDoc->Main().FindChild(3, true);
  const TDF_Label               aTarget2 = aDoc->Main().FindChild(4, true);
  occ::handle<TDataStd_ReferenceList> aDefault = TDataStd_ReferenceList::Set(aLabel);
  occ::handle<TDataStd_ReferenceList> aCustom = TDataStd_ReferenceList::Set(aLabel, aGuid);
  aDefault->Append(aTarget1);
  aDefault->Append(aTarget2);
  aCustom->Append(aTarget1);
  aCustom->Append(aTarget2);

  aDoc->NewCommand();
  aLabel.ForgetAllAttributes();
  aDoc->NewCommand();
  aDoc->Undo();
  aDoc->Redo();
  aDoc->Undo();

  EXPECT_EQ(aDefault->Extent(), aCustom->Extent());
  EXPECT_EQ(aDefault->First(), aCustom->First());
  EXPECT_EQ(aDefault->Last(), aCustom->Last());
}

// caf/bugs/E1: forgetting and replacing an integer must remain undoable.
TEST(TDataStd_Attribute_Test, CafBug_E1_UndoAfterForgetAndReplace)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = aDoc->Main().FindChild(1, true);
  TDataStd_Integer::Set(aLabel, 1);
  TDataStd_Real::Set(aLabel, 1.5);

  aDoc->NewCommand();
  aLabel.ForgetAttribute(TDataStd_Integer::GetID());
  TDataStd_Integer::Set(aLabel, 3);
  aDoc->NewCommand();

  EXPECT_NO_THROW(aDoc->Undo());
}
