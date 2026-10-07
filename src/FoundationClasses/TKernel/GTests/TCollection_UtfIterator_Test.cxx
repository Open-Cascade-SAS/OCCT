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

#include <TCollection_UtfIterator.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <memory>

TEST(TCollection_UtfIteratorTest, DecodeAndEncodeUtf8)
{
  constexpr char aText[] =
    {'\x24', '\xC2', '\xA2', '\xE2', '\x82', '\xAC', '\xF0', '\x9F', '\x98', '\x80', '\0'};
  constexpr char32_t anExpected[] = {0x24, 0xA2, 0x20AC, 0x1F600};
  constexpr int      aSizes[]     = {1, 2, 3, 4};

  TCollection_UtfIterator<char> anIterator(aText);
  const char*                   aPosition = aText;
  for (int anIndex = 0; anIndex < 4; ++anIndex, ++anIterator)
  {
    EXPECT_TRUE(anIterator.IsValid());
    EXPECT_EQ(anExpected[anIndex], *anIterator);
    EXPECT_EQ(aSizes[anIndex], anIterator.AdvanceBytesUtf8());
    EXPECT_EQ(aPosition, anIterator.BufferHere());

    unsigned char aBuffer[4] = {};
    EXPECT_EQ(aBuffer + aSizes[anIndex], anIterator.GetUtf8(aBuffer));
    for (int aByte = 0; aByte < aSizes[anIndex]; ++aByte)
    {
      EXPECT_EQ(static_cast<unsigned char>(aPosition[aByte]), aBuffer[aByte]);
    }
    aPosition += aSizes[anIndex];
  }
  EXPECT_EQ(char32_t(0), *anIterator);
}

TEST(TCollection_UtfIteratorTest, EveryTwoByteInput)
{
  for (uint32_t aFirst = 0; aFirst < 256; ++aFirst)
  {
    for (uint32_t aSecond = 0; aSecond < 256; ++aSecond)
    {
      const std::array<unsigned char, 2>     aData = {static_cast<unsigned char>(aFirst),
                                                      static_cast<unsigned char>(aSecond)};
      TCollection_UtfIterator<unsigned char> aRead(aData);
      const bool isPair = aFirst >= 0xC2 && aFirst <= 0xDF && aSecond >= 0x80 && aSecond <= 0xBF;
      ASSERT_EQ(aRead.IsValid(), aFirst < 0x80 || isPair);
      ASSERT_EQ(aRead.CodeUnits(), static_cast<size_t>(isPair ? 2 : 1));
      if (aFirst < 0x80)
      {
        ASSERT_EQ(*aRead, static_cast<char32_t>(aFirst));
      }
      else if (isPair)
      {
        ASSERT_EQ(*aRead, static_cast<char32_t>((aFirst - 0xC0) * 64 + aSecond - 0x80));
      }
      ++aRead;
      if (!isPair)
      {
        ASSERT_TRUE(aRead.More());
        ASSERT_EQ(aRead.IsValid(), aSecond < 0x80);
        ++aRead;
      }
      ASSERT_FALSE(aRead.More());
      ASSERT_EQ(aRead.BufferHere(), aData.data() + aData.size());
    }
  }
}

TEST(TCollection_UtfIteratorTest, CopiesBoundsAndFailedReset)
{
  const std::array<char, 4>     aData = {'A', 0, char(0xFF), 'B'};
  TCollection_UtfIterator<char> aRead(aData);
  const auto                    aCopy = aRead;
  TCollection_UtfIterator<char> aShort(aData.data(), 1);
  EXPECT_FALSE(aRead == aShort);
  EXPECT_THROW(aRead.Init(nullptr, 1), Standard_DomainError);
  EXPECT_TRUE(aRead == aCopy);
  EXPECT_EQ(*aRead++, U'A');
  EXPECT_EQ(*aRead, char32_t(0));
  EXPECT_TRUE(aRead.IsValid());
  EXPECT_EQ(*aCopy, U'A');
  ++aRead;
  EXPECT_FALSE(aRead.IsValid());
  const auto aBad = aRead++;
  EXPECT_FALSE(aBad.IsValid());
  EXPECT_EQ(*aRead, U'B');
  ++aRead;
  const auto anEnd = aRead++;
  EXPECT_TRUE(anEnd == aRead);
  std::array<char, 4> aOutput = {'x', 'x', 'x', 'x'};
  EXPECT_EQ(aRead.GetUtf8(aOutput), aOutput.data());
  EXPECT_EQ(aBad.GetUtf8(aOutput), aOutput.data());
  EXPECT_EQ(aOutput, (std::array<char, 4>{'x', 'x', 'x', 'x'}));
  aRead.Init(std::string_view(aData.data() + 3, 1));
  EXPECT_EQ(aRead.Index(), static_cast<size_t>(0));
  EXPECT_EQ(*aRead, U'B');
}

