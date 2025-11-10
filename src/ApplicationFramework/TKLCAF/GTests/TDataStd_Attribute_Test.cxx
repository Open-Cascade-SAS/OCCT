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
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

TEST(TDataStd_Attribute_Test, OCC29371_AttributeGUIDsNotNull)
{
  // Create document and label
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  Handle(TDocStd_Document)    aDoc;
  anApp->NewDocument("BinOcaf", aDoc);
  TDF_Label aLab = aDoc->Main();

  // Null GUID for comparison
  Standard_GUID aNullGuid("00000000-0000-0000-0000-000000000000");

  // Test each TDataStd attribute type has non-null GUID
  Handle(TDataStd_AsciiString) aStrAtt = new TDataStd_AsciiString();
  aLab.AddAttribute(aStrAtt);
  EXPECT_NE(aNullGuid, aStrAtt->ID());

  Handle(TDataStd_BooleanArray) aBArAtt = new TDataStd_BooleanArray();
  aLab.AddAttribute(aBArAtt);
  EXPECT_NE(aNullGuid, aBArAtt->ID());

  Handle(TDataStd_BooleanList) aBListAtt = new TDataStd_BooleanList();
  aLab.AddAttribute(aBListAtt);
  EXPECT_NE(aNullGuid, aBListAtt->ID());

  Handle(TDataStd_ByteArray) aByteArAtt = new TDataStd_ByteArray();
  aLab.AddAttribute(aByteArAtt);
  EXPECT_NE(aNullGuid, aByteArAtt->ID());

  Handle(TDataStd_ExtStringArray) anExtStrArAtt = new TDataStd_ExtStringArray();
  aLab.AddAttribute(anExtStrArAtt);
  EXPECT_NE(aNullGuid, anExtStrArAtt->ID());

  Handle(TDataStd_ExtStringList) anExtStrListAtt = new TDataStd_ExtStringList();
  aLab.AddAttribute(anExtStrListAtt);
  EXPECT_NE(aNullGuid, anExtStrListAtt->ID());

  Handle(TDataStd_Integer) anIntAtt = new TDataStd_Integer();
  aLab.AddAttribute(anIntAtt);
  EXPECT_NE(aNullGuid, anIntAtt->ID());

  Handle(TDataStd_IntegerArray) anIntArrAtt = new TDataStd_IntegerArray();
  aLab.AddAttribute(anIntArrAtt);
  EXPECT_NE(aNullGuid, anIntArrAtt->ID());

  Handle(TDataStd_IntegerList) anIntListAtt = new TDataStd_IntegerList();
  aLab.AddAttribute(anIntListAtt);
  EXPECT_NE(aNullGuid, anIntListAtt->ID());

  Handle(TDataStd_Name) aNameAtt = new TDataStd_Name();
  aLab.AddAttribute(aNameAtt);
  EXPECT_NE(aNullGuid, aNameAtt->ID());

  Handle(TDataStd_Real) aRealAtt = new TDataStd_Real();
  aLab.AddAttribute(aRealAtt);
  EXPECT_NE(aNullGuid, aRealAtt->ID());

  Handle(TDataStd_RealArray) aRealArrAtt = new TDataStd_RealArray();
  aLab.AddAttribute(aRealArrAtt);
  EXPECT_NE(aNullGuid, aRealArrAtt->ID());

  Handle(TDataStd_RealList) aRealListAtt = new TDataStd_RealList();
  aLab.AddAttribute(aRealListAtt);
  EXPECT_NE(aNullGuid, aRealListAtt->ID());

  Handle(TDataStd_ReferenceArray) aRefArrAtt = new TDataStd_ReferenceArray();
  aLab.AddAttribute(aRefArrAtt);
  EXPECT_NE(aNullGuid, aRefArrAtt->ID());

  Handle(TDataStd_ReferenceList) aRefListAtt = new TDataStd_ReferenceList();
  aLab.AddAttribute(aRefListAtt);
  EXPECT_NE(aNullGuid, aRefListAtt->ID());

  anApp->Close(aDoc);
}
