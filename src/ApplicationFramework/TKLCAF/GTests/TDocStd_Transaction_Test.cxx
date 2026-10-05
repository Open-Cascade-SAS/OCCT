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

#include <BinDrivers.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_List.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Current.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Variable.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Delta.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDocStd_ApplicationDelta.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Modified.hxx>
#include <TDocStd_MultiTransactionManager.hxx>
#include <TFunction_Function.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewTransactionDocument()
{
  return new TDocStd_Document("BinOcaf");
}

static occ::handle<TDocStd_Document> NewApplicationTransactionDocument(
  occ::handle<TDocStd_Application>& theApplication)
{
  theApplication = new TDocStd_Application();
  BinDrivers::DefineFormat(theApplication);
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument("BinOcaf", aDocument);
  return aDocument;
}

static TDF_Label RootChild(const occ::handle<TDocStd_Document>& theDocument, const int theTag)
{
  return theDocument->Main().Root().FindChild(theTag, true);
}

static bool HasModifiedLabels(const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<TDocStd_Modified> aModified;
  if (!theDocument->Main().Root().FindAttribute(TDocStd_Modified::GetID(), aModified))
  {
    return false;
  }
  return !aModified->IsEmpty();
}

static double RealValue(const TDF_Label& theLabel)
{
  occ::handle<TDataStd_Real> aReal;
  if (!theLabel.FindAttribute(TDataStd_Real::GetID(), aReal))
  {
    ADD_FAILURE() << "Real attribute is missing";
    return 0.0;
  }
  return aReal->Get();
}

static void ExpectLastDeltaIs(const occ::handle<TDocStd_Document>& theDocument,
                              const char*                          theAttributeName)
{
  ASSERT_FALSE(theDocument->GetUndos().IsEmpty());
  const occ::handle<TDF_Delta>& aDelta = theDocument->GetUndos().Last();
  ASSERT_FALSE(aDelta->AttributeDeltas().IsEmpty());
  EXPECT_STREQ(aDelta->AttributeDeltas().Last()->Attribute()->DynamicType()->Name(),
               theAttributeName);
}

static void ExpectLastDeltaIsReal(const occ::handle<TDocStd_Document>& theDocument)
{
  ExpectLastDeltaIs(theDocument, "TDataStd_Real");
}

static bool DeltaHasAttribute(const occ::handle<TDF_Delta>& theDelta, const char* theAttributeName)
{
  for (NCollection_List<occ::handle<TDF_AttributeDelta>>::Iterator anIterator(
         theDelta->AttributeDeltas());
       anIterator.More();
       anIterator.Next())
  {
    if (TCollection_AsciiString(anIterator.Value()->Attribute()->DynamicType()->Name())
          .IsEqual(theAttributeName))
    {
      return true;
    }
  }
  return false;
}

static void ExpectLastDeltaHas(const occ::handle<TDocStd_Document>& theDocument,
                               const char*                          theFirstAttributeName,
                               const char*                          theSecondAttributeName)
{
  ASSERT_FALSE(theDocument->GetUndos().IsEmpty());
  const occ::handle<TDF_Delta>& aDelta = theDocument->GetUndos().Last();
  EXPECT_TRUE(DeltaHasAttribute(aDelta, theFirstAttributeName));
  EXPECT_TRUE(DeltaHasAttribute(aDelta, theSecondAttributeName));
}
} // namespace