template <typename Unit>
class TCollection_UtfIteratorRangesTest : public testing::Test
{
};

using UtfInputUnits = testing::Types<unsigned char, char16_t, char32_t>;
TYPED_TEST_SUITE(TCollection_UtfIteratorRangesTest, UtfInputUnits);

TYPED_TEST(TCollection_UtfIteratorRangesTest, DeterministicExactSizedBuffers)
{
  uint32_t   aState    = 12345;
  const auto nextValue = [&aState]() {
    aState = aState * 1664525U + 1013904223U;
    return aState;
  };
  for (size_t aTrial = 0; aTrial < 25000; ++aTrial)
  {
    const size_t aLength = nextValue() % 9;
    auto         aData   = std::make_unique<TypeParam[]>(aLength);
    for (size_t i = 0; i < aLength; ++i)
    {
      aData[i] = static_cast<TypeParam>(nextValue() >> 8);
    }
    TCollection_UtfIterator<TypeParam> aRead(aData.get(), aLength);
    size_t                             aConsumed = 0;
    while (aRead.More())
    {
      const size_t aCount = aRead.CodeUnits();
      ASSERT_GT(aCount, static_cast<size_t>(0));
      ASSERT_LE(aCount, aLength - aConsumed);
      std::array<TypeParam, 4> aEncoded{};
      if (aRead.IsValid())
      {
        ASSERT_EQ(aRead.GetUtf(aEncoded), aEncoded.data() + aCount);
        for (size_t i = 0; i < aCount; ++i)
        {
          ASSERT_EQ(aEncoded[i], aData[aConsumed + i]);
        }
        std::array<char, 4> aOutput = {'x', 'x', 'x', 'x'};
        ASSERT_EQ(aRead.GetUtf8(aOutput.data(), aRead.AdvanceBytesUtf8() - 1), nullptr);
        ASSERT_EQ(aOutput, (std::array<char, 4>{'x', 'x', 'x', 'x'}));
      }
      else
      {
        ASSERT_EQ(aCount, static_cast<size_t>(1));
        ASSERT_EQ(aRead.GetUtf(aEncoded), aEncoded.data());
        ASSERT_EQ(aEncoded, (std::array<TypeParam, 4>{}));
      }
      aConsumed += aCount;
      ++aRead;
    }
    ASSERT_EQ(aConsumed, aLength);
    ++aRead;
    ASSERT_FALSE(aRead.More());
  }
}

TEST(TCollection_UtfIteratorTest, DecodeAndEncodeUtf16)
{
  constexpr char16_t aText[]    = {0x0041, 0xD83D, 0xDE00, 0};
  char16_t           aBuffer[2] = {};

  TCollection_UtfIterator<char16_t> anIterator(aText);
  EXPECT_TRUE(anIterator.IsValid());
  EXPECT_EQ(char32_t(0x41), *anIterator);
  EXPECT_EQ(1, anIterator.AdvanceCodeUnitsUtf16());

  ++anIterator;
  EXPECT_TRUE(anIterator.IsValid());
  EXPECT_EQ(char32_t(0x1F600), *anIterator);
  EXPECT_EQ(2, anIterator.AdvanceCodeUnitsUtf16());
  EXPECT_EQ(aBuffer + 2, anIterator.GetUtf16(aBuffer));
  EXPECT_EQ(aText[1], aBuffer[0]);
  EXPECT_EQ(aText[2], aBuffer[1]);
}

TEST(TCollection_UtfIteratorTest, RejectMalformedUtf8)
{
  const auto checkInvalid = [](const char* theText, int theConsumedBytes) {
    auto anIterator =
      TCollection_UtfIterator<char>(theText,
                                    TCollection_UtfIterator<char>::InputMode::NullTerminated);
    EXPECT_FALSE(anIterator.IsValid());
    EXPECT_EQ(0, anIterator.AdvanceBytesUtf8());
    EXPECT_EQ(theText + theConsumedBytes, anIterator.BufferNext());
  };

  constexpr char anUnexpectedTail[] = {'\x80', 0};
  constexpr char anOverlong[]       = {'\xC0', '\x80', 0};
  constexpr char aTruncated[]       = {'\xE2', 0};
  constexpr char aBadTail[]         = {'\xE2', 'A', 0};
  constexpr char anOutOfRange[]     = {'\xF4', '\x90', '\x80', '\x80', 0};
  constexpr char aSurrogate[]       = {'\xED', '\xA0', '\x80', 0};
  constexpr char anObsoleteLead[]   = {'\xF8', '\x88', '\x80', '\x80', '\x80', 0};

  checkInvalid(anUnexpectedTail, 1);
  checkInvalid(anOverlong, 1);
  checkInvalid(aTruncated, 1);
  checkInvalid(aBadTail, 1);
  checkInvalid(anOutOfRange, 1);
  checkInvalid(aSurrogate, 1);
  checkInvalid(anObsoleteLead, 1);
}

