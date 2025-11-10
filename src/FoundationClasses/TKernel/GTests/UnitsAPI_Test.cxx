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

#include <UnitsAPI.hxx>

#include <gtest/gtest.h>

// Test BUC60727: UnitsAPI unit conversion
// Migrated from QABugs_3.cxx
TEST(UnitsAPI_Test, BUC60727_AnyToLS_Conversion)
{
  // Set local system to MDTV (Millimeter, Degree, Ton, Velocity)
  UnitsAPI::SetLocalSystem(UnitsAPI_MDTV);

  // Test conversion: 3 mm in the MDTV system should remain 3
  // (since the base unit for length in MDTV is millimeter)
  Standard_Real aResult = UnitsAPI::AnyToLS(3.0, "mm");
  EXPECT_DOUBLE_EQ(3.0, aResult);
}
