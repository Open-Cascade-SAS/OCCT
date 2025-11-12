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
#include <XCAFDoc.hxx>

#include <gtest/gtest.h>

TEST(XCAFDoc_Test, OCC738_ShapeRefGUID)
{
  // Bug OCC738: The GUIDs in XCAFDoc have incorrect format
  // This test verifies that XCAFDoc::ShapeRefGUID() returns a valid GUID

  Standard_GUID aGUID;
  EXPECT_NO_THROW(aGUID = XCAFDoc::ShapeRefGUID());

  // Verify the GUID is not null (all zeros)
  Standard_GUID aNullGUID("00000000-0000-0000-0000-000000000000");
  EXPECT_NE(aGUID, aNullGUID) << "ShapeRefGUID should not be null";
}

TEST(XCAFDoc_Test, OCC738_AssemblyGUID)
{
  // Bug OCC738: The GUIDs in XCAFDoc have incorrect format
  // This test verifies that XCAFDoc::AssemblyGUID() returns a valid GUID

  Standard_GUID aGUID;
  EXPECT_NO_THROW(aGUID = XCAFDoc::AssemblyGUID());

  // Verify the GUID is not null (all zeros)
  Standard_GUID aNullGUID("00000000-0000-0000-0000-000000000000");
  EXPECT_NE(aGUID, aNullGUID) << "AssemblyGUID should not be null";
}
