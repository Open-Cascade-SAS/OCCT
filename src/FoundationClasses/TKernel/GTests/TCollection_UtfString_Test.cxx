// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <TCollection_UtfString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_UtfIterator.hxx>
#include <NCollection_UtfString.hxx>
#include <NCollection_String.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <gtest/gtest.h>

#include <array>
#include <clocale>
#include <string_view>
#include <type_traits>

namespace
{
// Locale-changing tests run sequentially and restore the process setting on every exit.
class LocaleGuard
{
public:
  LocaleGuard()
      : myPrevious(std::setlocale(LC_CTYPE, nullptr))
  {
  }

  ~LocaleGuard() { std::setlocale(LC_CTYPE, myPrevious.c_str()); }

  bool SetUtf8()
  {
    return std::setlocale(LC_CTYPE, "en_US.UTF-8") != nullptr
           || std::setlocale(LC_CTYPE, "C.UTF-8") != nullptr;
  }

private:
  std::string myPrevious;
};
} // namespace

static_assert(std::is_same_v<NCollection_UtfIterator<char>, TCollection_UtfIterator<char>>);
static_assert(std::is_same_v<NCollection_UtfString<char16_t>, TCollection_UtfString<char16_t>>);
static_assert(std::is_same_v<NCollection_String, TCollection_UtfString<char>>);

TEST(TCollection_UtfStringTest, ExtendedStringInteroperability)
{
  // ExtendedString indexes code units; UtfString indexes validated scalars.
  const char16_t                        aSource[] = {u'A', 0xD800, 0xDC00, 0, u'B'};
  const TCollection_UtfString<char16_t> aUnicode(std::u16string_view(aSource, 5));
  const TCollection_ExtendedString      aExtended(aSource, 5);
  EXPECT_EQ(aExtended.Length(), 5);
  EXPECT_EQ(aUnicode.Length(), static_cast<size_t>(4));
  EXPECT_EQ(aUnicode[1], char32_t(0x10000));
  const auto aView =
    std::u16string_view(aExtended.ToExtString(), static_cast<size_t>(aExtended.Length()));
  EXPECT_EQ(TCollection_UtfString<char16_t>(aView), aUnicode);

  // A legacy code-unit mutation can break a pair; validated import must reject it.
  TCollection_ExtendedString aBroken(aExtended);
  aBroken.SetValue(3, u'x');
  TCollection_UtfString<char16_t> aResult(u"unchanged");
  EXPECT_FALSE(aResult.FromUnicode(aBroken.ToExtString(), static_cast<size_t>(aBroken.Length())));
  EXPECT_EQ(aResult.View(), u"unchanged");
}

TEST(TCollection_UtfStringTest, ExtendedStringInvalidUtf8Fallback)
{
  const struct
  {
    const char*     Name;
    const char*     Input;
    const char16_t* Expected;
  } aCases[] = {{"Czech", "Test Prob\xEDh\xE1", u"Test Prob\u00EDh\u00E1"},
                {"Turkish",
                 "Test \xD6l\xE7"
                 "ek",
                 u"Test \u00D6l\u00E7ek"},
                {"UnexpectedContinuation", "A\x80Z", u"A\u0080Z"},
                {"Overlong", "\xC0\xAF", u"\u00C0\u00AF"},
                {"TruncatedTwoBytes", "A\xC2", u"A\u00C2"},
                {"TruncatedThreeBytes", "A\xE2\x82", u"A\u00E2\u0082"},
                {"TruncatedFourBytes", "A\xF0\x9F\x98", u"A\u00F0\u009F\u0098"},
                {"Surrogate", "\xED\xA0\x80", u"\u00ED\u00A0\u0080"},
                {"AboveUnicodeLimit", "\xF4\x90\x80\x80", u"\u00F4\u0090\u0080\u0080"},
                {"ValidPrefixInvalidTail", "\xC3\xA9\xFF", u"\u00C3\u00A9\u00FF"},
                {"InvalidPrefixValidTail", "\xFF\xC3\xA9", u"\u00FF\u00C3\u00A9"}};

  for (const auto& aCase : aCases)
  {
    const TCollection_ExtendedString aExpected(aCase.Expected);
    EXPECT_EQ(TCollection_ExtendedString(aCase.Input, true), aExpected) << aCase.Name;
    EXPECT_EQ(TCollection_ExtendedString(TCollection_AsciiString(aCase.Input), true), aExpected)
      << aCase.Name;

    const auto checkStrict = [&aCase](auto& theString) {
      const auto anOriginal = theString;
      EXPECT_FALSE(theString.FromUnicode(aCase.Input)) << aCase.Name;
      EXPECT_EQ(theString, anOriginal) << aCase.Name;
      EXPECT_FALSE(theString.FromUnicode(std::string_view(aCase.Input))) << aCase.Name;
      EXPECT_EQ(theString, anOriginal) << aCase.Name;
    };
    TCollection_UtfString<char>     aUtf8(U"keep\U0001F600");
    TCollection_UtfString<char16_t> aUtf16(U"keep\U0001F600");
    TCollection_UtfString<char32_t> aUtf32(U"keep\U0001F600");
    checkStrict(aUtf8);
    checkStrict(aUtf16);
    checkStrict(aUtf32);
  }
}

