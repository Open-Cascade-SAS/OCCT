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

#include <APIHeaderSection_MakeHeader.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <StepData_Field.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_SelectArrReal.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_ColourRgb.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

#include <istream>

namespace
{
//=================================================================================================

TCollection_AsciiString writeField(const StepData_Field& theField)
{
  occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  StepData_StepWriter             aWriter(aModel);
  aWriter.StartEntity("TEST");
  aWriter.SendField(theField, {});
  aWriter.EndEntity();
  return TCollection_AsciiString(aWriter.Line(1)->ToCString());
}
} // namespace

TEST(StepData_StepWriterTest, SendField_UnsupportedValuesWriteUndefined)
{
  StepData_Field aField;
  aField.Clear(10);
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");

  aField.Clear(StepData_Field::FieldKind::Select);
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");

  aField.SetEnum(-1, "");
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");
}

TEST(StepData_StepWriterTest, SendSelect_UnsupportedKindWritesUndefined)
{
  occ::handle<StepData_SelectInt> aSelect = new StepData_SelectInt;
  aSelect->SetKind(static_cast<int>(StepData_Field::FieldKind::Any));

  StepData_Field aField;
  aField.SetSelectMember(aSelect);
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");

  aSelect->SetKind(261);
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");
}

TEST(StepData_StepWriterTest, SendSelect_RealArrayPreservesBoundsAndParameterPosition)
{
  occ::handle<NCollection_HArray1<double>> anArray = new NCollection_HArray1<double>(-2, 0);
  anArray->SetValue(-2, 1.0);
  anArray->SetValue(-1, 2.0);
  anArray->SetValue(0, 3.0);

  occ::handle<StepData_SelectArrReal> aSelect = new StepData_SelectArrReal;
  aSelect->SetArrReal(anArray);

  occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  StepData_StepWriter             aWriter(aModel);
  aWriter.StartEntity("TEST");
  aWriter.Send(9);
  aWriter.SendSelect(aSelect, {});
  aWriter.Send(10);
  aWriter.EndEntity();
  EXPECT_STREQ(aWriter.Line(1)->ToCString(), "TEST(9,(1.,2.,3.),10);");

  aSelect->SetName("VALUES");
  StepData_StepWriter aNamedWriter(aModel);
  aNamedWriter.StartEntity("TEST");
  aNamedWriter.SendSelect(aSelect, {});
  aNamedWriter.EndEntity();
  EXPECT_STREQ(aNamedWriter.Line(1)->ToCString(), "TEST(VALUES((1.,2.,3.)));");
}

TEST(StepData_StepWriterTest, SendSelect_NullRealArrayWritesUndefined)
{
  occ::handle<StepData_SelectArrReal> aSelect = new StepData_SelectArrReal;

  StepData_Field aField;
  aField.SetSelectMember(aSelect);
  EXPECT_STREQ(writeField(aField).ToCString(), "TEST($);");
}

// Test CleanTextForSend with basic character escaping
TEST(StepData_StepWriterTest, CleanTextForSend_BasicEscaping)
{
  // Test single quote escaping
  TCollection_AsciiString anInput1("text with 'single quotes'");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "text with ''single quotes''");

  // Test backslash escaping
  TCollection_AsciiString anInput2("path\\with\\backslashes");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "path\\\\with\\\\backslashes");

  // Test newline escaping
  TCollection_AsciiString anInput3("line1\nline2");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "line1\\N\\line2");

  // Test tab escaping
  TCollection_AsciiString anInput4("text\twith\ttabs");
  TCollection_AsciiString aResult4 = StepData_StepWriter::CleanTextForSend(anInput4);
  EXPECT_STREQ(aResult4.ToCString(), "text\\T\\with\\T\\tabs");
}

