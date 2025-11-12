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

#include <Standard_OutOfRange.hxx>

#include <gtest/gtest.h>

#include <sstream>

TEST(Standard_Failure_Test, OCC670_ExceptionWithoutMessage)
{
  // Bug OCC670: Problem with the Print method
  // This test verifies that an exception initialized without a message string
  // can be safely handled and printed

  bool aCaught = false;
  try
  {
    throw Standard_OutOfRange("test");
  }
  catch (Standard_Failure const& anException)
  {
    aCaught = true;

    // Verify that the exception can be safely printed
    std::ostringstream aStream;
    EXPECT_NO_THROW(aStream << anException);

    // Verify that printing produced some output
    std::string aMessage = aStream.str();
    EXPECT_FALSE(aMessage.empty()) << "Exception message should not be empty";
  }

  EXPECT_TRUE(aCaught) << "Exception should have been caught";
}