TEST(TCollection_UtfIteratorTest, MalformedLegacyBytes_DoNotConsumeFollowingAscii)
{
  const char aInput[] = {'\xED', 'h', '\xE1', ' ', '\xD6', 'l', '\xE7', 'e', 'k', 0};
  using Iterator      = TCollection_UtfIterator<char>;
  for (auto anIter : {Iterator(aInput, sizeof(aInput) - 1),
                      Iterator(aInput, Iterator::InputMode::NullTerminated)})
  {
    for (size_t anIndex = 0; anIndex < sizeof(aInput) - 1; ++anIndex, ++anIter)
    {
      ASSERT_TRUE(anIter.More());
      EXPECT_EQ(anIter.BufferHere(), aInput + anIndex);
      EXPECT_EQ(anIter.BufferNext(), aInput + anIndex + 1);
      if (static_cast<unsigned char>(aInput[anIndex]) < 0x80)
      {
        EXPECT_TRUE(anIter.IsValid());
        EXPECT_EQ(*anIter, static_cast<char32_t>(aInput[anIndex]));
      }
      else
      {
        EXPECT_FALSE(anIter.IsValid());
        EXPECT_EQ(anIter.AdvanceCodeUnitsUtf16(), 0);
      }
    }
    EXPECT_FALSE(anIter.More());
  }
}

TEST(TCollection_UtfIteratorTest, BoundedUtf8_DoesNotUseContinuationOutsideRange)
{
  const char                    aSource[] = {'\xE2', '\x82', '\xAC'};
  TCollection_UtfIterator<char> aTruncated(aSource, 2);
  EXPECT_FALSE(aTruncated.IsValid());
  EXPECT_EQ(aTruncated.BufferNext(), aSource + 1);
  ++aTruncated;
  EXPECT_FALSE(aTruncated.IsValid());
  EXPECT_EQ(aTruncated.BufferNext(), aSource + 2);
  ++aTruncated;
  EXPECT_FALSE(aTruncated.More());

  TCollection_UtfIterator<char> aComplete(aSource, sizeof(aSource));
  EXPECT_TRUE(aComplete.IsValid());
  EXPECT_EQ(*aComplete, U'\u20AC');
  EXPECT_EQ(aComplete.BufferNext(), aSource + sizeof(aSource));
  ++aComplete;
  EXPECT_FALSE(aComplete.More());
}

TEST(TCollection_UtfIteratorTest, RejectNonScalarUtf16AndUtf32)
{
  constexpr char16_t aHighOnly[] = {0xD800, 0};
  constexpr char16_t aLowOnly[]  = {0xDC00, 0};
  EXPECT_FALSE(TCollection_UtfIterator<char16_t>(aHighOnly).IsValid());
  EXPECT_FALSE(TCollection_UtfIterator<char16_t>(aLowOnly).IsValid());

  constexpr char32_t aSurrogate[] = {0xDFFF, 0};
  constexpr char32_t aTooLarge[]  = {0x110000, 0};
  EXPECT_FALSE(TCollection_UtfIterator<char32_t>(aSurrogate).IsValid());
  EXPECT_FALSE(TCollection_UtfIterator<char32_t>(aTooLarge).IsValid());
}

