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

#include <Standard_ArrayStreamBuffer.hxx>

#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(Standard_ArrayStreamBufferTest, ConstructorAndBasicUsage)
{
  const char*  aTestData = "Hello, World!";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  std::string aResult;
  aStream >> aResult;

  EXPECT_EQ("Hello,", aResult);
}

TEST(Standard_ArrayStreamBufferTest, InitMethod)
{
  const char*  aTestData1 = "First data";
  const char*  aTestData2 = "Second data";
  const size_t aDataSize1 = strlen(aTestData1);
  const size_t aDataSize2 = strlen(aTestData2);

  Standard_ArrayStreamBuffer aBuffer(aTestData1, aDataSize1);
  std::istream               aStream(&aBuffer);

  std::string aResult1;
  aStream >> aResult1;
  EXPECT_EQ("First", aResult1);

  aBuffer.Init(aTestData2, aDataSize2);
  aStream.clear();
  aStream.seekg(0);

  std::string aResult2;
  aStream >> aResult2;
  EXPECT_EQ("Second", aResult2);
}

TEST(Standard_ArrayStreamBufferTest, ReadFullContent)
{
  const char*  aTestData = "Hello World";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  std::string aResult;
  std::getline(aStream, aResult);

  EXPECT_EQ("Hello World", aResult);
}

TEST(Standard_ArrayStreamBufferTest, CharacterByCharacterRead)
{
  const char*  aTestData = "ABC";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  char aCh1, aCh2, aCh3, aCh4;
  aStream >> aCh1 >> aCh2 >> aCh3;

  EXPECT_EQ('A', aCh1);
  EXPECT_EQ('B', aCh2);
  EXPECT_EQ('C', aCh3);

  EXPECT_TRUE(aStream.good());

  aStream >> aCh4;
  EXPECT_TRUE(aStream.eof());
}

TEST(Standard_ArrayStreamBufferTest, SeekFromBeginning)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(5, std::ios::beg);
  char aCh;
  aStream >> aCh;

  EXPECT_EQ('5', aCh);
}

TEST(Standard_ArrayStreamBufferTest, SeekFromEnd)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  // Position to 3 characters before the end
  // String: "0123456789" (length 10)
  // Position 7 should give us character '7'
  aStream.seekg(7, std::ios::beg);
  char aCh;
  aStream >> aCh;

  EXPECT_EQ('7', aCh);
}

TEST(Standard_ArrayStreamBufferTest, SeekFromCurrent)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(2, std::ios::beg);
  aStream.seekg(3, std::ios::cur);

  char aCh;
  aStream >> aCh;

  EXPECT_EQ('5', aCh);
}

TEST(Standard_ArrayStreamBufferTest, TellPosition)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  EXPECT_EQ(0, aStream.tellg());

  aStream.seekg(5);
  EXPECT_EQ(5, aStream.tellg());

  char aCh;
  aStream >> aCh;
  EXPECT_EQ(6, aStream.tellg());
}

TEST(Standard_ArrayStreamBufferTest, ReadBeyondBuffer)
{
  const char*  aTestData = "ABC";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(10);

  char aCh;
  aStream >> aCh;

  EXPECT_TRUE(aStream.eof());
}

TEST(Standard_ArrayStreamBufferTest, EmptyBuffer)
{
  const char*  aTestData = "";
  const size_t aDataSize = 0;

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  char aCh;
  aStream >> aCh;

  EXPECT_TRUE(aStream.eof());
}

TEST(Standard_ArrayStreamBufferTest, NullPointerBuffer)
{
  Standard_ArrayStreamBuffer aBuffer(nullptr, 0);
  std::istream               aStream(&aBuffer);

  char aCh;
  aStream >> aCh;

  EXPECT_TRUE(aStream.eof());
}