// Test CleanTextForSend with control directives preservation
TEST(StepData_StepWriterTest, CleanTextForSend_ControlDirectivePreservation)
{
  // Test \X\ control directive preservation
  TCollection_AsciiString anInput1("text with \\XA7\\ section sign");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "text with \\XA7\\ section sign");

  // Test \X2\ control directive preservation
  TCollection_AsciiString anInput2("\\X2\\03C0\\X0\\ is pi");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "\\X2\\03C0\\X0\\ is pi");

  // Test \X4\ control directive preservation (8 hexadecimal digits per character)
  TCollection_AsciiString anInput3("emoji \\X4\\0001F600\\X0\\ face");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "emoji \\X4\\0001F600\\X0\\ face");

  // Test \S\ control directive preservation
  TCollection_AsciiString anInput4("text with \\S\\ directive");
  TCollection_AsciiString aResult4 = StepData_StepWriter::CleanTextForSend(anInput4);
  EXPECT_STREQ(aResult4.ToCString(), "text with \\S\\ directive");

  // Test \P\ control directive preservation
  TCollection_AsciiString anInput5("\\PA\\ code page setting");
  TCollection_AsciiString aResult5 = StepData_StepWriter::CleanTextForSend(anInput5);
  EXPECT_STREQ(aResult5.ToCString(), "\\PA\\ code page setting");
}

// Test CleanTextForSend with existing \N\ and \T\ directive preservation
TEST(StepData_StepWriterTest, CleanTextForSend_ExistingDirectivePreservation)
{
  // Test existing \N\ directive preservation
  TCollection_AsciiString anInput1("line1\\N\\line2");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "line1\\N\\line2");

  // Test existing \T\ directive preservation
  TCollection_AsciiString anInput2("text\\T\\with\\T\\tab");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "text\\T\\with\\T\\tab");
}

// Test CleanTextForSend with mixed content
TEST(StepData_StepWriterTest, CleanTextForSend_MixedContent)
{
  // Test quotes outside control directives
  TCollection_AsciiString anInput1("see \\XA7\\ section and 'quotes'");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "see \\XA7\\ section and ''quotes''");

  // Test backslashes outside control directives
  TCollection_AsciiString anInput2("\\XA7\\ and path\\file");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "\\XA7\\ and path\\\\file");

  // Test complex mixture
  TCollection_AsciiString anInput3("prefix \\X2\\03B103B2\\X0\\ 'text' with\ttab");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "prefix \\X2\\03B103B2\\X0\\ ''text'' with\\T\\tab");
}

// Test CleanTextForSend with edge cases
TEST(StepData_StepWriterTest, CleanTextForSend_EdgeCases)
{
  // Test empty string
  TCollection_AsciiString anInput1("");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "");

  // Test string with only quotes
  TCollection_AsciiString anInput2("''");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "''''");

  // Test string with only control directive
  TCollection_AsciiString anInput3("\\XA7\\");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "\\XA7\\");

  // Test consecutive control directives
  TCollection_AsciiString anInput4("\\XA7\\\\XB6\\");
  TCollection_AsciiString aResult4 = StepData_StepWriter::CleanTextForSend(anInput4);
  EXPECT_STREQ(aResult4.ToCString(), "\\XA7\\\\XB6\\");
}

// Test CleanTextForSend with malformed but safe input
TEST(StepData_StepWriterTest, CleanTextForSend_MalformedInput)
{
  // Test incomplete control directive (should be treated as regular text)
  TCollection_AsciiString anInput1("incomplete \\X and 'quotes'");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "incomplete \\\\X and ''quotes''");

  // Test partial control directive
  TCollection_AsciiString anInput2("partial \\XA and more");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "partial \\\\XA and more");
}

// Test CleanTextForSend hex sequence detection
TEST(StepData_StepWriterTest, CleanTextForSend_HexSequenceDetection)
{
  // Test valid hex sequences in \X2\ directive
  TCollection_AsciiString anInput1("\\X2\\03B103B203B3\\X0\\");
  TCollection_AsciiString aResult1 = StepData_StepWriter::CleanTextForSend(anInput1);
  EXPECT_STREQ(aResult1.ToCString(), "\\X2\\03B103B203B3\\X0\\");

  // Test valid hex sequences in \X4\ directive
  TCollection_AsciiString anInput2("\\X4\\0001F6000001F638\\X0\\");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "\\X4\\0001F6000001F638\\X0\\");

  // Test text around hex sequences
  TCollection_AsciiString anInput3("start \\X2\\03C0\\X0\\ end");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "start \\X2\\03C0\\X0\\ end");
}