TEST(TCollection_UtfStringTest, ExtendedStringValidUtf8_DecodesWithoutFallback)
{
  const char*                      anInput = "A\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80";
  const TCollection_ExtendedString anExpected(u"A\u00E9\u20AC\U0001F600");
  EXPECT_EQ(TCollection_ExtendedString(anInput, true), anExpected);
  EXPECT_EQ(TCollection_ExtendedString(TCollection_AsciiString(anInput), true), anExpected);
  TCollection_UtfString<char16_t> aStrict;
  ASSERT_TRUE(aStrict.FromUnicode(anInput));
  EXPECT_EQ(aStrict.View(), std::u16string_view(anExpected));
}

TEST(TCollection_UtfStringTest, ExtendedStringAliasedViews)
{
  const std::u16string       aSource(4096, u'A');
  TCollection_ExtendedString aString{std::u16string_view(aSource)};
  const std::u16string_view  aSelf = aString;
  aString.AssignCat(aSelf);
  EXPECT_EQ(aString.Length(), 8192);
  EXPECT_EQ(std::u16string_view(aString), std::u16string_view(aSource + aSource));
  const std::u16string_view aFull = aString;
  aString                         = aFull.substr(1024, 4096);
  EXPECT_EQ(std::u16string_view(aString), std::u16string_view(aSource));
  aString = std::u16string_view();
  EXPECT_EQ(aString.Length(), 0);
}

TEST(TCollection_UtfStringTest, BoundedInputAndEmbeddedNulls)
{
  const std::array<char, 6>   aInput = {'A', 0, char(0xF0), char(0x90), char(0x80), char(0x80)};
  TCollection_UtfString<char> aString(std::string_view(aInput.data(), aInput.size()));
  EXPECT_EQ(aString.Length(), static_cast<size_t>(3));
  EXPECT_EQ(aString.CodeUnits(), aInput.size());
  EXPECT_EQ(aString.Size(), aInput.size());
  EXPECT_EQ(aString[1], char32_t(0));
  EXPECT_EQ(aString[2], char32_t(0x10000));
  const auto aUtf16 = aString.ToUtf16();
  EXPECT_EQ(aUtf16.CodeUnits(), static_cast<size_t>(4));
  EXPECT_EQ(aUtf16.Length(), static_cast<size_t>(3));
  EXPECT_EQ(aUtf16.Size(), static_cast<size_t>(8));
  EXPECT_EQ(aUtf16.ToUtf32().ToUtf8(), aString);
  EXPECT_EQ(aString.ToUtfWide().ToUtf8(), aString);
  EXPECT_EQ(aString.ToCString()[aString.CodeUnits()], char(0));
  auto anIter = aString.Iterator();
  for (size_t i = 0; i < aString.Length(); ++i, ++anIter)
  {
    ASSERT_TRUE(anIter.More());
    EXPECT_EQ(*anIter, aString[i]);
  }
  EXPECT_FALSE(anIter.More());
}

template <typename Unit>
class TCollection_UtfStringGenericTest : public testing::Test
{
};

using UtfStringUnits = testing::Types<char, char16_t, char32_t, wchar_t>;
TYPED_TEST_SUITE(TCollection_UtfStringGenericTest, UtfStringUnits);