// Original specification tests: RFC 3629 Sections 3-4 and RFC 2781 Section 2.
// Initial version authored and frozen before inspection of the existing OCCT tests.
namespace
{
void checkBoundary(char32_t                             theScalar,
                   std::initializer_list<unsigned char> theUtf8,
                   std::initializer_list<char16_t>      theUtf16)
{
  const std::array<char32_t, 2>     aSource = {theScalar, 0};
  TCollection_UtfIterator<char32_t> anIter(aSource);
  std::array<unsigned char, 5>      aUtf8  = {};
  std::array<char16_t, 3>           aUtf16 = {};
  std::array<char32_t, 2>           aUtf32 = {};
  ASSERT_TRUE(anIter.IsValid());
  EXPECT_EQ(anIter.GetUtf8(aUtf8), aUtf8.data() + theUtf8.size());
  EXPECT_EQ(anIter.GetUtf16(aUtf16), aUtf16.data() + theUtf16.size());
  EXPECT_EQ(anIter.GetUtf32(aUtf32), aUtf32.data() + 1);
  EXPECT_EQ(aUtf32[0], theScalar);
  EXPECT_TRUE(std::equal(theUtf8.begin(), theUtf8.end(), aUtf8.data()));
  EXPECT_TRUE(std::equal(theUtf16.begin(), theUtf16.end(), aUtf16.data()));
  EXPECT_EQ(anIter.AdvanceBytesUtf8(), int(theUtf8.size()));
  EXPECT_EQ(anIter.AdvanceCodeUnitsUtf16(), int(theUtf16.size()));
  EXPECT_EQ(anIter.AdvanceBytesUtf16(), int(theUtf16.size() * sizeof(char16_t)));
  TCollection_UtfIterator<unsigned char> aRead8(aUtf8);
  TCollection_UtfIterator<char16_t>      aRead16(aUtf16);
  EXPECT_TRUE(aRead8.IsValid());
  EXPECT_TRUE(aRead16.IsValid());
  EXPECT_EQ(*aRead8, theScalar);
  EXPECT_EQ(*aRead16, theScalar);
  EXPECT_EQ(aRead8.BufferNext(), aUtf8.data() + theUtf8.size());
  EXPECT_EQ(aRead16.BufferNext(), aUtf16.data() + theUtf16.size());
}

template <typename Unit>
void checkInvalid(const Unit* theInput, size_t theCount)
{
  TCollection_UtfIterator<Unit> anIter(theInput, theCount);
  ASSERT_FALSE(anIter.IsValid());
  EXPECT_EQ(anIter.BufferNext(), theInput + 1);
  EXPECT_EQ(anIter.AdvanceBytesUtf8(), 0);
  EXPECT_EQ(anIter.AdvanceBytesUtf16(), 0);
  EXPECT_EQ(anIter.AdvanceCodeUnitsUtf16(), 0);
  unsigned char aUtf8[]  = {0x55, 0x55, 0x55, 0x55};
  char16_t      aUtf16[] = {0x5555, 0x5555};
  char32_t      aUtf32[] = {0x55555};
  EXPECT_EQ(anIter.GetUtf8(aUtf8), aUtf8);
  EXPECT_EQ(anIter.GetUtf16(aUtf16), aUtf16);
  EXPECT_EQ(anIter.GetUtf32(aUtf32), aUtf32);
  for (auto aUnit : aUtf8)
  {
    EXPECT_EQ(aUnit, 0x55);
  }
  for (auto aUnit : aUtf16)
  {
    EXPECT_EQ(aUnit, 0x5555);
  }
  EXPECT_EQ(aUtf32[0], char32_t(0x55555));
  EXPECT_EQ(anIter.GetUtf8(static_cast<char*>(nullptr), 0), nullptr);
  EXPECT_EQ(anIter.GetUtf16(nullptr, 0), nullptr);
  EXPECT_EQ(anIter.GetUtf32(nullptr, 0), nullptr);
  EXPECT_EQ(anIter.template GetUtf<wchar_t>(nullptr, 0), nullptr);
}

template <typename Unit, size_t N>
void checkInvalid(const Unit (&theInput)[N])
{
  checkInvalid(theInput, N);
}
} // namespace

TEST(TCollection_UtfIteratorTest, EncodingBoundaries)
{
  checkBoundary(0, {0}, {0});
  checkBoundary(0x7F, {0x7F}, {0x7F});
  checkBoundary(0x80, {0xC2, 0x80}, {0x80});
  checkBoundary(0x7FF, {0xDF, 0xBF}, {0x7FF});
  checkBoundary(0x800, {0xE0, 0xA0, 0x80}, {0x800});
  checkBoundary(0xD7FF, {0xED, 0x9F, 0xBF}, {0xD7FF});
  checkBoundary(0xE000, {0xEE, 0x80, 0x80}, {0xE000});
  checkBoundary(0xFFFF, {0xEF, 0xBF, 0xBF}, {0xFFFF});
  checkBoundary(0x10000, {0xF0, 0x90, 0x80, 0x80}, {0xD800, 0xDC00});
  checkBoundary(0x103FF, {0xF0, 0x90, 0x8F, 0xBF}, {0xD800, 0xDFFF});
  checkBoundary(0x10400, {0xF0, 0x90, 0x90, 0x80}, {0xD801, 0xDC00});
  checkBoundary(0x10FC00, {0xF4, 0x8F, 0xB0, 0x80}, {0xDBFF, 0xDC00});
  checkBoundary(0x10FFFF, {0xF4, 0x8F, 0xBF, 0xBF}, {0xDBFF, 0xDFFF});
}