// Test that only complete \X2\ and \X4\ runs are kept: anything else is text, so its
// backslashes and apostrophes are doubled and the string stays valid
TEST(StepData_StepWriterTest, CleanTextForSend_IncompleteRunIsText)
{
  // other letters than hexadecimal digits, with an apostrophe
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("\\X2\\ don't \\X0\\").ToCString(),
               "\\\\X2\\\\ don''t \\\\X0\\\\");
  // no closing directive at the end
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("a\\X2\\03B3*b").ToCString(),
               "a\\\\X2\\\\03B3*b");
  // a number of digits that is not a multiple of 4 (or of 8 after X4)
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("\\X2\\03B\\X0\\").ToCString(),
               "\\\\X2\\\\03B\\\\X0\\\\");
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("\\X4\\001F600\\X0\\").ToCString(),
               "\\\\X4\\\\001F600\\\\X0\\\\");
  // an empty run, and the closing directive alone
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("\\X2\\\\X0\\").ToCString(),
               "\\\\X2\\\\\\\\X0\\\\");
  EXPECT_STREQ(StepData_StepWriter::CleanTextForSend("end\\X0\\").ToCString(), "end\\\\X0\\\\");
}

namespace
{
//=================================================================================================

//! Writes "TEST('<theText>');" and returns all written lines.
NCollection_Sequence<TCollection_AsciiString> writeString(
  const TCollection_AsciiString&                    theText,
  const DESTEP_Parameters::WriteMode_StringEncoding theEncoding =
    DESTEP_Parameters::WriteMode_StringEncoding_UTF8)
{
  occ::handle<StepData_StepModel> aModel         = new StepData_StepModel;
  aModel->InternalParameters.WriteStringEncoding = theEncoding;
  StepData_StepWriter aWriter(aModel);
  aWriter.StartEntity("TEST");
  aWriter.Send(theText);
  aWriter.EndEntity();
  NCollection_Sequence<TCollection_AsciiString> aLines;
  for (int aLine = 1; aLine <= aWriter.NbLines(); ++aLine)
  {
    aLines.Append(aWriter.Line(aLine)->ToCString());
  }
  return aLines;
}

//=================================================================================================

//! Joins the lines again; line breaks are not part of a string (ISO 10303-21, 5.6).
TCollection_AsciiString joinLines(const NCollection_Sequence<TCollection_AsciiString>& theLines)
{
  TCollection_AsciiString aText;
  for (const TCollection_AsciiString& aLine : theLines)
  {
    aText += aLine;
  }
  return aText;
}
} // namespace

// Test that a long string is wrapped between UTF-8 characters, never inside one
TEST(StepData_StepWriterTest, Send_LongStringKeepsUtf8CharactersWhole)
{
  // 40 times U+AC00 (3 bytes in UTF-8) after 0 to 10 ASCII letters,
  // so that the 72nd byte of the line falls on every byte of a character
  for (int aPrefix = 0; aPrefix <= 10; ++aPrefix)
  {
    SCOPED_TRACE(aPrefix);
    TCollection_AsciiString aText;
    for (int anIndex = 0; anIndex < aPrefix; ++anIndex)
    {
      aText += "a";
    }
    for (int anIndex = 0; anIndex < 40; ++anIndex)
    {
      aText += "\xEA\xB0\x80";
    }
    const NCollection_Sequence<TCollection_AsciiString> aLines = writeString(aText);
    ASSERT_GT(aLines.Length(), 2);
    for (const TCollection_AsciiString& aLine : aLines)
    {
      EXPECT_LE(aLine.Length(), 72);
      EXPECT_FALSE((static_cast<unsigned char>(aLine.Value(1)) & 0xC0) == 0x80) << aLine;
    }
    EXPECT_STREQ(joinLines(aLines).ToCString(),
                 (TCollection_AsciiString("TEST('") + aText + "');").ToCString());
  }
}