// caf/bugs/buc60790: the most recent redo keeps the name of the second undone delta.
TEST(TDocStd_Transaction_Test, Buc_CafBug_60790_TreeNodeDeltaNames)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);

  aDocument->NewCommand();
  ASSERT_FALSE(TDataStd_TreeNode::Set(RootChild(aDocument, 2)).IsNull());
  aDocument->NewCommand();
  ASSERT_FALSE(TDataStd_TreeNode::Set(RootChild(aDocument, 3)).IsNull());
  aDocument->NewCommand();
  ASSERT_FALSE(TDataStd_TreeNode::Set(RootChild(aDocument, 4)).IsNull());
  aDocument->NewCommand();

  ASSERT_EQ(aDocument->GetAvailableUndos(), 3);
  const TCollection_ExtendedString aNames[3] = {TCollection_ExtendedString("n1"),
                                                TCollection_ExtendedString("n2"),
                                                TCollection_ExtendedString("n3")};
  int                              anIndex   = 0;
  for (NCollection_List<occ::handle<TDF_Delta>>::Iterator anIterator(aDocument->GetUndos());
       anIterator.More();
       anIterator.Next(), ++anIndex)
  {
    ASSERT_LT(anIndex, 3);
    anIterator.Value()->SetName(aNames[anIndex]);
  }
  ASSERT_EQ(anIndex, 3);

  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Undo());
  ASSERT_EQ(aDocument->GetAvailableUndos(), 1);
  ASSERT_EQ(aDocument->GetAvailableRedos(), 2);
  ASSERT_FALSE(aDocument->GetRedos().IsEmpty());
  EXPECT_TRUE(aDocument->GetRedos().First()->Name().IsEqual(TCollection_ExtendedString("n2")));
}

// caf/bugs/buc60831: modified labels are restored when the command adding them is undone.
TEST(TDocStd_Transaction_Test, Buc_CafBug_60831_ModifiedAttributeUndo)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);

  aDocument->NewCommand();
  const TDF_Label aLabel = RootChild(aDocument, 3);
  EXPECT_FALSE(HasModifiedLabels(aDocument));

  aDocument->NewCommand();
  EXPECT_TRUE(TDocStd_Modified::Add(aLabel));
  EXPECT_TRUE(HasModifiedLabels(aDocument));

  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());
  EXPECT_FALSE(HasModifiedLabels(aDocument));
}

// caf/bugs/bug1722: a multi-document transaction manager commits and restores the
// second value while preserving the value set before the managed transaction.
TEST(TDocStd_Transaction_Test, CafBug_1722_MultiTransactionManagerUndoRedo)
{
  occ::handle<TDocStd_Document>                aDocument = NewTransactionDocument();
  occ::handle<TDocStd_MultiTransactionManager> aManager  = new TDocStd_MultiTransactionManager();
  aDocument->SetUndoLimit(4);
  aManager->SetUndoLimit(4);
  aManager->AddDocument(aDocument);

  const TDF_Label aLabel = aDocument->Main().FindChild(1, true);
  TDataStd_Real::Set(aLabel, 1.1);
  EXPECT_DOUBLE_EQ(RealValue(aLabel), 1.1);

  aManager->OpenCommand();
  TDataStd_Real::Set(aLabel, 2.1);
  EXPECT_DOUBLE_EQ(RealValue(aLabel), 2.1);
  EXPECT_TRUE(aManager->GetAvailableUndos().IsEmpty());

  ASSERT_TRUE(aManager->CommitCommand(TCollection_ExtendedString("Transaction1")));
  ASSERT_EQ(aManager->GetAvailableUndos().Length(), 1);
  EXPECT_TRUE(aManager->GetAvailableUndos().First()->GetName().IsEqual(
    TCollection_ExtendedString("Transaction1")));

  aManager->Undo();
  EXPECT_DOUBLE_EQ(RealValue(aLabel), 1.1);
  ASSERT_EQ(aManager->GetAvailableRedos().Length(), 1);

  aManager->Redo();
  EXPECT_DOUBLE_EQ(RealValue(aLabel), 2.1);
  EXPECT_TRUE(aManager->GetAvailableRedos().IsEmpty());
}