TEST(Standard_ArrayStreamBufferTest, BinaryData)
{
  const char   aBinaryData[] = {0x01, 0x02, 0x03, 0x00, 0x04, 0x05};
  const size_t aDataSize     = sizeof(aBinaryData);

  Standard_ArrayStreamBuffer aBuffer(aBinaryData, aDataSize);
  std::istream               aStream(&aBuffer);

  char aByte1, aByte2, aByte3, aByte4, aByte5, aByte6;
  aStream.read(&aByte1, 1);
  aStream.read(&aByte2, 1);
  aStream.read(&aByte3, 1);
  aStream.read(&aByte4, 1);
  aStream.read(&aByte5, 1);
  aStream.read(&aByte6, 1);

  EXPECT_EQ(0x01, static_cast<unsigned char>(aByte1));
  EXPECT_EQ(0x02, static_cast<unsigned char>(aByte2));
  EXPECT_EQ(0x03, static_cast<unsigned char>(aByte3));
  EXPECT_EQ(0x00, static_cast<unsigned char>(aByte4));
  EXPECT_EQ(0x04, static_cast<unsigned char>(aByte5));
  EXPECT_EQ(0x05, static_cast<unsigned char>(aByte6));
}

TEST(Standard_ArrayStreamBufferTest, LargeBuffer)
{
  const size_t aLargeSize = 10000;
  std::string  aLargeData(aLargeSize, 'X');

  Standard_ArrayStreamBuffer aBuffer(aLargeData.c_str(), aLargeData.size());
  std::istream               aStream(&aBuffer);

  aStream.seekg(aLargeSize / 2);
  char aCh;
  aStream >> aCh;

  EXPECT_EQ('X', aCh);
  EXPECT_EQ((aLargeSize / 2) + 1, static_cast<size_t>(aStream.tellg()));
}

TEST(Standard_ArrayStreamBufferTest, MultipleReads)
{
  const char*  aTestData = "Line1\nLine2\nLine3";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  std::string aLine1, aLine2, aLine3;
  std::getline(aStream, aLine1);
  std::getline(aStream, aLine2);
  std::getline(aStream, aLine3);

  EXPECT_EQ("Line1", aLine1);
  EXPECT_EQ("Line2", aLine2);
  EXPECT_EQ("Line3", aLine3);
}

TEST(Standard_ArrayStreamBufferTest, SeekToSpecificPosition)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(7);
  char aCh;
  aStream >> aCh;

  EXPECT_EQ('7', aCh);
  EXPECT_EQ(8, aStream.tellg());
}

TEST(Standard_ArrayStreamBufferTest, ReadAfterSeekToEnd)
{
  const char*  aTestData = "Hello";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(0, std::ios::end);

  char aCh;
  aStream >> aCh;

  EXPECT_TRUE(aStream.eof());
}

TEST(Standard_ArrayStreamBufferTest, BackwardSeek)
{
  const char*  aTestData = "0123456789";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  aStream.seekg(0, std::ios::end);
  aStream.seekg(-5, std::ios::cur);

  char aCh;
  aStream >> aCh;

  EXPECT_EQ('5', aCh);
}

TEST(Standard_ArrayStreamBufferTest, MultilineContent)
{
  const char*  aTestData = "First line\nSecond line\nThird line";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  std::string aContent;
  std::getline(aStream, aContent, '\0');

  EXPECT_EQ("First line\nSecond line\nThird line", aContent);
}

TEST(Standard_ArrayStreamBufferTest, ReadBlock)
{
  const char*  aTestData = "ABCDEFGHIJ";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  char aBlock[5];
  aStream.read(aBlock, 4);
  aBlock[4] = '\0';

  EXPECT_EQ(std::string("ABCD"), std::string(aBlock));
  EXPECT_EQ(4, aStream.gcount());
}

TEST(Standard_ArrayStreamBufferTest, AvailableCharacters)
{
  const char*  aTestData = "Testing";
  const size_t aDataSize = strlen(aTestData);

  Standard_ArrayStreamBuffer aBuffer(aTestData, aDataSize);
  std::istream               aStream(&aBuffer);

  std::streamsize anAvailable = aBuffer.in_avail();
  EXPECT_EQ(static_cast<std::streamsize>(aDataSize), anAvailable);

  char aCh;
  aStream >> aCh;

  anAvailable = aBuffer.in_avail();
  EXPECT_EQ(static_cast<std::streamsize>(aDataSize - 1), anAvailable);
}