TYPED_TEST(TCollection_UtfStringGenericTest, CrossEncodingAndEverySubstring)
{
  // Boundary scalars, embedded nulls, combining marks and valid noncharacters.
  const std::array<char32_t, 14> aSource =
    {0, 'A', 0x7F, 0x80, 0x7FF, 0x800, 0xD7FF, 0xE000, 0xFFFF, 0x10000, 0x10FFFF, 'e', 0x301, 0};
  const std::u32string_view              aReference(aSource.data(), aSource.size());
  const TCollection_UtfString<TypeParam> aString(aReference);
  EXPECT_EQ(aString.Length(), aSource.size());
  EXPECT_EQ(aString.ToUtf8().ToUtf32().View(), aReference);
  EXPECT_EQ(aString.ToUtf16().ToUtf32().View(), aReference);
  EXPECT_EQ(aString.ToUtfWide().ToUtf32().View(), aReference);
  for (size_t aStart = 0; aStart <= aSource.size(); ++aStart)
  {
    for (size_t anEnd = aStart; anEnd <= aSource.size(); ++anEnd)
    {
      const auto aPart = aString.SubString(aStart, anEnd);
      EXPECT_EQ(aPart.Length(), anEnd - aStart);
      EXPECT_EQ(aPart.ToUtf32().View(), aReference.substr(aStart, anEnd - aStart));
      EXPECT_EQ(aPart.ToCString()[aPart.CodeUnits()], TypeParam(0));
    }
    if (aStart < aSource.size())
    {
      ASSERT_TRUE(aString.GetCharView(aStart));
      const TCollection_UtfString<TypeParam> aScalar(*aString.GetCharView(aStart));
      EXPECT_EQ(aScalar.Length(), static_cast<size_t>(1));
      EXPECT_EQ(aScalar[0], aSource[aStart]);
    }
  }
}

TYPED_TEST(TCollection_UtfStringGenericTest, FromUnicode_InvalidByteAfterNull_RespectsInputLength)
{
  const char                       aSource[] = {'A', 0, '\xFF', 0};
  TCollection_UtfString<TypeParam> aString(U"unchanged\U0001F600");
  const auto                       anOriginal = aString;
  EXPECT_FALSE(aString.FromUnicode(aSource, 3));
  EXPECT_EQ(aString, anOriginal);

  ASSERT_TRUE(aString.FromUnicode(aSource, 2));
  const char32_t anExpected[] = {U'A', 0};
  EXPECT_EQ(aString.ToUtf32().View(), std::u32string_view(anExpected, 2));
  EXPECT_EQ(aString.Length(), size_t(2));

  ASSERT_TRUE(aString.FromUnicode(aSource));
  EXPECT_EQ(aString.ToUtf32().View(), U"A");
  EXPECT_EQ(aString.Length(), size_t(1));
}

TYPED_TEST(TCollection_UtfStringGenericTest, PartialScalarsRejectWithoutMutation)
{
  TCollection_UtfString<TypeParam> aString(U"keep\U00010000");
  const auto                       aOriginal = aString;
  const std::array<char, 5>        aUtf8 = {char(0xF0), char(0x90), char(0x80), char(0x80), 'A'};
  for (size_t aSize = 1; aSize < 4; ++aSize)
  {
    EXPECT_FALSE(aString.FromUnicode(aUtf8.data(), aSize));
    EXPECT_EQ(aString, aOriginal);
    EXPECT_FALSE(aString.FromUnicode(aUtf8.data() + aSize, aUtf8.size() - aSize));
    EXPECT_EQ(aString, aOriginal);
  }
  const std::array<char16_t, 2> aPair = {0xD800, 0xDC00};
  for (size_t i = 0; i < aPair.size(); ++i)
  {
    EXPECT_FALSE(aString.FromUnicode(aPair.data() + i, 1));
    EXPECT_EQ(aString, aOriginal);
  }
  const std::array<char32_t, 3> aInvalid = {0xD800, 0xDFFF, 0x110000};
  for (char32_t aValue : aInvalid)
  {
    EXPECT_FALSE(aString.FromUnicode(&aValue, 1));
    EXPECT_EQ(aString, aOriginal);
  }
  // Exact one-scalar nonterminated input must use the bounded constructor.
  const std::array<char32_t, 1>          aScalar = {0x10000};
  const TCollection_UtfString<TypeParam> aSingle(aScalar.data(), aScalar.size());
  EXPECT_EQ(aSingle.Length(), static_cast<size_t>(1));
  EXPECT_EQ(aSingle[0], aScalar[0]);
  ASSERT_TRUE(aString.FromUnicode(aUtf8.data() + 4, 1));
  EXPECT_EQ(aString.ToUtf32().View(), U"A");
}