TEST(TCollection_UtfIteratorTest, InvalidUtf8Forms)
{
  const unsigned char aCases[][7] = {{0xC0, 0x80},
                                     {0xC1, 0xBF},
                                     {0xE0, 0x80, 0x80},
                                     {0xE0, 0x9F, 0xBF},
                                     {0xF0, 0x80, 0x80, 0x80},
                                     {0xF0, 0x8F, 0xBF, 0xBF},
                                     {0xED, 0xA0, 0x80},
                                     {0xED, 0xBF, 0xBF},
                                     {0xF4, 0x90, 0x80, 0x80},
                                     {0xF7, 0xBF, 0xBF, 0xBF},
                                     {0xF8, 0x88, 0x80, 0x80, 0x80},
                                     {0xFC, 0x84, 0x80, 0x80, 0x80, 0x80},
                                     {0xFE},
                                     {0xFF}};
  for (const auto& aCase : aCases)
  {
    checkInvalid(aCase);
  }
  for (uint32_t aLead = 0x80; aLead <= 0xFF; ++aLead)
  {
    if (aLead >= 0xC2 && aLead <= 0xF4)
    {
      continue;
    }
    const unsigned char aSource[] = {static_cast<unsigned char>(aLead), 0};
    checkInvalid(aSource);
  }
}

TEST(TCollection_UtfIteratorTest, TruncatedUtf8AtAllocationEnd)
{
  const unsigned char aForms[][4] = {{0xC2, 0x80}, {0xE0, 0xA0, 0x80}, {0xF0, 0x90, 0x80, 0x80}};
  for (int aForm = 0; aForm < 3; ++aForm)
  {
    for (int aLength = 1; aLength < aForm + 2; ++aLength)
    {
      // The null is the last allocated byte, so ASan detects lookahead past it.
      std::unique_ptr<unsigned char[]> aSource(new unsigned char[aLength + 1]);
      std::copy_n(aForms[aForm], aLength, aSource.get());
      aSource[aLength] = 0;
      checkInvalid(aSource.get(), static_cast<size_t>(aLength + 1));
      TCollection_UtfIterator<unsigned char> anIter(aSource.get(),
                                                    static_cast<size_t>(aLength + 1));
      for (int anIndex = 0; anIndex < aLength; ++anIndex, ++anIter)
      {
        EXPECT_FALSE(anIter.IsValid());
        EXPECT_EQ(anIter.BufferHere(), aSource.get() + anIndex);
      }
      EXPECT_EQ(*anIter, char32_t(0));
      EXPECT_EQ(anIter.BufferHere(), aSource.get() + aLength);
    }
  }
}

TEST(TCollection_UtfIteratorTest, MalformedContinuationAndRecovery)
{
  const unsigned char aForms[][5] = {{0xC2, 0x80}, {0xE0, 0xA0, 0x80}, {0xF0, 0x90, 0x80, 0x80}};
  for (int aForm = 0; aForm < 3; ++aForm)
  {
    for (int aPosition = 1; aPosition < aForm + 2; ++aPosition)
    {
      for (uint32_t aBad = 0; aBad <= 0xFF; ++aBad)
      {
        if (aBad >= 0x80 && aBad <= 0xBF)
        {
          continue;
        }
        unsigned char aSource[5];
        std::copy_n(aForms[aForm], 5, aSource);
        aSource[aPosition] = static_cast<unsigned char>(aBad);
        checkInvalid(aSource);
      }
    }
  }
  const unsigned char                    aSource[] = {0xE2, 0x82, 'A', 0xC2, 0x80, 0};
  TCollection_UtfIterator<unsigned char> anIter(aSource);
  EXPECT_FALSE(anIter.IsValid());
  EXPECT_FALSE((++anIter).IsValid());
  EXPECT_EQ(*++anIter, U'A');
  EXPECT_EQ(*++anIter, char32_t(0x80));
  EXPECT_EQ(*++anIter, char32_t(0));
  EXPECT_EQ(anIter.Index(), 4);
}

TEST(TCollection_UtfIteratorTest, IsolatedUtf16Surrogates)
{
  for (uint32_t aValue = 0xD800; aValue <= 0xDFFF; ++aValue)
  {
    std::unique_ptr<char16_t[]> aSource(new char16_t[2]{char16_t(aValue), 0});
    checkInvalid(aSource.get(), 2);
    TCollection_UtfIterator<char16_t> anIter(aSource.get(), 2);
    EXPECT_EQ(*++anIter, char32_t(0));
  }
  constexpr char16_t                aSource[] = {0xD800, 'A', 0xD800, 0xDBFF, 0xDFFF, 0};
  TCollection_UtfIterator<char16_t> anIter(aSource);
  EXPECT_FALSE(anIter.IsValid());
  EXPECT_EQ(*++anIter, U'A');
  EXPECT_FALSE((++anIter).IsValid());
  EXPECT_EQ(*++anIter, char32_t(0x10FFFF));
  EXPECT_EQ(*++anIter, char32_t(0));
}