// caf/bugs/bug25112: undoing a packed-map modification restores both removal
// and addition performed by one command.
TEST(TDocStd_Transaction_Test, CafBug_25112_IntPackedMapUndo)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(10);
  const TDF_Label aLabel = aDocument->Main();

  occ::handle<TDataStd_IntPackedMap> aMap = TDataStd_IntPackedMap::Set(aLabel);
  ASSERT_FALSE(aMap.IsNull());
  ASSERT_TRUE(aMap->Add(319));

  aDocument->OpenCommand();
  ASSERT_TRUE(aMap->Add(344));
  ASSERT_TRUE(aMap->Remove(319));
  ASSERT_TRUE(aDocument->CommitCommand());

  ASSERT_TRUE(aDocument->Undo());
  occ::handle<TDataStd_IntPackedMap> aRestoredMap;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_IntPackedMap::GetID(), aRestoredMap));
  EXPECT_TRUE(aRestoredMap->Contains(319));
  EXPECT_FALSE(aRestoredMap->Contains(344));
}

// caf/bugs/bug29142: undo and redo of a forget-plus-replace command keep the
// integer attribute valid rather than leaving a missing attribute behind.
TEST(TDocStd_Transaction_Test, CafBug_29142_ForgetAndReplaceIntegerUndoRedo)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = aDocument->Main().FindChild(1, true);

  aDocument->NewCommand();
  TDataStd_Integer::Set(aLabel, 111);
  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 0.234);
  aDocument->NewCommand();
  ASSERT_TRUE(aLabel.ForgetAttribute(TDataStd_Integer::GetID()));
  TDataStd_Integer::Set(aLabel, 222);
  aDocument->NewCommand();

  ASSERT_TRUE(aDocument->Undo());
  occ::handle<TDataStd_Integer> aInteger;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aInteger));
  EXPECT_EQ(aInteger->Get(), 111);
  occ::handle<TDataStd_Real> aReal;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Real::GetID(), aReal));
  EXPECT_DOUBLE_EQ(aReal->Get(), 0.234);

  ASSERT_TRUE(aDocument->Redo());
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aInteger));
  EXPECT_EQ(aInteger->Get(), 222);
}

// caf/bugs/bug31920: enabling entry indexing does not change the value
// retrieved from a label.
TEST(TDocStd_Transaction_Test, CafBug_31920_AccessLabelByEntry)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  const TDF_Label               aLabel    = aDocument->Main().Root().FindChild(2, true);
  TDataStd_Integer::Set(aLabel, 5);

  occ::handle<TDataStd_Integer> aValue;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Integer::GetID(), aValue));
  EXPECT_EQ(aValue->Get(), 5);

  aDocument->GetData()->SetAccessByEntries(true);
  TDF_Label aFoundLabel;
  ASSERT_TRUE(aDocument->GetData()->GetLabel(TCollection_AsciiString("0:2"), aFoundLabel));
  EXPECT_EQ(aFoundLabel, aLabel);
  ASSERT_TRUE(aFoundLabel.FindAttribute(TDataStd_Integer::GetID(), aValue));
  EXPECT_EQ(aValue->Get(), 5);
}

// caf/bugs/bug2932_9: repeating an unchanged integer attribute must not hide
// the real attribute modification in the committed transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_9_UnchangedIntegerKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  TDataStd_Integer::Set(aLabel, 100);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Integer::Set(aLabel, 100);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// caf/bugs/bug2932_11: an unchanged name attribute must not become the
// reported modified attribute when a real value changes.
TEST(TDocStd_Transaction_Test, CafBug_2932_11_UnchangedNameKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  TDataStd_Name::Set(aLabel, TCollection_ExtendedString("New Name"));
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Name::Set(aLabel, TCollection_ExtendedString("New Name"));
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// caf/bugs/bug2932_1: unchanged integer-array values must not replace the
// real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_1_UnchangedIntegerArrayKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// caf/bugs/bug2932_16: the same delta rule applies to real arrays.
TEST(TDocStd_Transaction_Test, CafBug_2932_16_UnchangedRealArrayKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  anArray = TDataStd_RealArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug261: clearing redo history after two undo operations leaves no
// redo command available while preserving the remaining undo history.
TEST(TDocStd_Transaction_Test, CafBug_261_ClearRedosAfterUndo)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(10);
  const TDF_Label aLabel = aDocument->Main();

  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 10.0);
  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 20.0);
  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 30.0);
  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 40.0);

  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Undo());
  EXPECT_EQ(aDocument->GetAvailableUndos(), 1);
  EXPECT_EQ(aDocument->GetAvailableRedos(), 2);

  aDocument->ClearRedos();
  EXPECT_EQ(aDocument->GetAvailableUndos(), 1);
  EXPECT_EQ(aDocument->GetAvailableRedos(), 0);
}

