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

#include <gtest/gtest.h>

#include <CDF_FWOSDriver.hxx>
#include <CDM_MetaData.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

TEST(CDF_FWOSDriverTest, ExistingReadableFile)
{
  TCollection_AsciiString aFolder;
  TCollection_AsciiString aFileName;
  OSD_Path::FolderAndFileFromPath(__FILE__, aFolder, aFileName);
  ASSERT_FALSE(aFolder.IsEmpty());
  ASSERT_FALSE(aFileName.IsEmpty());

  NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>> aLookUpTable;
  CDF_FWOSDriver                                                             aDriver(aLookUpTable);

  EXPECT_TRUE(aDriver.FindFolder(aFolder));
  EXPECT_TRUE(aDriver.Find(aFolder, aFileName, ""));
  EXPECT_TRUE(aDriver.HasReadPermission(aFolder, aFileName, ""));
}

TEST(CDF_FWOSDriverTest, MissingFileIsNotReadable)
{
  TCollection_AsciiString aFolder;
  TCollection_AsciiString aFileName;
  OSD_Path::FolderAndFileFromPath(__FILE__, aFolder, aFileName);
  ASSERT_FALSE(aFolder.IsEmpty());

  NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>> aLookUpTable;
  CDF_FWOSDriver                                                             aDriver(aLookUpTable);

  const TCollection_ExtendedString aMissingName("CDF_FWOSDriver_missing_test_file");
  EXPECT_FALSE(aDriver.Find(aFolder, aMissingName, ""));
  EXPECT_FALSE(aDriver.HasReadPermission(aFolder, aMissingName, ""));
}