TYPED_TEST(TCollection_UtfStringGenericTest, MutationSequenceMatchesScalarModel)
{
  const std::array<char32_t, 8>    aValues = {0, 'A', 0x80, 0x800, 0x10000, 0x10FFFF, 'e', 0x301};
  TCollection_UtfString<TypeParam> aString;
  std::u32string                   aReference;
  for (size_t i = 0; i < 128; ++i)
  {
    const char32_t aValue = aValues[i % aValues.size()];
    aString += TCollection_UtfString<TypeParam>(&aValue, 1);
    aReference.push_back(aValue);
    if (i % 7 == 0 && aReference.size() < 32)
    {
      aString += aString;
      aReference += aReference;
    }
    if (i % 5 == 0 && aReference.size() > 1)
    {
      const auto aFirst = aString.GetCharView(0);
      ASSERT_TRUE(aFirst);
      // Import a self-view starting at a complete scalar boundary.
      ASSERT_TRUE(aString.FromUnicode(aString.View().substr(aFirst->size())));
      aReference.erase(0, 1);
    }
    if (i % 11 == 0)
    {
      TCollection_UtfString<TypeParam> aMoved(std::move(aString));
      EXPECT_TRUE(aString.IsEmpty());
      EXPECT_EQ(aString.Length(), static_cast<size_t>(0));
      aString.Swap(aMoved);
      EXPECT_TRUE(aMoved.IsEmpty());
    }
    EXPECT_EQ(aString.Length(), aReference.size());
    EXPECT_EQ(aString.ToUtf32().View(), std::u32string_view(aReference));
    EXPECT_EQ(aString.ToCString()[aString.CodeUnits()], TypeParam(0));
    const auto aCopy = aString;
    EXPECT_EQ(aCopy, aString);
    EXPECT_FALSE(aCopy != aString);
  }
  aString.Clear();
  EXPECT_FALSE(aString.Iterator().More());
  EXPECT_EQ(aString.CodeUnits(), static_cast<size_t>(0));
  aString += TCollection_UtfString<TypeParam>(U"reuse");
  EXPECT_EQ(aString.ToUtf32().View(), U"reuse");
}

TEST(TCollection_UtfStringTest, MalformedInputIsTransactional)
{
  TCollection_UtfString<char>   aString("unchanged");
  constexpr std::array<char, 3> aBad = {'A', char(0xE2), char(0x82)};
  EXPECT_FALSE(aString.FromUnicode(aBad.data(), aBad.size()));
  EXPECT_EQ(aString.View(), "unchanged");
  constexpr std::array<char16_t, 2> aBad16 = {'A', 0xD800};
  EXPECT_FALSE(aString.FromUnicode(aBad16.data(), aBad16.size()));
  EXPECT_EQ(aString.View(), "unchanged");
  TCollection_UtfString<char16_t> aString16(u"unchanged");
  EXPECT_FALSE(aString16.FromUnicode(aBad16.data(), aBad16.size()));
  EXPECT_EQ(aString16.View(), u"unchanged");
  constexpr std::array<char32_t, 2> aBad32 = {'A', 0x110000};
  EXPECT_FALSE(aString.FromUnicode(aBad32.data(), aBad32.size()));
  EXPECT_EQ(aString.View(), "unchanged");
  TCollection_UtfString<char32_t> aString32(U"unchanged");
  EXPECT_FALSE(aString32.FromUnicode(aBad32.data(), aBad32.size()));
  EXPECT_EQ(aString32.View(), U"unchanged");
  EXPECT_FALSE(aString.FromUnicode(static_cast<const char*>(nullptr), 1));
  EXPECT_EQ(aString.View(), "unchanged");
  EXPECT_THROW((TCollection_UtfString<char>(aBad.data(), aBad.size())), Standard_DomainError);
  EXPECT_EQ(aString.View(), "unchanged");
}

