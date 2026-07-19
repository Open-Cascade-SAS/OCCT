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

#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

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

  // Test \X4\ control directive preservation
  TCollection_AsciiString anInput3("emoji \\X4\\001F600\\X0\\ face");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "emoji \\X4\\001F600\\X0\\ face");

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
  TCollection_AsciiString anInput2("\\X4\\001F600001F638\\X0\\");
  TCollection_AsciiString aResult2 = StepData_StepWriter::CleanTextForSend(anInput2);
  EXPECT_STREQ(aResult2.ToCString(), "\\X4\\001F600001F638\\X0\\");

  // Test text around hex sequences
  TCollection_AsciiString anInput3("start \\X2\\03C0\\X0\\ end");
  TCollection_AsciiString aResult3 = StepData_StepWriter::CleanTextForSend(anInput3);
  EXPECT_STREQ(aResult3.ToCString(), "start \\X2\\03C0\\X0\\ end");
}