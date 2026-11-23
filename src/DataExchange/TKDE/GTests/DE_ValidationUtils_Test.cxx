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

#include <DE_ValidationUtils.hxx>
#include <DE_Provider.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_Buffer.hxx>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <istream>
#include <streambuf>

namespace
{
class DE_TestUnseekableBuffer : public std::streambuf
{
public:
  DE_TestUnseekableBuffer();
};

//==================================================================================================

DE_TestUnseekableBuffer::DE_TestUnseekableBuffer() = default;
} // namespace

//==================================================================================================

TEST(DE_ValidationUtilsTest, ResolvesRelativeNamesWithinTransferRoot)
{
  const char                  aBytes[] = "data";
  Standard_ArrayStreamBuffer  aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream                aStream(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/part.obj", aStream));
  aStreams.Append(DE_Provider::ReadStreamNode("materials/paint.mtl", aStream));
  ASSERT_TRUE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
  TCollection_AsciiString aName;
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams,
                                               "models/part.obj",
                                               "../materials/./paint.mtl",
                                               aName),
            &aStream);
  EXPECT_EQ(aName, "materials/paint.mtl");
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams,
                                               "materials/paint.mtl",
                                               "../models/part.obj",
                                               aName),
            &aStream);
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams,
                                               "models/part.obj",
                                               "../materials/missing.mtl",
                                               aName),
            nullptr);
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams,
                                               "models/part.obj",
                                               "../materials/Paint.mtl",
                                               aName),
            nullptr);
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, RejectsNamesOutsideTransferRoot)
{
  const char                  aBytes[] = "data";
  Standard_ArrayStreamBuffer  aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream                aStream(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/part.obj", aStream));
  TCollection_AsciiString aName;
  for (const char* aReference : {"../../other.mtl",
                                 "/other.mtl",
                                 "C:/other.mtl",
                                 "file:other.mtl",
                                 "https://other.mtl",
                                 "other\\file.mtl",
                                 ""})
  {
    EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams, "models/part.obj", aReference, aName),
              nullptr);
    EXPECT_TRUE(aName.IsEmpty());
  }
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams, "/invalid.obj", "part.obj", aName),
            nullptr);
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, RejectsDuplicateReducedNames)
{
  const char                  aBytes[] = "data";
  Standard_ArrayStreamBuffer  aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream                aStream(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/part.obj", aStream));
  aStreams.Append(DE_Provider::ReadStreamNode("models/./part.obj", aStream));
  EXPECT_FALSE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, RejectsInvalidCompanionStream)
{
  const char                 aBytes[] = "data";
  Standard_ArrayStreamBuffer aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream               aStream(&aBuffer);
  std::istream               aFailed(&aBuffer);
  aFailed.setstate(std::ios::badbit);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("part.obj", aStream));
  aStreams.Append(DE_Provider::ReadStreamNode("part.mtl", aFailed));
  EXPECT_TRUE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false));
  EXPECT_FALSE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, OwnedBufferOutlivesBorrowedInput)
{
  constexpr size_t                aBufferSize = 130000;
  occ::handle<NCollection_Buffer> aRead;
  {
    NCollection_Buffer aBytes(NCollection_BaseAllocator::CommonBaseAllocator(), aBufferSize);
    for (size_t anIndex = 0; anIndex < aBytes.Size(); ++anIndex)
    {
      aBytes.ChangeData()[anIndex] = static_cast<uint8_t>(anIndex % 251);
    }
    Standard_ArrayStreamBuffer aBuffer(reinterpret_cast<const char*>(aBytes.Data()), aBytes.Size());
    std::istream               aStream(&aBuffer);
    aStream.seekg(100);
    ASSERT_TRUE(DE_ValidationUtils::ReadStreamBuffer(aStream, aRead));
    ASSERT_FALSE(aRead.IsNull());
    ASSERT_EQ(aRead->Size(), aBytes.Size());
    EXPECT_EQ(std::memcmp(aRead->Data(), aBytes.Data(), aBytes.Size()), 0);
  }
  ASSERT_EQ(aRead->Size(), aBufferSize);
  EXPECT_EQ(aRead->Data()[aBufferSize - 1], static_cast<uint8_t>((aBufferSize - 1) % 251));
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, EmptyAndUnseekableInput)
{
  const char                      aBytes[] = "";
  Standard_ArrayStreamBuffer      aBuffer(aBytes, 0);
  std::istream                    aStream(&aBuffer);
  occ::handle<NCollection_Buffer> aRead;
  ASSERT_TRUE(DE_ValidationUtils::ReadStreamBuffer(aStream, aRead));
  ASSERT_FALSE(aRead.IsNull());
  EXPECT_EQ(aRead->Size(), 0);
  DE_TestUnseekableBuffer aUnseekable;
  std::istream            aFailed(&aUnseekable);
  EXPECT_FALSE(DE_ValidationUtils::ReadStreamBuffer(aFailed, aRead));
  EXPECT_TRUE(aRead.IsNull());
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, RejectsInvalidCompanionNames)
{
  const char                 aBytes[] = "data";
  Standard_ArrayStreamBuffer aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream               aStream(&aBuffer);
  for (const char* aName : {"../part.obj", "/part.obj", "C:/part.obj", "folder\\part.obj"})
  {
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode(aName, aStream));
    EXPECT_TRUE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false));
    EXPECT_FALSE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true))
      << aName;
  }
  TCollection_AsciiString aNullName(9, ' ');
  aNullName.SetValue(1, "part\0.obj", 9);
  ASSERT_EQ(aNullName.Length(), 9);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode(aNullName, aStream));
  EXPECT_FALSE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, UnnamedMainAndNamedCompanion)
{
  const char                  aBytes[] = "data";
  Standard_ArrayStreamBuffer  aBuffer(aBytes, sizeof(aBytes) - 1);
  std::istream                aStream(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("", aStream));
  aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aStream));
  ASSERT_TRUE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
  TCollection_AsciiString aName;
  EXPECT_EQ(DE_ValidationUtils::FindReadStream(aStreams, "", "mesh.bin", aName), &aStream);
  aStreams.Append(DE_Provider::ReadStreamNode("", aStream));
  EXPECT_FALSE(DE_ValidationUtils::ValidateReadStreamList(aStreams, "streams", false, true));
}

//==================================================================================================

TEST(DE_ValidationUtilsTest, PropagatesStreamExceptions)
{
  DE_TestUnseekableBuffer aBuffer;
  std::istream            aStream(&aBuffer);
  aStream.exceptions(std::ios::badbit | std::ios::failbit);
  occ::handle<NCollection_Buffer> aRead =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), 1);
  EXPECT_THROW(DE_ValidationUtils::ReadStreamBuffer(aStream, aRead), std::ios_base::failure);
  EXPECT_TRUE(aRead.IsNull());
}
