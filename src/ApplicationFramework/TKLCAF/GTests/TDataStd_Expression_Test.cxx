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

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewExpressionDocument()
{
  return new TDocStd_Document("BinOcaf");
}

static occ::handle<TDataStd_Variable> SetVariable(const TDF_Label&                  theLabel,
                                                  const TCollection_AsciiString& theUnit)
{
  occ::handle<TDataStd_Variable> aVariable = TDataStd_Variable::Set(theLabel);
  aVariable->Constant(false);
  aVariable->Unit(theUnit);
  return aVariable;
}

static occ::handle<TDataStd_Relation> SetRelation(
  const TDF_Label&                             theLabel,
  const TCollection_ExtendedString&            theExpression,
  const occ::handle<TDataStd_Variable>&        theVariable)
{
  occ::handle<TDataStd_Relation> aRelation = TDataStd_Relation::Set(theLabel);
  aRelation->SetRelation(theExpression);
  aRelation->GetVariables().Append(theVariable);
  return aRelation;
}
} // namespace

// caf/bugs/bug387: undoing a relation change restores the previous expression.
TEST(TDataStd_Expression_Test, CafBug_387_RelationUndo)
{
  occ::handle<TDocStd_Document> aDocument = NewExpressionDocument();
  aDocument->SetUndoLimit(10);
  const TDF_Label aLabel = aDocument->Main();

  aDocument->NewCommand();
  occ::handle<TDataStd_Variable> aVariable = SetVariable(aLabel, TCollection_AsciiString("a"));
  occ::handle<TDataStd_Relation> aRelation =
    SetRelation(aLabel, TCollection_ExtendedString("a=10"), aVariable);
  TDataStd_Real::Set(aLabel, 10.0);

  aDocument->NewCommand();
  aRelation->SetRelation(TCollection_ExtendedString("a=20"));
  TDataStd_Real::Set(aLabel, 20.0);

  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());

  occ::handle<TDataStd_Relation> aRestoredRelation;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Relation::GetID(), aRestoredRelation));
  EXPECT_TRUE(aRestoredRelation->GetRelation().IsEqual(TCollection_ExtendedString("a=10")));
  occ::handle<TDataStd_Real> aRestoredReal;
  ASSERT_TRUE(aLabel.FindAttribute(TDataStd_Real::GetID(), aRestoredReal));
  EXPECT_DOUBLE_EQ(aRestoredReal->Get(), 10.0);
}

// caf/bugs/bug644: repeated undo/redo of a relation leaves the original
// expression and its variable reference intact.
TEST(TDataStd_Expression_Test, CafBug_644_RelationUndoRedo)
{
  occ::handle<TDocStd_Document> aDocument = NewExpressionDocument();
  aDocument->SetUndoLimit(10);
  const TDF_Label aVariableLabel = aDocument->Main();
  const TDF_Label aRelationLabel = aDocument->Main().Root().FindChild(2, true);

  aDocument->NewCommand();
  occ::handle<TDataStd_Variable> aVariable =
    SetVariable(aVariableLabel, TCollection_AsciiString("mm"));
  occ::handle<TDataStd_Relation> aRelation =
    SetRelation(aRelationLabel, TCollection_ExtendedString("x=10"), aVariable);

  aDocument->NewCommand();
  aRelation->SetRelation(TCollection_ExtendedString("x=20"));

  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Redo());
  ASSERT_TRUE(aDocument->Undo());

  occ::handle<TDataStd_Relation> aRestoredRelation;
  ASSERT_TRUE(aRelationLabel.FindAttribute(TDataStd_Relation::GetID(), aRestoredRelation));
  EXPECT_TRUE(aRestoredRelation->GetRelation().IsEqual(TCollection_ExtendedString("x=10")));
  ASSERT_EQ(aRestoredRelation->GetVariables().Extent(), 1);
  EXPECT_EQ(aRestoredRelation->GetVariables().First()->Label(), aVariableLabel);
}