TEST(TCollection_UtfStringTest, AliasingCopyMoveAndAppend)
{
  TCollection_UtfString<char> aString("abcdef");
  ASSERT_TRUE(aString.FromUnicode(aString.View().substr(1, 4)));
  EXPECT_EQ(aString.View(), "bcde");
  aString += aString;
  EXPECT_EQ(aString.View(), "bcdebcde");
  EXPECT_EQ(aString.Length(), static_cast<size_t>(8));
  TCollection_UtfString<char> aCopy(aString);
  aString.Clear();
  EXPECT_TRUE(aString.IsEmpty());
  EXPECT_NE(aString.ToCString(), nullptr);
  TCollection_UtfString<char> aMoved(std::move(aCopy));
  EXPECT_TRUE(aCopy.IsEmpty());
  EXPECT_EQ(aCopy.Length(), static_cast<size_t>(0));
  EXPECT_NE(aCopy.ToCString(), nullptr);
  EXPECT_FALSE(aCopy.Iterator().More());
  aCopy = "reuse";
  aCopy = std::move(aMoved);
  EXPECT_TRUE(aMoved.IsEmpty());
  EXPECT_EQ(aCopy.View(), "bcdebcde");
  auto* aSelf = &aCopy;
  aCopy       = std::move(*aSelf);
  EXPECT_EQ(aCopy.View(), "bcdebcde");
  aCopy.Assign(*aSelf);
  EXPECT_EQ(aCopy.View(), "bcdebcde");
  EXPECT_EQ((aCopy + "!").View(), "bcdebcde!");
}

TEST(TCollection_UtfStringTest, HeapStorageCopyMoveAndConversion)
{
  // Exceed inline string storage for every encoding and exercise growth and release.
  const std::string           aSource(4096, 'A');
  TCollection_UtfString<char> aString{std::string_view(aSource)};
  aString += aString;
  EXPECT_EQ(aString.CodeUnits(), static_cast<size_t>(8192));
  EXPECT_EQ(aString.Length(), static_cast<size_t>(8192));
  const auto aWide = aString.ToUtfWide();
  EXPECT_EQ(aWide.ToUtf8(), aString);
  EXPECT_EQ(aString.ToUtf16().ToUtf32().ToUtf8(), aString);

  TCollection_UtfString<char> aCopy(aString);
  TCollection_UtfString<char> aMoved(std::move(aString));
  EXPECT_TRUE(aString.IsEmpty());
  EXPECT_EQ(aCopy, aMoved);
  aCopy.Clear();
  aCopy += aMoved;
  EXPECT_EQ(aCopy, aMoved);
  aString = std::move(aCopy);
  EXPECT_TRUE(aCopy.IsEmpty());
  EXPECT_EQ(aString, aMoved);
  EXPECT_EQ(aString.SubString(1024, 5120).View(), std::string_view(aSource));

  ASSERT_TRUE(aCopy.FromLocale(std::string_view(aSource)));
  std::array<char, 4097> aOutput{};
  ASSERT_TRUE(aCopy.ToLocale(aOutput.data(), aOutput.size()));
  EXPECT_EQ(std::string_view(aOutput.data(), aSource.size()), std::string_view(aSource));
  EXPECT_EQ(aOutput.back(), '\0');
}

TEST(TCollection_UtfStringTest, ScalarSubstringAndBounds)
{
  const std::array<char32_t, 4> aSource = {'A', 0x10000, 0, 'B'};
  TCollection_UtfString<char>   aString(aSource.data(), aSource.size());
  EXPECT_EQ(aString.SubString(1, 3).ToUtf32().View(), std::u32string_view(aSource.data() + 1, 2));
  EXPECT_TRUE(aString.SubString(4, 4).IsEmpty());
  ASSERT_TRUE(aString.GetCharView(1));
  EXPECT_EQ(aString.GetCharView(1)->size(), static_cast<size_t>(4));
  EXPECT_FALSE(aString.GetCharView(4));
  EXPECT_FALSE(aString.GetCharView(5));
  EXPECT_THROW(aString.GetChar(4), Standard_OutOfRange);
  EXPECT_THROW(aString.GetChar(static_cast<size_t>(-1)), Standard_OutOfRange);
  EXPECT_THROW(aString.SubString(3, 2), Standard_OutOfRange);
  EXPECT_THROW(aString.SubString(0, 5), Standard_OutOfRange);
  ASSERT_TRUE(aString.FromUnicode(static_cast<const char*>(nullptr), 0));
  EXPECT_TRUE(aString.IsEmpty());
  EXPECT_FALSE(aString.GetCharView(0));
}

