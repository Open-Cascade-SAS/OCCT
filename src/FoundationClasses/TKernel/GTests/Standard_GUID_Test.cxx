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

#include <Standard_GUID.hxx>

#include <Standard_Failure.hxx>

#include <gtest/gtest.h>

TEST(Standard_GUID_Test, OCC669_ValidGUIDConstruction)
{
  // Bug OCC669: Standard_GUID(HoleFeature) cause stack overwrite
  // This test verifies that Standard_GUID correctly parses various valid GUID strings
  // with hex digits (0-9, a-f, A-F) in different positions

  // Test GUIDs with uppercase hex digits A-F
  EXPECT_NO_THROW(Standard_GUID("0000000A-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000B-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000C-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000D-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000E-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000F-0000-0000-0000-000000000000"));

  // Test GUIDs with lowercase hex digits a-f
  EXPECT_NO_THROW(Standard_GUID("0000000a-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000b-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000c-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000d-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000e-0000-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("0000000f-0000-0000-0000-000000000000"));

  // Test hex digits in second group
  EXPECT_NO_THROW(Standard_GUID("00000000-000A-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-000F-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-000a-0000-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-000f-0000-0000-000000000000"));

  // Test hex digits in third group
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-000A-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-000F-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-000a-0000-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-000f-0000-000000000000"));

  // Test hex digits in fourth group
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-000A-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-000F-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-000a-000000000000"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-000f-000000000000"));

  // Test hex digits in fifth group
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000A"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000F"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000a"));
  EXPECT_NO_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000f"));
}

TEST(Standard_GUID_Test, OCC669_InvalidGUIDConstruction)
{
  // Test that invalid GUID strings throw exceptions or fail gracefully
  // Invalid characters (G/g instead of valid hex)
  EXPECT_THROW(Standard_GUID("0000000G-0000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("0000000g-0000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-000G-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-000g-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-000G-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-000g-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-000G-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-000g-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000G"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000g"), Standard_Failure);

  // Invalid formats - wrong lengths
  EXPECT_THROW(Standard_GUID("000000000000000000000000000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("0000000000000000000000000000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000000000000000000000000000000"), Standard_Failure);

  // Invalid formats - wrong group lengths
  EXPECT_THROW(Standard_GUID("0000000000000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-000000000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-000000000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-00000000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("000000000-0000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("0000000-0000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-00000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-000-0000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-00000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-000-0000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-00000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-000-000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-0000-0000000000000"), Standard_Failure);
  EXPECT_THROW(Standard_GUID("00000000-0000-0000-0000-00000000000"), Standard_Failure);

  // Empty string
  EXPECT_THROW(Standard_GUID(""), Standard_Failure);
}

TEST(Standard_GUID_Test, OCC669_GUIDFormatting)
{
  // Test that a valid GUID can be constructed and formatted back to a string
  Standard_GUID aGUID1("00000000-0000-0000-0000-000000000000");
  Standard_GUID aGUID2("0000000A-000B-000C-000D-00000000000E");
  Standard_GUID aGUID3("0000000a-000b-000c-000d-00000000000e");

  // Verify that GUIDs with the same string representation are equal
  Standard_GUID aGUID4("0000000A-000B-000C-000D-00000000000E");
  EXPECT_EQ(aGUID2, aGUID4);

  // Verify that uppercase and lowercase hex digits create equal GUIDs
  EXPECT_EQ(aGUID2, aGUID3);

  // Verify that different GUIDs are not equal
  EXPECT_NE(aGUID1, aGUID2);
}
