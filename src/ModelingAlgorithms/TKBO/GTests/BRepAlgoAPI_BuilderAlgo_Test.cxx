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

#include <gtest/gtest.h>

#include <type_traits>

#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Splitter.hxx>

//==================================================================================================
// BRepAlgoAPI_BuilderAlgo Non-Copyability Tests
// These tests verify that BRepAlgoAPI_BuilderAlgo and its derived classes
// are not copyable or movable, preventing double-free issues with raw pointers.
//==================================================================================================

// Test that BRepAlgoAPI_BuilderAlgo is not copy constructible
TEST(BRepAlgoAPI_BuilderAlgoTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_BuilderAlgo>::value);
}

// Test that BRepAlgoAPI_BuilderAlgo is not copy assignable
TEST(BRepAlgoAPI_BuilderAlgoTest, NotCopyAssignable)
{
  EXPECT_FALSE(std::is_copy_assignable<BRepAlgoAPI_BuilderAlgo>::value);
}

// Test that BRepAlgoAPI_BuilderAlgo is not move constructible
TEST(BRepAlgoAPI_BuilderAlgoTest, NotMoveConstructible)
{
  EXPECT_FALSE(std::is_move_constructible<BRepAlgoAPI_BuilderAlgo>::value);
}

// Test that BRepAlgoAPI_BuilderAlgo is not move assignable
TEST(BRepAlgoAPI_BuilderAlgoTest, NotMoveAssignable)
{
  EXPECT_FALSE(std::is_move_assignable<BRepAlgoAPI_BuilderAlgo>::value);
}

// Test that BRepAlgoAPI_BooleanOperation is not copy constructible
TEST(BRepAlgoAPI_BooleanOperationTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_BooleanOperation>::value);
}

// Test that BRepAlgoAPI_Cut is not copy constructible
TEST(BRepAlgoAPI_CutTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_Cut>::value);
}

// Test that BRepAlgoAPI_Fuse is not copy constructible
TEST(BRepAlgoAPI_FuseTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_Fuse>::value);
}

// Test that BRepAlgoAPI_Common is not copy constructible
TEST(BRepAlgoAPI_CommonTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_Common>::value);
}

// Test that BRepAlgoAPI_Section is not copy constructible
TEST(BRepAlgoAPI_SectionTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_Section>::value);
}

// Test that BRepAlgoAPI_Splitter is not copy constructible
TEST(BRepAlgoAPI_SplitterTest, NotCopyConstructible)
{
  EXPECT_FALSE(std::is_copy_constructible<BRepAlgoAPI_Splitter>::value);
}
