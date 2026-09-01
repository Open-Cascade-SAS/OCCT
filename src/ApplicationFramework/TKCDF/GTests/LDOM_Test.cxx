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

#include <LDOM_BasicElement.hxx>
#include <LDOM_CharReference.hxx>
#include <LDOM_MemManager.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

namespace
{
TCollection_AsciiString encode(const TCollection_AsciiString& theValue, const bool theIsAttribute)
{
  int                     aLength = 0;
  const char*             aSource = theValue.ToCString();
  char*                   aResult = LDOM_CharReference::Encode(aSource, aLength, theIsAttribute);
  TCollection_AsciiString anEncoded(aResult, aLength);
  if (aResult != aSource)
  {
    delete[] aResult;
  }
  return anEncoded;
}
} // namespace

TEST(LDOM_CharReference, EncodeCharacterClasses)
{
  EXPECT_EQ(encode("text", false), "text");
  EXPECT_EQ(encode("\t\n\r", false), "\t\n\r");
  EXPECT_EQ(encode("&<>", false), "&amp;&lt;&gt;");
  EXPECT_EQ(encode("\"'", false), "\"'");
  EXPECT_EQ(encode("\"'", true), "&quot;'");
  EXPECT_EQ(encode(TCollection_AsciiString(1, '\x01'), false), "&#x01;");
  EXPECT_EQ(encode(TCollection_AsciiString(1, static_cast<char>(0x80)), false), "&#x80;");
}

TEST(LDOM_BasicElement, CreateInitializesElement)
{
  occ::handle<LDOM_MemManager> aManager = new LDOM_MemManager(1024);
  LDOM_BasicElement&           anElement = LDOM_BasicElement::Create("element", 7, aManager);

  EXPECT_FALSE(anElement.isNull());
  EXPECT_EQ(anElement.getNodeType(), LDOM_Node::ELEMENT_NODE);
  EXPECT_STREQ(anElement.GetTagName(), "element");
}

TEST(LDOM_BasicElement, CreateRejectsInvalidArguments)
{
  const occ::handle<LDOM_MemManager> aManager = new LDOM_MemManager(1024);
  const occ::handle<LDOM_MemManager> aNullManager;
  EXPECT_THROW(LDOM_BasicElement::Create("element", 7, aNullManager), Standard_NullObject);
  EXPECT_THROW(LDOM_BasicElement::Create(nullptr, 0, aManager), Standard_NullObject);
  EXPECT_THROW(LDOM_BasicElement::Create("", 0, aManager), Standard_ConstructionError);
  EXPECT_THROW(LDOM_BasicElement::Create("element", -1, aManager), Standard_ConstructionError);
}