// Test that a long string is never wrapped between the two letters of '' or of \\,
// nor inside a control directive that is kept as it is
TEST(StepData_StepWriterTest, Send_LongStringKeepsEscapesWhole)
{
  const char* const aPieces[] = {"x'", "x\\", "x\\X2\\0041\\X0\\"};
  for (const char* aPiece : aPieces)
  {
    for (int aPrefix = 0; aPrefix <= 12; ++aPrefix)
    {
      SCOPED_TRACE(TCollection_AsciiString(aPiece) + " after " + aPrefix);
      TCollection_AsciiString aText;
      for (int anIndex = 0; anIndex < aPrefix; ++anIndex)
      {
        aText += "a";
      }
      for (int anIndex = 0; anIndex < 30; ++anIndex)
      {
        aText += aPiece;
      }
      const NCollection_Sequence<TCollection_AsciiString> aLines = writeString(aText);
      ASSERT_GT(aLines.Length(), 2);
      const TCollection_AsciiString aJoined = joinLines(aLines);
      EXPECT_STREQ(
        aJoined.ToCString(),
        (TCollection_AsciiString("TEST('") + StepData_StepWriter::CleanTextForSend(aText) + "');")
          .ToCString());

      // positions in aJoined where a line ends
      NCollection_Sequence<int> aBreaks;
      int                       aPosition = 0;
      for (int aLine = 1; aLine < aLines.Length(); ++aLine)
      {
        aPosition += aLines.Value(aLine).Length();
        aBreaks.Append(aPosition);
      }
      // walk the string token and check that no line ends inside '' , \\ or \X2\...\X0\ .
      for (int anIndex = 7; anIndex < aJoined.Length() - 2;)
      {
        int aLength = 1;
        if (aJoined.Value(anIndex) == '\\' && aJoined.Value(anIndex + 1) == 'X')
        {
          aLength = aJoined.Search("\\X0\\") > 0 ? 12 : 1; // the directive \X2\0041\X0\ above
        }
        else if ((aJoined.Value(anIndex) == '\'' || aJoined.Value(anIndex) == '\\')
                 && aJoined.Value(anIndex + 1) == aJoined.Value(anIndex))
        {
          aLength = 2;
        }
        for (const int aBreak : aBreaks)
        {
          EXPECT_FALSE(aBreak >= anIndex && aBreak < anIndex + aLength - 1)
            << "line ends at " << aBreak << " inside the piece at " << anIndex;
        }
        anIndex += aLength;
      }
    }
  }
}

