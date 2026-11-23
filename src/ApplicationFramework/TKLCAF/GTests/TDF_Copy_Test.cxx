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

#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_ClosureTool.hxx>
#include <TDF_CopyLabel.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_Document.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewCopyDocument()
{
  return new TDocStd_Document("BinOcaf");
}

static TDF_Label RootChild(const occ::handle<TDocStd_Document>& theDocument, const int theTag)
{
  return theDocument->Main().Root().FindChild(theTag, true);
}

static void ExpectCopiedLabel(const TDF_Label&                  theLabel,
                              const TCollection_ExtendedString& theExpectedName,
                              const double                      theExpectedReal)
{
  occ::handle<TDataStd_Name> aName;
  if (!theLabel.FindAttribute(TDataStd_Name::GetID(), aName))
  {
    ADD_FAILURE() << "Name attribute is missing";
    return;
  }
  EXPECT_TRUE(aName->Get().IsEqual(theExpectedName));
  occ::handle<TDataStd_Real> aReal;
  if (!theLabel.FindAttribute(TDataStd_Real::GetID(), aReal))
  {
    ADD_FAILURE() << "Real attribute is missing";
    return;
  }
  EXPECT_DOUBLE_EQ(aReal->Get(), theExpectedReal);
}
} // namespace

// caf/bugs/buc60813: TDF_CopyLabel can copy a label repeatedly without losing
// the copied name or real attribute.
TEST(TDF_Copy_Test, Buc_CafBug_60813_CopyLabelTwice)
{
  occ::handle<TDocStd_Document> aDocument = NewCopyDocument();
  aDocument->SetUndoLimit(100);
  aDocument->NewCommand();

  const TDF_Label aSource  = aDocument->Main();
  const TDF_Label aTarget1 = RootChild(aDocument, 2);
  const TDF_Label aTarget2 = RootChild(aDocument, 3);
  TDataStd_Name::Set(aSource, TCollection_ExtendedString("SourceLabel"));
  TDataStd_Real::Set(aSource, 3.1415);

  TDF_CopyLabel aFirstCopy;
  aFirstCopy.Load(aSource, aTarget1);
  aFirstCopy.Perform();
  ASSERT_TRUE(aFirstCopy.IsDone());
  ExpectCopiedLabel(aTarget1, TCollection_ExtendedString("SourceLabel"), 3.1415);

  TDF_CopyLabel aSecondCopy;
  aSecondCopy.Load(aTarget1, aTarget2);
  aSecondCopy.Perform();
  ASSERT_TRUE(aSecondCopy.IsDone());
  ExpectCopiedLabel(aTarget2, TCollection_ExtendedString("SourceLabel"), 3.1415);
}

// caf/bugs/bug22976: CopyDF copies the content of a label, not only the label
// itself.
TEST(TDF_Copy_Test, CafBug_22976_CopyDataFrameworkLabel)
{
  occ::handle<TDocStd_Document> aDocument = NewCopyDocument();
  aDocument->SetUndoLimit(100);
  aDocument->NewCommand();

  const TDF_Label aSource = aDocument->Main();
  const TDF_Label aTarget = RootChild(aDocument, 2);
  TDataStd_Real::Set(aSource, 1.2345);

  occ::handle<TDF_DataSet> aDataSet = new TDF_DataSet();
  aDataSet->AddLabel(aSource);
  TDF_ClosureTool::Closure(aDataSet);

  occ::handle<TDF_RelocationTable> aRelocation = new TDF_RelocationTable();
  aRelocation->SetRelocation(aSource, aTarget);
  TDF_CopyTool::Copy(aDataSet, aRelocation);

  occ::handle<TDataStd_Real> aCopiedReal;
  ASSERT_TRUE(aTarget.FindAttribute(TDataStd_Real::GetID(), aCopiedReal));
  EXPECT_DOUBLE_EQ(aCopiedReal->Get(), 1.2345);
}
