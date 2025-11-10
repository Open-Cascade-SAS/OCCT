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

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>

// Test BUC60925: TNaming_Name solve with empty NamedShape
TEST(TNaming_Name_Test, BUC60925_SolveWithEmptyNamedShape)
{
  // Create TDF document
  Handle(TDF_Data) aDF = new TDF_Data();

  // Create a label
  TDF_Label aLabel = aDF->Root().FindChild(2, Standard_True);

  // Create label map
  TDF_LabelMap aLabelMap;
  aLabelMap.Add(aLabel);

  // Create an empty NamedShape
  Handle(TNaming_NamedShape) aNS = new TNaming_NamedShape;

  // Create TNaming_Name and configure it
  TNaming_Name aNN;
  aNN.Type(TNaming_IDENTITY);
  aNN.Append(aNS);

  // Test that Solve returns false for empty NamedShape
  Standard_Boolean aResult = aNN.Solve(aLabel, aLabelMap);

  EXPECT_FALSE(aResult) << "Solve should return false for empty NamedShape";
}
