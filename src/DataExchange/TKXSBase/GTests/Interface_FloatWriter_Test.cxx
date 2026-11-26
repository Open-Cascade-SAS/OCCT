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

#include <Interface_FloatWriter.hxx>

#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <vector>

// A bounded Write never writes past the buffer and reports the size it needs.
TEST(Interface_FloatWriterTest, BoundedWrite_ReportsRequiredSize)
{
  const double aValue           = -1.2345678901234567e-123;
  char         anExpected[64]   = {};
  const int    anExpectedLength = std::snprintf(anExpected, sizeof(anExpected), "%.17E", aValue);
  ASSERT_EQ(anExpectedLength, 25);

  Interface_FloatWriter aWriter;
  aWriter.SetFormat("%.17E");

  char aSmall[12];
  std::memset(aSmall, '#', sizeof(aSmall));
  EXPECT_EQ(aWriter.Write(aValue, aSmall, 8), anExpectedLength);
  EXPECT_EQ(aSmall[7], '\0');
  EXPECT_EQ(aSmall[8], '#');

  std::vector<char> aBuffer(static_cast<size_t>(anExpectedLength) + 1);
  EXPECT_EQ(aWriter.Write(aValue, aBuffer.data(), aBuffer.size()), anExpectedLength);
  EXPECT_STREQ(aBuffer.data(), anExpected);
}

// The bounded overloads keep the zero suppression of the legacy ones.
TEST(Interface_FloatWriterTest, BoundedWrite_MatchesLegacyZeroSuppression)
{
  Interface_FloatWriter aWriter;
  for (const double aValue : {0.0, 1.0, -2.5, 1.0e-7, 123456.0, -1.0e+100})
  {
    char      aLegacy[32]   = {};
    char      aBounded[32]  = {};
    const int aLegacyLength = aWriter.Write(aValue, aLegacy);
    EXPECT_EQ(aWriter.Write(aValue, aBounded, sizeof(aBounded)), aLegacyLength);
    EXPECT_STREQ(aBounded, aLegacy);
  }
}