TEST(TCollection_UtfIteratorTest, InvalidUtf32Encoding)
{
  for (char32_t aValue = 0xD800; aValue <= 0xDFFF; ++aValue)
  {
    const char32_t aSource[] = {aValue, 0};
    checkInvalid(aSource);
    EXPECT_EQ(*TCollection_UtfIterator<char32_t>(aSource), aValue);
  }
  for (char32_t aValue : {char32_t(0x110000), char32_t(0x7FFFFFFF), char32_t(-1)})
  {
    const char32_t aSource[] = {aValue, 0};
    checkInvalid(aSource);
    EXPECT_EQ(*TCollection_UtfIterator<char32_t>(aSource), aValue);
  }
}

TEST(TCollection_UtfIteratorTest, RoundTripAllScalarValues)
{
  for (char32_t aValue = 0; aValue <= 0x10FFFF; ++aValue)
  {
    if (aValue >= 0xD800 && aValue <= 0xDFFF)
    {
      continue;
    }
    const std::array<char32_t, 2>     aSource = {aValue, 0};
    TCollection_UtfIterator<char32_t> anIter(aSource);
    std::array<char, 5>               aUtf8  = {};
    std::array<char16_t, 3>           aUtf16 = {};
    std::array<char32_t, 2>           aUtf32 = {};
    ASSERT_EQ(anIter.GetUtf8(aUtf8), aUtf8.data() + anIter.AdvanceBytesUtf8());
    ASSERT_EQ(anIter.GetUtf16(aUtf16), aUtf16.data() + anIter.AdvanceCodeUnitsUtf16());
    TCollection_UtfIterator<char>     aRead8(aUtf8);
    TCollection_UtfIterator<char16_t> aRead16(aUtf16);
    ASSERT_TRUE(aRead8.IsValid()) << static_cast<uint32_t>(aValue);
    ASSERT_TRUE(aRead16.IsValid()) << static_cast<uint32_t>(aValue);
    ASSERT_EQ(*aRead8, aValue);
    ASSERT_EQ(*aRead16, aValue);
    ASSERT_EQ(aRead8.BufferNext(), aUtf8.data() + anIter.AdvanceBytesUtf8());
    ASSERT_EQ(aRead16.BufferNext(), aUtf16.data() + anIter.AdvanceCodeUnitsUtf16());
    ASSERT_EQ(aRead8.GetUtf32(aUtf32), aUtf32.data() + 1);
    ASSERT_EQ(aUtf32[0], aValue);
    ASSERT_EQ(aRead16.GetUtf32(aUtf32), aUtf32.data() + 1);
    ASSERT_EQ(aUtf32[0], aValue);
    std::array<char, 5>     aCross8  = {};
    std::array<char16_t, 3> aCross16 = {};
    ASSERT_EQ(aRead16.GetUtf8(aCross8), aCross8.data() + anIter.AdvanceBytesUtf8());
    ASSERT_EQ(aRead8.GetUtf16(aCross16), aCross16.data() + anIter.AdvanceCodeUnitsUtf16());
    ASSERT_EQ(aUtf8, aCross8);
    ASSERT_EQ(aUtf16, aCross16);
  }
}