// bugs/caf/bug2932_2: repeating an unchanged real-array attribute must not
// replace the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_2_UnchangedRealArrayKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  occ::handle<TDataStd_RealArray> anArray = TDataStd_RealArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  anArray = TDataStd_RealArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3.0);
  anArray->SetValue(2, 4.0);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_3: repeating an unchanged axis attribute must not replace
// the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_3_UnchangedAxisKeepsRealDelta)
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  BinDrivers::DefineFormat(anApplication);
  occ::handle<TDocStd_Document> aDocument;
  anApplication->NewDocument("BinOcaf", aDocument);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);
  const gp_Lin    anAxis(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(100.0, 200.0, 300.0));

  aDocument->NewCommand();
  TDataXtd_Axis::Set(aLabel, anAxis);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataXtd_Axis::Set(aLabel, anAxis);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_4: repeating an unchanged comment attribute must not
// replace the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_4_UnchangedCommentKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label                  aLabel = RootChild(aDocument, 2);
  const TCollection_ExtendedString aComment("New Comment");

  aDocument->NewCommand();
  TDataStd_Comment::Set(aLabel, aComment);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Comment::Set(aLabel, aComment);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_10: repeating an unchanged integer-array attribute must
// not replace the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_10_UnchangedIntegerArrayKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  occ::handle<TDataStd_IntegerArray> anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  anArray = TDataStd_IntegerArray::Set(aLabel, 1, 2, false);
  anArray->SetValue(1, 3);
  anArray->SetValue(2, 4);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_6: an unchanged current attribute must not replace the
// real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_6_UnchangedCurrentKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  TDataStd_Current::Set(aLabel);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Current::Set(aLabel);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_7: an unchanged expression must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_7_UnchangedExpressionKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label                  aLabel = RootChild(aDocument, 2);
  const TCollection_ExtendedString anExpression("New Expression");

  aDocument->NewCommand();
  TDataStd_Expression::Set(aLabel)->SetExpression(anExpression);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Expression::Set(aLabel)->SetExpression(anExpression);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_15: changing an integer while repeating an unchanged real
// must report the integer attribute as the last delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_15_UnchangedRealKeepsIntegerDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  TDataStd_Real::Set(aLabel, 100.0);
  TDataStd_Integer::Set(aLabel, 300);
  aDocument->NewCommand();

  TDataStd_Real::Set(aLabel, 100.0);
  TDataStd_Integer::Set(aLabel, 400);
  aDocument->NewCommand();

  ExpectLastDeltaIs(aDocument, "TDataStd_Integer");
}

// bugs/caf/bug2932_17: an unchanged relation must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_17_UnchangedRelationKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label                  aLabel = RootChild(aDocument, 2);
  const TCollection_ExtendedString aRelation("New Relation");

  aDocument->NewCommand();
  TDataStd_Relation::Set(aLabel)->SetRelation(aRelation);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataStd_Relation::Set(aLabel)->SetRelation(aRelation);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_19: an unchanged variable must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_19_UnchangedVariableKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  occ::handle<TDataStd_Variable> aVariable = TDataStd_Variable::Set(aLabel);
  aVariable->Unit(TCollection_AsciiString("kg/m3"));
  aVariable->Constant(true);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  aVariable = TDataStd_Variable::Set(aLabel);
  aVariable->Unit(TCollection_AsciiString("kg/m3"));
  aVariable->Constant(true);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_20: an unchanged label reference must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_20_UnchangedReferenceKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel     = RootChild(aDocument, 2);
  const TDF_Label aReference = RootChild(aDocument, 3);

  aDocument->NewCommand();
  TDF_Reference::Set(aLabel, aReference);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDF_Reference::Set(aLabel, aReference);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_21: an unchanged tag source must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_21_UnchangedTagSourceKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);

  aDocument->NewCommand();
  TDF_TagSource::Set(aLabel)->Set(5);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDF_TagSource::Set(aLabel)->Set(5);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_22: an unchanged function attribute must not replace the
