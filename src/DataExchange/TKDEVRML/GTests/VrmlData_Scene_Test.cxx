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

#include <VrmlData_InBuffer.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_Node.hxx>
#include <VrmlData_Sphere.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <utility>

//==================================================================================================

TEST(VrmlData_SceneTest, ReadsSupportedHeadersAndRejectsUnknownVersion)
{
  for (const auto& [aHeader, anExpectedStatus] :
       {std::pair{"#VRML V1.0 ascii", VrmlData_StatusOK},
        std::pair{"#VRML V2.0 utf8", VrmlData_StatusOK},
        std::pair{"#VRML V3.0 utf8", VrmlData_NotVrmlFile},
        std::pair{"", VrmlData_NotVrmlFile}})
  {
    SCOPED_TRACE(aHeader);
    const TCollection_AsciiString aText =
      TCollection_AsciiString(aHeader) + "\nDEF Ball Sphere { radius 2 }\n";
    std::istringstream anInput(aText.ToCString());
    VrmlData_Scene     aScene;
    aScene << anInput;
    ASSERT_EQ(aScene.Status(), anExpectedStatus);
    const occ::handle<VrmlData_Sphere> aSphere =
      occ::down_cast<VrmlData_Sphere>(aScene.FindNode("Ball"));
    if (anExpectedStatus == VrmlData_StatusOK)
    {
      ASSERT_FALSE(aSphere.IsNull());
      EXPECT_DOUBLE_EQ(aSphere->Radius(), 2.0);
    }
    else
    {
      EXPECT_TRUE(aSphere.IsNull());
    }
  }
}

//==================================================================================================

TEST(VrmlData_SceneTest, ReadsStringLists)
{
  for (const char* aText : {"[\"a\",\"b\"]\n", "[\"a\" \"b\"]\n", "[\"a\",\n# comment\n\"b\"]\n"})
  {
    std::istringstream                        anInput(aText);
    VrmlData_InBuffer                         aBuffer(anInput);
    NCollection_List<TCollection_AsciiString> aStrings;
    ASSERT_EQ(VrmlData_Node::ReadMultiString(aBuffer, aStrings), VrmlData_StatusOK);
    ASSERT_EQ(aStrings.Size(), size_t(2));
    EXPECT_EQ(aStrings.First(), "a");
    EXPECT_EQ(aStrings.Last(), "b");
  }
}

//==================================================================================================

TEST(VrmlData_SceneTest, ReadsEmptyAndSingleStrings)
{
  for (const char* aText : {"[]\n", "[ ]\n"})
  {
    std::istringstream                        anInput(aText);
    VrmlData_InBuffer                         aBuffer(anInput);
    NCollection_List<TCollection_AsciiString> aStrings;
    EXPECT_EQ(VrmlData_Node::ReadMultiString(aBuffer, aStrings), VrmlData_StatusOK);
    EXPECT_TRUE(aStrings.IsEmpty());
  }
  std::istringstream                        anInput("\"single\"\n");
  VrmlData_InBuffer                         aBuffer(anInput);
  NCollection_List<TCollection_AsciiString> aStrings;
  ASSERT_EQ(VrmlData_Node::ReadMultiString(aBuffer, aStrings), VrmlData_StatusOK);
  ASSERT_EQ(aStrings.Size(), size_t(1));
  EXPECT_EQ(aStrings.First(), "single");
}

//==================================================================================================

TEST(VrmlData_SceneTest, RejectsIncompleteStringList)
{
  for (const char* aText : {"[\"a\"\n", "[\"a\", invalid]\n"})
  {
    std::istringstream                        anInput(aText);
    VrmlData_InBuffer                         aBuffer(anInput);
    NCollection_List<TCollection_AsciiString> aStrings;
    EXPECT_NE(VrmlData_Node::ReadMultiString(aBuffer, aStrings), VrmlData_StatusOK);
  }
}

TEST(VrmlData_SceneTest, HandlesTrailingQuotedEscape)
{
  std::istringstream anInput("\"unterminated\\\n");
  VrmlData_InBuffer  aBuffer(anInput);
  EXPECT_EQ(VrmlData_Scene::ReadLine(aBuffer), VrmlData_StatusOK);
  EXPECT_STREQ(aBuffer.Line.data(), "\"unterminated\\");
}

TEST(VrmlData_SceneTest, RejectsLineExceedingInputBuffer)
{
  std::istringstream anInput(std::string(9000, 'a'));
  VrmlData_InBuffer  aBuffer(anInput);
  EXPECT_EQ(VrmlData_Scene::ReadLine(aBuffer), VrmlData_UnrecoverableError);
}

//==================================================================================================

TEST(VrmlData_SceneTest, ReadsEscapedStringListsAndPreservesFollowingField)
{
  std::istringstream anInput(R"vrml(["a\"b", "c\\d", "#tag", "x]y"] "after")vrml");
  VrmlData_InBuffer  aBuffer(anInput);
  NCollection_List<TCollection_AsciiString> aStrings;
  ASSERT_EQ(VrmlData_Node::ReadMultiString(aBuffer, aStrings), VrmlData_StatusOK);
  ASSERT_EQ(aStrings.Size(), size_t(4));
  NCollection_List<TCollection_AsciiString>::Iterator anIterator(aStrings);
  for (const char* anExpected : {"a\"b", "c\\d", "#tag", "x]y"})
  {
    ASSERT_TRUE(anIterator.More());
    EXPECT_EQ(anIterator.Value(), anExpected);
    anIterator.Next();
  }
  TCollection_AsciiString aFollowing;
  EXPECT_EQ(VrmlData_Node::ReadString(aBuffer, aFollowing), VrmlData_StatusOK);
  EXPECT_EQ(aFollowing, "after");
}

//==================================================================================================

TEST(VrmlData_SceneTest, RejectsUnterminatedEscapeWithoutChangingResult)
{
  std::istringstream      anInput("\"unterminated\\");
  VrmlData_InBuffer       aBuffer(anInput);
  TCollection_AsciiString aString("original");
  EXPECT_EQ(VrmlData_Node::ReadString(aBuffer, aString), VrmlData_StringInputError);
  EXPECT_EQ(aString, "original");
}