TEST(TCollection_UtfStringTest, LocaleBoundsAndErrors)
{
  LocaleGuard aGuard;
  ASSERT_NE(std::setlocale(LC_CTYPE, "C"), nullptr);

  TCollection_UtfString<char16_t> aString;
  ASSERT_TRUE(aString.FromLocale("several characters"));
  EXPECT_EQ(aString.ToUtf8().View(), "several characters");
  const std::string_view aEmbedded("a\0b", 3);
  ASSERT_TRUE(aString.FromLocale(aEmbedded));
  EXPECT_EQ(aString.Length(), static_cast<size_t>(3));
  std::array<char, 4> aOutput = {'x', 'x', 'x', 'x'};
  EXPECT_FALSE(aString.ToLocale(aOutput.data(), 3));
  EXPECT_EQ(aOutput, (std::array<char, 4>{'x', 'x', 'x', 'x'}));
  EXPECT_FALSE(aString.ToLocale(nullptr, 4));
  ASSERT_TRUE(aString.ToLocale(aOutput.data(), aOutput.size()));
  EXPECT_EQ(aOutput, (std::array<char, 4>{'a', 0, 'b', 0}));
#if !defined(_WIN32)
  if (aGuard.SetUtf8())
  {
    const char aBad = char(0xFF);
    EXPECT_FALSE(aString.FromLocale(std::string_view(&aBad, 1)));
    EXPECT_EQ(aString.ToUtf8().View(), aEmbedded);
    const std::array<char, 2> aTruncated = {char(0xE2), char(0x82)};
    EXPECT_FALSE(aString.FromLocale(std::string_view(aTruncated.data(), aTruncated.size())));
    EXPECT_EQ(aString.ToUtf8().View(), aEmbedded);
  }
#endif
  ASSERT_TRUE(aString.FromLocale(""));
  EXPECT_TRUE(aString.IsEmpty());
  EXPECT_FALSE(aString.ToLocale(aOutput.data(), 0));
  EXPECT_TRUE(aString.ToLocale(aOutput.data(), 1));
  EXPECT_EQ(aOutput[0], char(0));
}

TEST(TCollection_UtfStringTest, LocaleUtf8BoundedRoundTrip)
{
#if defined(_WIN32)
  GTEST_SKIP() << "Windows conversion uses CP_ACP rather than the C locale";
#else
  LocaleGuard aGuard;
  if (!aGuard.SetUtf8())
  {
    GTEST_SKIP() << "No UTF-8 C locale is installed";
  }
  const std::string aLocale = std::setlocale(LC_CTYPE, nullptr);
  // Exact-sized input with no extra terminator: nulls are part of the bounded text.
  const std::array<char, 13>      aInput    = {0,
                                               'A',
                                               char(0xC2),
                                               char(0xA2),
                                               char(0xE2),
                                               char(0x82),
                                               char(0xAC),
                                               char(0xF0),
                                               char(0x90),
                                               char(0x80),
                                               char(0x80),
                                               0,
                                               'Z'};
  const std::array<char32_t, 7>   aExpected = {0, 'A', 0xA2, 0x20AC, 0x10000, 0, 'Z'};
  TCollection_UtfString<char16_t> aString;
  ASSERT_TRUE(aString.FromLocale(std::string_view(aInput.data(), aInput.size())));
  EXPECT_EQ(aString.ToUtf32().View(), std::u32string_view(aExpected.data(), aExpected.size()));
  std::array<char, 14> aOutput;
  for (size_t aCapacity = 0; aCapacity < aOutput.size(); ++aCapacity)
  {
    aOutput.fill('x');
    EXPECT_FALSE(aString.ToLocale(aOutput.data(), aCapacity));
    for (char aByte : aOutput)
    {
      EXPECT_EQ(aByte, 'x');
    }
  }
  ASSERT_TRUE(aString.ToLocale(aOutput.data(), aOutput.size()));
  EXPECT_EQ(std::string_view(aOutput.data(), aInput.size()),
            std::string_view(aInput.data(), aInput.size()));
  EXPECT_EQ(aOutput.back(), 0);
  EXPECT_EQ(std::string(std::setlocale(LC_CTYPE, nullptr)), aLocale);
#endif
}