// real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_22_UnchangedFunctionKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label     aLabel = RootChild(aDocument, 2);
  const Standard_GUID aDriverGuid("5b35ca00-5b78-11d1-8940-080009dc3333");

  aDocument->NewCommand();
  occ::handle<TFunction_Function> aFunction = TFunction_Function::Set(aLabel, aDriverGuid);
  aFunction->SetFailure(13);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  aFunction = TFunction_Function::Set(aLabel, aDriverGuid);
  aFunction->SetFailure(13);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_24: an unchanged position must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_24_UnchangedPositionKeepsRealDelta)
{
  occ::handle<TDocStd_Document> aDocument = NewTransactionDocument();
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);
  const gp_Pnt    aPosition(123.456, 789.012, 345.678);

  aDocument->NewCommand();
  TDataXtd_Position::Set(aLabel, aPosition);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataXtd_Position::Set(aLabel, aPosition);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_8: repeating unchanged construction geometry keeps both the
// named-shape and real attribute deltas in the transaction.
TEST(TDocStd_Transaction_Test, CafBug_2932_8_UnchangedGeometryKeepsBothDeltas)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label    aLabel = RootChild(aDocument, 2);
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), 100.0, 200.0, 300.0).Shape();

  aDocument->NewCommand();
  TNaming_Builder(aLabel).Generated(aBox);
  TDataXtd_Geometry::Set(aLabel)->SetType(TDataXtd_POINT);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TNaming_Builder(aLabel).Generated(aBox);
  TDataXtd_Geometry::Set(aLabel)->SetType(TDataXtd_POINT);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaHas(aDocument, "TDataStd_Real", "TNaming_NamedShape");
}

// bugs/caf/bug2932_13: an unchanged plane construction attribute must not
// replace the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_13_UnchangedPlaneKeepsRealDelta)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);
  const gp_Pln    aPlane(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(-1.0, 0.0, 0.0));

  aDocument->NewCommand();
  TDataXtd_Plane::Set(aLabel, aPlane);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataXtd_Plane::Set(aLabel, aPlane);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_14: an unchanged point construction attribute must not
// replace the real attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_14_UnchangedPointKeepsRealDelta)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label aLabel = RootChild(aDocument, 2);
  const gp_Pnt    aPoint(10.0, 20.0, 30.0);

  aDocument->NewCommand();
  TDataXtd_Point::Set(aLabel, aPoint);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TDataXtd_Point::Set(aLabel, aPoint);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_18: repeating an unchanged named shape keeps both the
// named-shape and real attribute deltas in the transaction.
TEST(TDocStd_Transaction_Test, CafBug_2932_18_UnchangedShapeKeepsBothDeltas)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label    aLabel = RootChild(aDocument, 2);
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), 100.0, 200.0, 300.0).Shape();

  aDocument->NewCommand();
  TNaming_Builder(aLabel).Generated(aBox);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  TNaming_Builder(aLabel).Generated(aBox);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  ExpectLastDeltaHas(aDocument, "TDataStd_Real", "TNaming_NamedShape");
}