TEST(TCollection_UtfIteratorTest, GenericTypesAndIteratorState)
{
  constexpr std::array<char32_t, 2> aSource = {0x10400, 0};
  TCollection_UtfIterator<char32_t> anIter(aSource);
  std::array<wchar_t, 3>            aWide     = {};
  std::array<unsigned short, 3>     aShort    = {};
  std::array<uint32_t, 2>           aInt      = {};
  std::array<signed char, 5>        aSigned   = {};
  std::array<unsigned char, 5>      aUnsigned = {};
  EXPECT_EQ(anIter.GetUtf(aWide), aWide.data() + (sizeof(wchar_t) == 2 ? 2 : 1));
  EXPECT_EQ(anIter.GetUtf(aShort), aShort.data() + 2);
  EXPECT_EQ(anIter.GetUtf(aInt), aInt.data() + 1);
  EXPECT_EQ(anIter.GetUtf(aSigned), aSigned.data() + 4);
  EXPECT_EQ(anIter.GetUtf(aUnsigned), aUnsigned.data() + 4);
  EXPECT_EQ(*TCollection_UtfIterator<wchar_t>(aWide), aSource[0]);
  EXPECT_EQ(*TCollection_UtfIterator<unsigned short>(aShort), aSource[0]);
  EXPECT_EQ(*TCollection_UtfIterator<uint32_t>(aInt), aSource[0]);
  EXPECT_EQ(*TCollection_UtfIterator<signed char>(aSigned), aSource[0]);
  EXPECT_EQ(*TCollection_UtfIterator<unsigned char>(aUnsigned), aSource[0]);
  EXPECT_EQ(anIter.AdvanceBytesUtf<char>(), 4);
  EXPECT_EQ(anIter.AdvanceBytesUtf<char16_t>(), 4);
  EXPECT_EQ(anIter.AdvanceBytesUtf<char32_t>(), 4);

  TCollection_UtfIterator<char> aNull;
  EXPECT_EQ(*aNull, char32_t(0));
  EXPECT_EQ(aNull.BufferHere(), nullptr);
  EXPECT_EQ(aNull.Index(), 0);
  constexpr std::array<char, 3> aText = {'A', 'B', 0};
  aNull.Init(aText.data(), aText.size());
  auto aPrevious = aNull++;
  EXPECT_EQ(*aPrevious, U'A');
  EXPECT_EQ(*aNull, U'B');
  EXPECT_EQ(aNull.Index(), 1);
  aNull.Init(aText.data(), aText.size());
  EXPECT_TRUE(aNull == aPrevious);
  EXPECT_EQ(aNull.BufferHere(), aText.data());
  aNull.Init(nullptr, 0);
  EXPECT_EQ(aNull.BufferNext(), nullptr);
  EXPECT_EQ(aNull.Index(), 0);
}

TEST(TCollection_UtfIteratorTest, ConstexprConversion)
{
  constexpr bool isCorrect = [] {
    constexpr std::array<unsigned char, 5> aSource = {0xF0, 0x90, 0x80, 0x80, 0};
    TCollection_UtfIterator<unsigned char> anIter(aSource);
    if (!anIter.IsValid() || *anIter != 0x10000 || anIter.AdvanceBytesUtf8() != 4
        || anIter.AdvanceBytesUtf<char16_t>() != 4)
    {
      return false;
    }
    std::array<char16_t, 3> aUtf16{};
    if (anIter.GetUtf16(aUtf16) != aUtf16.data() + 2 || aUtf16[0] != 0xD800 || aUtf16[1] != 0xDC00)
    {
      return false;
    }
    TCollection_UtfIterator<char16_t> aRead16(aUtf16);
    std::array<unsigned char, 4>      aUtf8{};
    std::array<char, 4>               aChars{};
    std::array<signed char, 4>        aSigned{};
    std::array<uint32_t, 1>           aUtf32{};
    if (*aRead16 != 0x10000 || aRead16.GetUtf8(aUtf8) != aUtf8.data() + 4
        || aRead16.GetUtf8(aChars) != aChars.data() + 4
        || aRead16.GetUtf(aSigned) != aSigned.data() + 4
        || aRead16.GetUtf(aUtf32) != aUtf32.data() + 1 || aUtf32[0] != 0x10000)
    {
      return false;
    }
    for (std::size_t anIndex = 0; anIndex < aUtf8.size(); ++anIndex)
    {
      if (aUtf8[anIndex] != aSource[anIndex]
          || static_cast<unsigned char>(aChars[anIndex]) != aSource[anIndex]
          || static_cast<unsigned char>(aSigned[anIndex]) != aSource[anIndex])
      {
        return false;
      }
    }
    constexpr std::array<unsigned char, 2> aInvalid = {0xFF, 0};
    anIter.Init(aInvalid.data(), aInvalid.size());
    if (anIter.IsValid() || anIter.GetUtf16(aUtf16) != aUtf16.data())
    {
      return false;
    }
    const auto aPrevious = anIter++;
    return !aPrevious.IsValid() && *anIter == 0 && anIter.Index() == 1;
  }();
  static_assert(isCorrect, "UTF conversion must support C++17 constant evaluation");
  EXPECT_TRUE(isCorrect);
}

