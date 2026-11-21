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

#include <StepData_Field.hxx>
#include <StepData_SelectInt.hxx>

#include <gtest/gtest.h>

#include <array>

TEST(StepData_SelectMemberTest, LogicalSelectPreservesAllValues)
{
  occ::handle<StepData_SelectInt> aSelect = new StepData_SelectInt;
  StepData_Field                  aField;
  aField.SetSelectMember(aSelect);

  for (const StepData_Logical aValue :
       std::array<StepData_Logical, 3>{StepData_LFalse, StepData_LTrue, StepData_LUnknown})
  {
    aField.SetLogical(aValue);
    EXPECT_EQ(aSelect->Logical(), aValue);
    EXPECT_EQ(aField.Logical(), aValue);
  }
}
