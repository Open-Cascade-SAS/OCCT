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

#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(XCAFDoc_DatumTest, PointRoundTripUsesIndependentArray)
{
  const occ::handle<TDocStd_Document> aDoc = new TDocStd_Document("BinXCAF");
  const occ::handle<XCAFDoc_DimTolTool> aTool = XCAFDoc_DocumentTool::DimTolTool(aDoc->Main());
  for (const int aCase : {0, 1, 2})
  {
    SCOPED_TRACE(aCase);
    const bool hasPlane = aCase != 0;
    const bool hasPoint = aCase != 1;
    const TDF_Label aLabel = aTool->AddDatum();
    occ::handle<XCAFDoc_Datum> anAttribute;
    ASSERT_TRUE(aLabel.FindAttribute(XCAFDoc_Datum::GetID(), anAttribute));
    const occ::handle<XCAFDimTolObjects_DatumObject> anObject = anAttribute->GetObject();
    if (hasPlane)
    {
      anObject->SetPlane(gp_Ax2(gp_Pnt(6.0, 6.0, 6.0), gp_Dir(0.0, 0.0, 1.0)));
    }
    if (hasPoint)
    {
      anObject->SetPoint(gp_Pnt(7.0, 8.0, 9.0));
    }
    anAttribute->SetObject(anObject);

    const occ::handle<XCAFDimTolObjects_DatumObject> aResult = anAttribute->GetObject();
    ASSERT_FALSE(aResult.IsNull());
    EXPECT_EQ(aResult->HasPlane(), hasPlane);
    ASSERT_EQ(aResult->HasPoint(), hasPoint);
    if (hasPoint)
    {
      EXPECT_DOUBLE_EQ(aResult->GetPoint().X(), 7.0);
      EXPECT_DOUBLE_EQ(aResult->GetPoint().Y(), 8.0);
      EXPECT_DOUBLE_EQ(aResult->GetPoint().Z(), 9.0);
    }
  }
}
