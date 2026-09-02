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

#include <Standard_SHA256.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <string>

namespace
{
void expectDigest(const void* theData, const size_t theSize, const char* theExpected)
{
  Standard_SHA256::Digest aDigest;
  ASSERT_TRUE(Standard_SHA256::Hash(theData, theSize, aDigest));
  EXPECT_STREQ(aDigest.ToString().ToCString(), theExpected);
}
} // namespace

TEST(Standard_SHA256Test, PublishedVectors)
{
  expectDigest(nullptr,
               0,
               "e3b0c44298fc1c149afbf4c8996fb924"
               "27ae41e4649b934ca495991b7852b855");
  static constexpr char THE_ABC[] = "abc";
  expectDigest(THE_ABC,
               3,
               "ba7816bf8f01cfea414140de5dae2223"
               "b00361a396177a9cb410ff61f20015ad");
  static constexpr char THE_LONG[] =
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  expectDigest(THE_LONG,
               std::strlen(THE_LONG),
               "248d6a61d20638b8e5c026930c3e6039"
               "a33ce45964ff2167f6ecedd419db06c1");
}

TEST(Standard_SHA256Test, ChunkingDoesNotChangeDigest)
{
  std::array<uint8_t, 1000> aBytes;
  for (size_t anIndex = 0; anIndex < aBytes.size(); ++anIndex)
  {
    aBytes[anIndex] = static_cast<uint8_t>(anIndex);
  }

  Standard_SHA256::Digest aContiguous;
  ASSERT_TRUE(Standard_SHA256::Hash(aBytes.data(), aBytes.size(), aContiguous));

  Standard_SHA256 aChunked;
  size_t          anOffset = 0;
  while (anOffset < aBytes.size())
  {
    const size_t aSize = std::min<size_t>((anOffset % 37) + 1, aBytes.size() - anOffset);
    ASSERT_TRUE(aChunked.Append(aBytes.data() + anOffset, aSize));
    anOffset += aSize;
  }
  Standard_SHA256::Digest aResult;
  ASSERT_TRUE(aChunked.Finish(aResult));
  EXPECT_EQ(aResult, aContiguous);
}

TEST(Standard_SHA256Test, FinishDoesNotConsumeState)
{
  Standard_SHA256 aHash;
  ASSERT_TRUE(aHash.Append("abc", 3));

  Standard_SHA256::Digest aFirst;
  Standard_SHA256::Digest aSecond;
  ASSERT_TRUE(aHash.Finish(aFirst));
  ASSERT_TRUE(aHash.Finish(aSecond));
  EXPECT_EQ(aFirst, aSecond);

  ASSERT_TRUE(aHash.Append("def", 3));
  Standard_SHA256::Digest aExtended;
  ASSERT_TRUE(aHash.Finish(aExtended));
  expectDigest("abcdef", 6, aExtended.ToString().ToCString());
}