// bugs/caf/bug2932_5: unchanged constraint metadata must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_5_UnchangedConstraintKeepsRealDelta)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label    aLabel      = RootChild(aDocument, 2);
  const TDF_Label    aShapeLabel = RootChild(aDocument, 3);
  const TDF_Label    anIntLabel  = RootChild(aDocument, 4);
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), 100.0, 200.0, 300.0).Shape();

  aDocument->NewCommand();
  TNaming_Builder(aShapeLabel).Generated(aBox);
  TDataStd_Integer::Set(anIntLabel, 10);
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aShapeLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  occ::handle<TDataXtd_Constraint> aConstraint = TDataXtd_Constraint::Set(aLabel);
  aConstraint->SetType(TDataXtd_TANGENT);
  aConstraint->SetGeometry(1, aNamedShape);
  aConstraint->SetPlane(aNamedShape);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  aConstraint = TDataXtd_Constraint::Set(aLabel);
  aConstraint->SetType(TDataXtd_TANGENT);
  aConstraint->SetGeometry(1, aNamedShape);
  aConstraint->SetPlane(aNamedShape);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  EXPECT_EQ(aConstraint->GetType(), TDataXtd_TANGENT);
  EXPECT_EQ(aConstraint->NbGeometries(), 1);
  EXPECT_FALSE(aConstraint->GetPlane().IsNull());
  ExpectLastDeltaIsReal(aDocument);
}

// bugs/caf/bug2932_12: unchanged pattern metadata must not replace the real
// attribute modification in the transaction delta.
TEST(TDocStd_Transaction_Test, CafBug_2932_12_UnchangedPatternKeepsRealDelta)
{
  occ::handle<TDocStd_Application> anApplication;
  occ::handle<TDocStd_Document>    aDocument = NewApplicationTransactionDocument(anApplication);
  ASSERT_FALSE(aDocument.IsNull());
  aDocument->SetUndoLimit(100);
  const TDF_Label    aLabel      = RootChild(aDocument, 2);
  const TDF_Label    aShapeLabel = RootChild(aDocument, 3);
  const TDF_Label    aRealLabel  = RootChild(aDocument, 4);
  const TDF_Label    anIntLabel  = RootChild(aDocument, 5);
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(10.0, 20.0, 30.0), 100.0, 200.0, 300.0).Shape();

  aDocument->NewCommand();
  TNaming_Builder(aShapeLabel).Generated(aBox);
  TDataStd_Real::Set(aRealLabel, 123.456789);
  TDataStd_Integer::Set(anIntLabel, 123456789);

  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aShapeLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  occ::handle<TDataStd_Real> aPatternValue;
  ASSERT_TRUE(aRealLabel.FindAttribute(TDataStd_Real::GetID(), aPatternValue));
  occ::handle<TDataStd_Integer> aPatternInstances;
  ASSERT_TRUE(anIntLabel.FindAttribute(TDataStd_Integer::GetID(), aPatternInstances));

  occ::handle<TDataXtd_PatternStd> aPattern = TDataXtd_PatternStd::Set(aLabel);
  aPattern->Signature(1);
  aPattern->Axis1(aNamedShape);
  aPattern->Value1(aPatternValue);
  aPattern->NbInstances1(aPatternInstances);
  TDataStd_Real::Set(aLabel, 300.0);
  aDocument->NewCommand();

  aPattern = TDataXtd_PatternStd::Set(aLabel);
  aPattern->Signature(1);
  aPattern->Axis1(aNamedShape);
  aPattern->Value1(aPatternValue);
  aPattern->NbInstances1(aPatternInstances);
  TDataStd_Real::Set(aLabel, 400.0);
  aDocument->NewCommand();

  EXPECT_EQ(aPattern->Signature(), 1);
  ASSERT_FALSE(aPattern->Axis1().IsNull());
  EXPECT_TRUE(aPattern->Axis1()->Get().IsEqual(aBox));
  ASSERT_FALSE(aPattern->Value1().IsNull());
  EXPECT_DOUBLE_EQ(aPattern->Value1()->Get(), 123.456789);
  ASSERT_FALSE(aPattern->NbInstances1().IsNull());
  EXPECT_EQ(aPattern->NbInstances1()->Get(), 123456789);
  ExpectLastDeltaIsReal(aDocument);
}
