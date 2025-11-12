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

#include <TDF_AttributeIterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <gtest/gtest.h>

TEST(TDF_AttributeIterator_Test, OCC24755_AttributeInsertionOrder)
{
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document)    aDoc;
  anApp->NewDocument("BinOcaf", aDoc);
  TDF_Label aLab = aDoc->Main();

  // Prepend an int value.
  TDataStd_Integer::Set(aLab, 0);
  // Prepend a name.
  TDataStd_Name::Set(aLab, "test");
  // Append a double value.
  aLab.AddAttribute(new TDataStd_Real(), true /*append*/);

  // Verify that attributes are iterated in the expected order:
  // Integer (prepended second, so it's first), Name (prepended first), Real (appended)
  TDF_AttributeIterator anIter(aLab);

  ASSERT_TRUE(anIter.More()) << "Iterator should have first attribute";
  Handle(TDF_Attribute) anAttr = anIter.Value();
  EXPECT_TRUE(anAttr->IsKind(STANDARD_TYPE(TDataStd_Integer)))
    << "First attribute should be TDataStd_Integer";

  anIter.Next();
  ASSERT_TRUE(anIter.More()) << "Iterator should have second attribute";
  anAttr = anIter.Value();
  EXPECT_TRUE(anAttr->IsKind(STANDARD_TYPE(TDataStd_Name)))
    << "Second attribute should be TDataStd_Name";

  anIter.Next();
  ASSERT_TRUE(anIter.More()) << "Iterator should have third attribute";
  anAttr = anIter.Value();
  EXPECT_TRUE(anAttr->IsKind(STANDARD_TYPE(TDataStd_Real)))
    << "Third attribute should be TDataStd_Real";

  anIter.Next();
  EXPECT_FALSE(anIter.More()) << "Iterator should have no more attributes";
}