// Test that a line break next to a backslash or an apostrophe has at least three bytes of the
// string after it (the reader of OCCT 7.5 fails otherwise), while plain text keeps its layout
TEST(StepData_StepWriterTest, Send_LongStringLeavesBytesAfterBreakNextToEscape)
{
  const char* const aTails[] = {"\\y", "y\\", "'y", "y'", "\\\\", "''", "\\", "'"};
  for (const char* aTail : aTails)
  {
    for (int aPrefix = 60; aPrefix <= 76; ++aPrefix)
    {
      SCOPED_TRACE(TCollection_AsciiString(aTail) + " after " + aPrefix);
      TCollection_AsciiString aText;
      for (int anIndex = 0; anIndex < aPrefix; ++anIndex)
      {
        aText += "x";
      }
      aText += aTail;
      const NCollection_Sequence<TCollection_AsciiString> aLines  = writeString(aText);
      const TCollection_AsciiString                       aJoined = joinLines(aLines);
      // the string token, without the indentation that the writer may put outside of it
      const int anOpen = aJoined.Search("'");
      const int aClose = aJoined.SearchFromEnd("'");
      ASSERT_GT(anOpen, 0);
      EXPECT_STREQ(aJoined.SubString(anOpen + 1, aClose - 1).ToCString(),
                   StepData_StepWriter::CleanTextForSend(aText).ToCString());
      int aBreak = 0;
      for (int aLine = 1; aLine < aLines.Length(); ++aLine)
      {
        aBreak += aLines.Value(aLine).Length();
        if (aBreak <= anOpen || aBreak >= aClose)
        {
          continue; // not inside the string
        }
        const char aBefore = aJoined.Value(aBreak);
        const char anAfter = aJoined.Value(aBreak + 1);
        if (aBefore == '\\' || aBefore == '\''
            || (aBreak + 1 < aClose && (anAfter == '\\' || anAfter == '\'')))
        {
          EXPECT_GE(aClose - aBreak - 1, 3) << "break at " << aBreak;
        }
      }
    }
  }
  // plain text is wrapped as before: 72 letters give a line of 72 bytes and "x'" on the next one
  TCollection_AsciiString aPlain;
  for (int anIndex = 0; anIndex < 72; ++anIndex)
  {
    aPlain += "x";
  }
  const NCollection_Sequence<TCollection_AsciiString> aLines = writeString(aPlain);
  ASSERT_GE(aLines.Length(), 2);
  EXPECT_STREQ(aLines.Value(aLines.Length()).ToCString(), "x');");
}

namespace
{
//=================================================================================================

//! Writes a model whose header has the implementation level theLevel and whose data has one
//! COLOUR_RGB named theName. Returns the implementation level found in the written header.
TCollection_AsciiString writtenLevel(const char* theLevel, const char* theName)
{
  STEPControl_Writer              aStepWriter; // creates a model with the STEP protocol
  occ::handle<StepData_StepModel> aModel = aStepWriter.Model(true);
  APIHeaderSection_MakeHeader     aHeader;
  aHeader.SetImplementationLevel(new TCollection_HAsciiString(theLevel));
  aHeader.Apply(aModel);
  occ::handle<StepVisual_ColourRgb> aColour = new StepVisual_ColourRgb;
  aColour->Init(new TCollection_HAsciiString(theName), 1.0, 0.0, 0.0);
  aModel->AddWithRefs(aColour);

  StepData_StepWriter aWriter(aModel);
  aWriter.SendModel(occ::down_cast<StepData_Protocol>(aModel->Protocol()));
  for (int aLine = 1; aLine <= aWriter.NbLines(); ++aLine)
  {
    const TCollection_AsciiString aText = aWriter.Line(aLine)->ToCString();
    if (aText.Search("FILE_DESCRIPTION") == 1)
    {
      // FILE_DESCRIPTION(('...'),'<level>');
      const int aClose = aText.SearchFromEnd("'");
      int       anOpen = aClose - 1;
      while (anOpen > 0 && aText.Value(anOpen) != '\'')
      {
        --anOpen;
      }
      return aText.SubString(anOpen + 1, aClose - 1);
    }
  }
  return "";
}
} // namespace

// Test that the header says "4;1" (ISO 10303-21:2016) when strings are written as raw UTF-8,
// because the levels "2;1" and "3;1" allow other characters than ASCII only as \X2\ and \X4\.
TEST(StepData_StepWriterTest, SendModel_RawUtf8NeedsEdition3Level)
{
  const char* const anAscii = "Box";
  const char* const anUtf8  = "Box \xEC\x83\x81\xEC\x9E\x90"; // "Box" and two Korean characters
  EXPECT_STREQ(writtenLevel("2;1", anAscii).ToCString(), "2;1");
  EXPECT_STREQ(writtenLevel("2;1", anUtf8).ToCString(), "4;1");
  EXPECT_STREQ(writtenLevel("3;1", anUtf8).ToCString(), "4;1");
  EXPECT_STREQ(writtenLevel("4;1", anUtf8).ToCString(), "4;1");
  // other values are written as they are
  EXPECT_STREQ(writtenLevel("1", anUtf8).ToCString(), "1");
}

