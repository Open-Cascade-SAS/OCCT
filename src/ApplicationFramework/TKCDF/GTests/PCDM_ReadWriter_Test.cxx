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

#include <PCDM_ReadWriter.hxx>
#include <TCollection_ExtendedString.hxx>

TEST(PCDM_ReadWriterTest, ParseReference)
{
  int                        anIdentifier = 0;
  int                        aVersion     = 0;
  TCollection_ExtendedString aFileName;

  ASSERT_TRUE(
    PCDM_ReadWriter::ParseReference("12 3 relative/path.ext", anIdentifier, aVersion, aFileName));
  EXPECT_EQ(anIdentifier, 12);
  EXPECT_EQ(aVersion, 3);
  EXPECT_EQ(aFileName, TCollection_ExtendedString("relative/path.ext"));
}

TEST(PCDM_ReadWriterTest, ParseReferenceFileNameWithSpaces)
{
  int                        anIdentifier = 0;
  int                        aVersion     = 0;
  TCollection_ExtendedString aFileName;

  ASSERT_TRUE(
    PCDM_ReadWriter::ParseReference("7 2 folder/file name.ext", anIdentifier, aVersion, aFileName));
  EXPECT_EQ(anIdentifier, 7);
  EXPECT_EQ(aVersion, 2);
  EXPECT_EQ(aFileName, TCollection_ExtendedString("folder/file name.ext"));
}

TEST(PCDM_ReadWriterTest, RejectMalformedReferenceWithoutChangingOutputs)
{
  const TCollection_ExtendedString aMalformedReferences[] = {
    "",
    "1",
    "1 2",
    "identifier 2 file.ext",
    "1 version file.ext",
    "1 2 ",
  };

  for (const TCollection_ExtendedString& aReference : aMalformedReferences)
  {
    int                        anIdentifier = 10;
    int                        aVersion     = 20;
    TCollection_ExtendedString aFileName("unchanged");

    EXPECT_FALSE(PCDM_ReadWriter::ParseReference(aReference, anIdentifier, aVersion, aFileName));
    EXPECT_EQ(anIdentifier, 10);
    EXPECT_EQ(aVersion, 20);
    EXPECT_EQ(aFileName, TCollection_ExtendedString("unchanged"));
  }
}
