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

#include <gtest/gtest.h>

#include <TDataStd_TreeNode.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

// Test BUC60817: TDataStd_TreeNode descendant relationship
TEST(TDataStd_TreeNode_Test, BUC60817_DescendantRelationship)
{
  // Create TDF document
  Handle(TDF_Data) aDF = new TDF_Data();

  // Create two labels
  TDF_Label aLabel1 = aDF->Root().FindChild(2, Standard_True);
  TDF_Label aLabel2 = aDF->Root().FindChild(3, Standard_True);

  // Create TreeNodes on the labels
  Handle(TDataStd_TreeNode) aTN1 = TDataStd_TreeNode::Set(aLabel1);
  Handle(TDataStd_TreeNode) aTN2 = TDataStd_TreeNode::Set(aLabel2);

  // Append TN2 as a child of TN1
  aTN1->Append(aTN2);

  // Verify that TN2 is a descendant of TN1
  EXPECT_TRUE(aTN2->IsDescendant(aTN1)) << "TN2 should be a descendant of TN1 after Append";

  // Verify that TN1 is NOT a descendant of TN2
  EXPECT_FALSE(aTN1->IsDescendant(aTN2)) << "TN1 should not be a descendant of TN2";
}