// Test that characters outside ASCII are written with \X2\ and \X4\ when the parameter asks it
TEST(StepData_StepWriterTest, Send_EscapeModeWritesControlDirectives)
{
  const DESTEP_Parameters::WriteMode_StringEncoding anEscape =
    DESTEP_Parameters::WriteMode_StringEncoding_Escape;
  // "Box", U+C0C1 U+C790 (Korean), U+1F600 (outside the BMP) and U+00E9
  EXPECT_STREQ(
    joinLines(writeString("Box \xEC\x83\x81\xEC\x9E\x90 \xF0\x9F\x98\x80 \xC3\xA9", anEscape))
      .ToCString(),
    "TEST('Box \\X2\\C0C1C790\\X0\\ \\X4\\0001F600\\X0\\ \\X2\\00E9\\X0\\');");
  // a character of the BMP right before one outside it, and an apostrophe
  EXPECT_STREQ(joinLines(writeString("It's \xC3\xA9\xF0\x9F\x98\x80", anEscape)).ToCString(),
               "TEST('It''s \\X2\\00E9\\X0\\\\X4\\0001F600\\X0\\');");
  // a byte that is not valid UTF-8 is kept as \X\hh
  EXPECT_STREQ(joinLines(writeString("a\xFFz", anEscape)).ToCString(), "TEST('a\\X\\FFz');");
  // a single N or T right after a directive goes inside it, because OCCT 7.5 would read
  // "\X0\T\X2\" as a tab; U+AC00, "T", U+B098 and then U+AC00, "N", a backslash, "x"
  EXPECT_STREQ(joinLines(writeString("\xEA\xB0\x80T\xEB\x82\x98", anEscape)).ToCString(),
               "TEST('\\X2\\AC000054B098\\X0\\');");
  EXPECT_STREQ(joinLines(writeString("\xEA\xB0\x80N\\x", anEscape)).ToCString(),
               "TEST('\\X2\\AC00004E\\X0\\\\\\x');");
  EXPECT_STREQ(joinLines(writeString("\xEA\xB0\x80TT\xEB\x82\x98", anEscape)).ToCString(),
               "TEST('\\X2\\AC00\\X0\\TT\\X2\\B098\\X0\\');");
  // ASCII is written as before, and so is everything in the default mode
  EXPECT_STREQ(joinLines(writeString("plain", anEscape)).ToCString(), "TEST('plain');");
  EXPECT_STREQ(joinLines(writeString("\xC3\xA9")).ToCString(), "TEST('\xC3\xA9');");
}

// Test that a \X2\ or \X4\ run longer than a line is closed at the end of the line and opened
// again on the next one, instead of being cut or written on one very long line
TEST(StepData_StepWriterTest, Send_LongRunIsClosedAndOpenedAgain)
{
  TCollection_AsciiString aKorean; // 40 times U+AC00
  TCollection_AsciiString anEmoji; // 20 times U+1F600
  TCollection_AsciiString aKept;   // a run already in the text, kept as it is in the default mode
  for (int anIndex = 0; anIndex < 40; ++anIndex)
  {
    aKorean += "\xEA\xB0\x80";
    aKept += anIndex == 0 ? "\\X2\\0041" : "0041";
  }
  aKept += "\\X0\\";
  for (int anIndex = 0; anIndex < 20; ++anIndex)
  {
    anEmoji += "\xF0\x9F\x98\x80";
  }

  const struct
  {
    TCollection_AsciiString                     Text;
    DESTEP_Parameters::WriteMode_StringEncoding Encoding;
    const char*                                 Opening;
  } aCases[] = {{aKorean, DESTEP_Parameters::WriteMode_StringEncoding_Escape, "\\X2\\"},
                {anEmoji, DESTEP_Parameters::WriteMode_StringEncoding_Escape, "\\X4\\"},
                {aKept, DESTEP_Parameters::WriteMode_StringEncoding_UTF8, "\\X2\\"}};

  for (const auto& aCase : aCases)
  {
    SCOPED_TRACE(aCase.Opening);
    const NCollection_Sequence<TCollection_AsciiString> aLines =
      writeString(aCase.Text, aCase.Encoding);
    ASSERT_GT(aLines.Length(), 2);
    for (int aLine = 1; aLine <= aLines.Length(); ++aLine)
    {
      const TCollection_AsciiString& aText = aLines.Value(aLine);
      EXPECT_LE(aText.Length(), 72) << aText;
      if (aLine > 1 && aLine < aLines.Length())
      {
        // a whole part of the run on each line
        EXPECT_EQ(aText.Search(aCase.Opening), aText.Value(1) == '\'' ? 2 : 1) << aText;
        EXPECT_EQ(aText.SearchFromEnd("\\X0\\"), aText.Length() - 3) << aText;
      }
    }
  }
}

