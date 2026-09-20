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
  static constexpr char THE_LONG[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
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

TEST(Standard_SHA256Test, PaddingAndBlockBoundaries)
{
  std::array<unsigned char, 129> aBytes;
  for (size_t i = 0; i < aBytes.size(); ++i)
  {
    aBytes[i] = static_cast<unsigned char>(i);
  }
  expectDigest(aBytes.data(), 55, "463eb28e72f82e0a96c0a4cc53690c571281131f672aa229e0d45ae59b598b59");
  expectDigest(aBytes.data(), 56, "da2ae4d6b36748f2a318f23e7ab1dfdf45acdc9d049bd80e59de82a60895f562");
  expectDigest(aBytes.data(), 63, "29af2686fd53374a36b0846694cc342177e428d1647515f078784d69cdb9e488");
  expectDigest(aBytes.data(), 64, "fdeab9acf3710362bd2658cdc9a29e8f9c757fcf9811603a8c447cd1d9151108");
  expectDigest(aBytes.data(), 65, "4bfd2c8b6f1eec7a2afeb48b934ee4b2694182027e6d0fc075074f2fabb31781");
  expectDigest(aBytes.data(), 119, "da18797ed7c3a777f0847f429724a2d8cd5138e6ed2895c3fa1a6d39d18f7ec6");
  expectDigest(aBytes.data(), 120, "f52b23db1fbb6ded89ef42a23ce0c8922c45f25c50b568a93bf1c075420bbb7c");
  expectDigest(aBytes.data(), 127, "92ca0fa6651ee2f97b884b7246a562fa71250fedefe5ebf270d31c546bfea976");
  expectDigest(aBytes.data(), 128, "471fb943aa23c511f6f72f8d1652d9c880cfa392ad80503120547703e56a2be5");
  expectDigest(aBytes.data(), 129, "5099c6a56203f9687f7d33f4bfdf576d31dc91f6b695ecea38b2770c87631135");
}