TEST(TCollection_UtfIteratorTest, BoundedRangesAndEndState)
{
  const std::array<unsigned char, 4>     aSource = {0xF0, 0x90, 0x80, 0x80};
  TCollection_UtfIterator<unsigned char> anIter(aSource);
  EXPECT_EQ(*anIter, char32_t(0x10000));
  EXPECT_TRUE(anIter.More());
  ++anIter;
  EXPECT_FALSE(anIter.More());
  EXPECT_FALSE(anIter.IsValid());
  EXPECT_EQ(anIter.BufferHere(), aSource.data() + aSource.size());
  ++anIter;
  EXPECT_EQ(anIter.Index(), static_cast<size_t>(1));
  EXPECT_EQ(anIter.AdvanceBytesUtf32(), static_cast<size_t>(0));

  // Each truncated buffer ends at its allocation boundary, without a null sentinel.
  for (size_t aSize = 1; aSize < aSource.size(); ++aSize)
  {
    auto aData = std::make_unique<unsigned char[]>(aSize);
    std::copy_n(aSource.data(), aSize, aData.get());
    TCollection_UtfIterator<unsigned char> aRead(aData.get(), aSize);
    size_t                                 aVisited = 0;
    while (aRead.More())
    {
      EXPECT_FALSE(aRead.IsValid());
      ++aRead;
      ++aVisited;
    }
    EXPECT_EQ(aVisited, aSize);
  }
  const std::array<char16_t, 1> aHigh = {0xD800};
  EXPECT_FALSE(TCollection_UtfIterator<char16_t>(aHigh).IsValid());
  const std::string_view        aEmbedded("a\0b", 3);
  TCollection_UtfIterator<char> aRead(aEmbedded);
  EXPECT_EQ(*aRead, U'a');
  EXPECT_EQ(*++aRead, char32_t(0));
  EXPECT_TRUE(aRead.More());
  EXPECT_TRUE(aRead.IsValid());
  EXPECT_EQ(*++aRead, U'b');
  EXPECT_FALSE((++aRead).More());
  EXPECT_THROW((TCollection_UtfIterator<char>(nullptr, 1)), Standard_DomainError);
}

TEST(TCollection_UtfIteratorTest, ExplicitNullTerminatedInput)
{
  using Utf8 = TCollection_UtfIterator<char>;
  constexpr Utf8 anEmpty(nullptr, Utf8::InputMode::NullTerminated);
  static_assert(!anEmpty.More(), "Null input is empty");
  constexpr Utf8 aConstant("A", Utf8::InputMode::NullTerminated);
  static_assert(aConstant.More() && *aConstant == U'A', "Tagged input supports constexpr");

  const std::array<char, 4> aSource = {'A', 0, 'B', 0};
  Utf8                      aRead(aSource.data(), Utf8::InputMode::NullTerminated);
  EXPECT_EQ(*aRead, U'A');
  EXPECT_FALSE((++aRead).More());
  EXPECT_EQ(aRead.BufferHere(), aSource.data() + 1);
  EXPECT_FALSE(Utf8("", Utf8::InputMode::NullTerminated).More());

  // The terminator is excluded from decoding even after a truncated prefix.
  const std::array<char, 2> aTruncated = {char(0xE2), 0};
  Utf8                      aBad(aTruncated.data(), Utf8::InputMode::NullTerminated);
  EXPECT_FALSE(aBad.IsValid());
  EXPECT_FALSE((++aBad).More());

  using Utf16 = TCollection_UtfIterator<char16_t>;
  Utf16 aPair(u"\U00010000", Utf16::InputMode::NullTerminated);
  EXPECT_EQ(*aPair, char32_t(0x10000));
  EXPECT_FALSE((++aPair).More());
  using Utf32 = TCollection_UtfIterator<char32_t>;
  Utf32 aScalar(U"\U0010FFFF", Utf32::InputMode::NullTerminated);
  EXPECT_EQ(*aScalar, char32_t(0x10FFFF));
  EXPECT_FALSE((++aScalar).More());
}

TEST(TCollection_UtfIteratorTest, CheckedOutputCapacity)
{
  const std::array<char32_t, 1>     aSource = {0x10000};
  TCollection_UtfIterator<char32_t> anIter(aSource);
  for (size_t aCapacity = 0; aCapacity < 4; ++aCapacity)
  {
    std::array<char, 4> aOutput = {'x', 'x', 'x', 'x'};
    EXPECT_EQ(anIter.GetUtf8(aOutput.data(), aCapacity), nullptr);
    EXPECT_EQ(aOutput, (std::array<char, 4>{'x', 'x', 'x', 'x'}));
  }
  std::array<char16_t, 2> aUtf16 = {1, 2};
  EXPECT_EQ(anIter.GetUtf16(aUtf16.data(), 1), nullptr);
  EXPECT_EQ(aUtf16, (std::array<char16_t, 2>{1, 2}));
  EXPECT_EQ(anIter.GetUtf16(aUtf16), aUtf16.data() + 2);
  std::array<uint32_t, 1> aUtf32 = {7};
  EXPECT_EQ(anIter.GetUtf(aUtf32.data(), 0), nullptr);
  EXPECT_EQ(aUtf32[0], uint32_t(7));
  EXPECT_EQ(anIter.GetUtf(aUtf32), aUtf32.data() + 1);
  EXPECT_EQ(anIter.GetUtf8(static_cast<char*>(nullptr), 4), nullptr);
}