TEST(TCollection_UtfStringTest, LocaleFailureDoesNotPoisonNextConversion)
{
#if defined(_WIN32)
  GTEST_SKIP() << "Windows conversion uses CP_ACP rather than the C locale";
#else
  LocaleGuard aGuard;
  if (!aGuard.SetUtf8())
  {
    GTEST_SKIP() << "No UTF-8 C locale is installed";
  }
  const std::array<char, 3>       aMalformed = {'A', char(0xE2), 'B'};
  const std::array<char, 3>       aTruncated = {char(0xF0), char(0x90), char(0x80)};
  const std::array<char, 4>       aValid     = {char(0xF0), char(0x90), char(0x80), char(0x80)};
  TCollection_UtfString<char32_t> aString(U"unchanged");
  EXPECT_FALSE(aString.FromLocale(std::string_view(aMalformed.data(), aMalformed.size())));
  EXPECT_EQ(aString.View(), U"unchanged");
  EXPECT_FALSE(aString.FromLocale(std::string_view(aTruncated.data(), aTruncated.size())));
  EXPECT_EQ(aString.View(), U"unchanged");
  ASSERT_TRUE(aString.FromLocale(std::string_view(aValid.data(), aValid.size())));
  EXPECT_EQ(aString.View(), U"\U00010000");
  ASSERT_TRUE(aString.FromLocale(static_cast<const char*>(nullptr)));
  EXPECT_TRUE(aString.IsEmpty());
#endif
}

TEST(TCollection_UtfStringTest, LocaleSelectionRestorationAndOutputFailure)
{
  const std::string aPrevious = std::setlocale(LC_CTYPE, nullptr);
  {
    LocaleGuard aGuard;
    ASSERT_NE(std::setlocale(LC_CTYPE, "C"), nullptr);
    TCollection_UtfString<char16_t> aAscii;
    ASSERT_TRUE(aAscii.FromLocale("plain ASCII"));
    EXPECT_STREQ(std::setlocale(LC_CTYPE, nullptr), "C");
#if !defined(_WIN32)
    const TCollection_UtfString<char32_t> aSupplementary(U"A\U00010000");
    std::array<char, 32>                  aOutput;
    aOutput.fill('x');
    EXPECT_FALSE(aSupplementary.ToLocale(aOutput.data(), aOutput.size()));
    for (char aByte : aOutput)
    {
      EXPECT_EQ(aByte, 'x');
    }
    ASSERT_TRUE(aAscii.ToLocale(aOutput.data(), aOutput.size()));
    EXPECT_STREQ(aOutput.data(), "plain ASCII");
#endif
  }
  EXPECT_EQ(std::string(std::setlocale(LC_CTYPE, nullptr)), aPrevious);
}

TEST(TCollection_UtfStringTest, LocaleSentryRestoresThreadConversionLocale)
{
#if defined(OCCT_CLOCALE_POSIX2008) && !defined(__ANDROID__)
  LocaleGuard aGuard;
  if (!aGuard.SetUtf8())
  {
    GTEST_SKIP() << "No UTF-8 C locale is installed";
  }
  const auto                      aPreviousThreadLocale = uselocale(nullptr);
  const std::string               aGlobalLocale         = std::setlocale(LC_CTYPE, nullptr);
  const std::array<char, 3>       aEuro                 = {char(0xE2), char(0x82), char(0xAC)};
  const std::string_view          aBytes(aEuro.data(), aEuro.size());
  TCollection_UtfString<char32_t> aString;
  ASSERT_TRUE(aString.FromLocale(aBytes));
  ASSERT_EQ(aString.View(), U"\u20AC");
  {
    Standard_CLocaleSentry aSentry;
    EXPECT_EQ(uselocale(nullptr), Standard_CLocaleSentry::GetCLocale());
    EXPECT_EQ(std::string(std::setlocale(LC_CTYPE, nullptr)), aGlobalLocale);
    ASSERT_TRUE(aString.FromLocale("ASCII"));
    // C locales can reject non-ASCII bytes or map them individually, but not as UTF-8.
    const bool isConverted = aString.FromLocale(aBytes);
    EXPECT_FALSE(isConverted && aString.View() == U"\u20AC");
    {
      Standard_CLocaleSentry aNestedSentry;
      EXPECT_EQ(uselocale(nullptr), Standard_CLocaleSentry::GetCLocale());
    }
    EXPECT_EQ(uselocale(nullptr), Standard_CLocaleSentry::GetCLocale());
  }
  EXPECT_EQ(uselocale(nullptr), aPreviousThreadLocale);
  ASSERT_TRUE(aString.FromLocale(aBytes));
  EXPECT_EQ(aString.View(), U"\u20AC");
  EXPECT_EQ(std::string(std::setlocale(LC_CTYPE, nullptr)), aGlobalLocale);
#else
  GTEST_SKIP() << "Thread-local POSIX sentry coverage";
#endif
}
