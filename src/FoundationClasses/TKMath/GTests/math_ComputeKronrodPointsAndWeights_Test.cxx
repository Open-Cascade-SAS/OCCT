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

#include <math_ComputeKronrodPointsAndWeights.hxx>

#include <gtest/gtest.h>

TEST(math_ComputeKronrodPointsAndWeights_Test, OCC33048_ComputeWithOrder125)
{
  // This method uses raw pointers for memory manipulations
  // Test that it completes successfully without crashes
  Standard_Boolean isOK = Standard_True;
  try
  {
    math_ComputeKronrodPointsAndWeights aCalc(125);
    EXPECT_TRUE(aCalc.IsDone()) << "Kronrod points and weights calculation should succeed";
    isOK = aCalc.IsDone();
  }
  catch (...)
  {
    FAIL() << "Exception occurred during calculation of Kronrod points and weights";
    isOK = Standard_False;
  }

  EXPECT_TRUE(isOK);
}
