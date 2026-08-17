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
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

TEST(StepData_FieldCopyTest, CopyKeepsIndependentRealValuesAtNegativeBounds)
{
  StepData_Field aSource;
  aSource.SetReal();
  aSource.SetList(2, -2);
  aSource.SetReal(-2, 1.5);
  aSource.SetReal(-1, 2.5);

  StepData_Field aCopy;
  aCopy.CopyFrom(aSource);
  aSource.SetReal(-2, 3.5);

  EXPECT_EQ(aCopy.Lower(), -2);
  EXPECT_EQ(aCopy.Length(), 2);
  EXPECT_DOUBLE_EQ(aCopy.Real(-2), 1.5);
  EXPECT_DOUBLE_EQ(aCopy.Real(-1), 2.5);
}

TEST(StepData_FieldCopyTest, CopyPreservesUnsetStringItems)
{
  StepData_Field aSource;
  aSource.SetString();
  aSource.SetList(2, -1);
  aSource.SetString(0, "text");

  StepData_Field aCopy;
  aCopy.CopyFrom(aSource);
  aSource.SetString(0, "changed");

  EXPECT_FALSE(aCopy.IsSet(-1));
  EXPECT_STREQ(aCopy.String(0), "text");
}