// Test that a file written with escapes has only ASCII, keeps "2;1" and is read back exactly
TEST(StepData_StepWriterTest, SendModel_EscapeModeRoundTrip)
{
  TCollection_AsciiString aLong; // 40 Korean characters, longer than one line when escaped
  for (int anIndex = 0; anIndex < 40; ++anIndex)
  {
    aLong += "\xEA\xB0\x80";
  }
  const TCollection_AsciiString aNames[] = {"Box \xEC\x83\x81\xEC\x9E\x90",
                                            "\xF0\x9F\x98\x80 \xF0\xA0\x80\x8B",
                                            "Gr\xC3\xBC\xC3\x9F"
                                            "e 'quoted' C:\\temp",
                                            aLong};

  STEPControl_Writer              aStepWriter; // creates a model with the STEP protocol
  occ::handle<StepData_StepModel> aModel = aStepWriter.Model(true);
  APIHeaderSection_MakeHeader     aHeader;
  aHeader.Apply(aModel);
  aModel->InternalParameters.WriteStringEncoding =
    DESTEP_Parameters::WriteMode_StringEncoding_Escape;
  for (const TCollection_AsciiString& aName : aNames)
  {
    occ::handle<StepVisual_ColourRgb> aColour = new StepVisual_ColourRgb;
    aColour->Init(new TCollection_HAsciiString(aName), 1.0, 0.0, 0.0);
    aModel->AddWithRefs(aColour);
  }
  StepData_StepWriter aWriter(aModel);
  aWriter.SendModel(occ::down_cast<StepData_Protocol>(aModel->Protocol()));
  TCollection_AsciiString aText;
  for (int aLine = 1; aLine <= aWriter.NbLines(); ++aLine)
  {
    aText += aWriter.Line(aLine)->ToCString();
    aText += "\n";
  }
  for (int anIndex = 1; anIndex <= aText.Length(); ++anIndex)
  {
    ASSERT_LT(static_cast<unsigned char>(aText.Value(anIndex)), 0x80) << anIndex;
  }
  EXPECT_GT(aText.Search("'2;1');"), 0);

  Standard_ArrayStreamBuffer aBuffer(aText.ToCString(), aText.Length());
  std::istream               aStream(&aBuffer);
  STEPControl_Reader         aReader;
  ASSERT_EQ(aReader.ReadStream("escape.stp", aStream), IFSelect_RetDone);
  const occ::handle<StepData_StepModel> aReadModel = aReader.StepModel();
  ASSERT_EQ(aReadModel->NbEntities(), 4);
  for (int anIndex = 1; anIndex <= 4; ++anIndex)
  {
    const occ::handle<StepVisual_ColourRgb> aColour =
      occ::down_cast<StepVisual_ColourRgb>(aReadModel->Value(anIndex));
    ASSERT_FALSE(aColour.IsNull());
    EXPECT_STREQ(aColour->Name()->ToCString(), aNames[anIndex - 1].ToCString());
  }
}
