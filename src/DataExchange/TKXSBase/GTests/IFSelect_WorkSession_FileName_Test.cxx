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

#include <IFSelect_WorkSession.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

#include <atomic>

namespace
{
//! Creates a work session that completes file names with the given prefix and extension.
//! A null argument leaves the prefix or the extension undefined.
occ::handle<IFSelect_WorkSession> createSession(const char* const thePrefix,
                                                const char* const theExtension)
{
  occ::handle<IFSelect_WorkSession> aSession = new IFSelect_WorkSession();
  if (thePrefix != nullptr)
  {
    aSession->SetFilePrefix(thePrefix);
  }
  if (theExtension != nullptr)
  {
    aSession->SetFileExtension(theExtension);
  }
  return aSession;
}
} // namespace

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileRoot_ValidName_ReturnsRootName)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, nullptr);
  const TCollection_AsciiString     aRoot    = aSession->GiveFileRoot("model.step");
  EXPECT_STREQ(aRoot.ToCString(), "model");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileRoot_InvalidName_ReturnsInputCopy)
{
  // OSD_Path::IsValid rejects names only on some platforms (for example Windows).
  const char* const anInput = "bad*name.step";
  if (OSD_Path::IsValid(TCollection_AsciiString(anInput)))
  {
    GTEST_SKIP() << "This platform accepts every name as a valid path.";
  }
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, nullptr);
  const TCollection_AsciiString     aResult  = aSession->GiveFileRoot(anInput);
  EXPECT_STREQ(aResult.ToCString(), anInput);
  // The result is a copy, not the caller's own pointer.
  EXPECT_NE(aResult.ToCString(), anInput);
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileRoot_ResultHeldAcrossSecondCall_StaysValid)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, nullptr);
  const TCollection_AsciiString     aFirst   = aSession->GiveFileRoot("alpha.step");
  const TCollection_AsciiString     aSecond  = aSession->GiveFileRoot("omega.step");
  EXPECT_STREQ(aSecond.ToCString(), "omega");
  EXPECT_STREQ(aFirst.ToCString(), "alpha");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_ResultHeldAcrossSecondCall_StaysValid)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession("out_", ".stp");
  const TCollection_AsciiString     aFirst   = aSession->GiveFileComplete("first");
  const TCollection_AsciiString     aRoot    = aSession->GiveFileRoot("second.step");
  const TCollection_AsciiString     aSecond  = aSession->GiveFileComplete("alongersecondname");
  EXPECT_STREQ(aSecond.ToCString(), "out_alongersecondname.stp");
  EXPECT_STREQ(aRoot.ToCString(), "second");
  EXPECT_STREQ(aFirst.ToCString(), "out_first.stp");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_TwoSessionsResultsHeld_UseOwnSettings)
{
  occ::handle<IFSelect_WorkSession> aSessionA = createSession("a_", ".stp");
  occ::handle<IFSelect_WorkSession> aSessionB = createSession("b_", ".igs");
  const TCollection_AsciiString     aResultA  = aSessionA->GiveFileComplete("part");
  const TCollection_AsciiString     aResultB  = aSessionB->GiveFileComplete("part");
  EXPECT_STREQ(aResultB.ToCString(), "b_part.igs");
  EXPECT_STREQ(aResultA.ToCString(), "a_part.stp");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_NoPrefixNoExtension_ReturnsInput)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, nullptr);
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("part");
  EXPECT_STREQ(aResult.ToCString(), "part");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_ExtensionMissing_AppendsExtension)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, ".stp");
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("part");
  EXPECT_STREQ(aResult.ToCString(), "part.stp");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_ExtensionPresent_KeepsName)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession(nullptr, ".stp");
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("part.txt");
  EXPECT_STREQ(aResult.ToCString(), "part.txt");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_PrefixMissing_InsertsPrefix)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession("dir/", nullptr);
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("part");
  EXPECT_STREQ(aResult.ToCString(), "dir/part");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_PrefixPresent_KeepsName)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession("dir/", nullptr);
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("other/part");
  EXPECT_STREQ(aResult.ToCString(), "other/part");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_PrefixAndExtension_AppliesBoth)
{
  occ::handle<IFSelect_WorkSession> aSession = createSession("dir/", ".stp");
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("part");
  EXPECT_STREQ(aResult.ToCString(), "dir/part.stp");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFileComplete_PrefixCheck_IgnoresAppendedExtension)
{
  // The last prefix character 'p' occurs in the appended extension but not in the given name.
  occ::handle<IFSelect_WorkSession> aSession = createSession("pre_p", ".stp");
  const TCollection_AsciiString     aResult  = aSession->GiveFileComplete("name");
  EXPECT_STREQ(aResult.ToCString(), "pre_pname.stp");
}

//==================================================================================================

TEST(IFSelect_WorkSessionTest, GiveFile_ConcurrentCallsOnTwoSessions_ReturnOwnNames)
{
  occ::handle<IFSelect_WorkSession> aSessionA = createSession("a_", ".stp");
  occ::handle<IFSelect_WorkSession> aSessionB = createSession("b_", ".igs");
  std::atomic<int>                  aNbWrong(0);
  OSD_Parallel::For(0, 20000, [&](int theIndex) {
    const bool                               isA      = (theIndex % 2 == 0);
    const occ::handle<IFSelect_WorkSession>& aSession = isA ? aSessionA : aSessionB;
    const TCollection_AsciiString            aName    = TCollection_AsciiString("part") + theIndex;
    const TCollection_AsciiString aComplete = aSession->GiveFileComplete(aName.ToCString());
    const TCollection_AsciiString aExpected =
      TCollection_AsciiString(isA ? "a_" : "b_") + aName + (isA ? ".stp" : ".igs");
    if (!aComplete.IsEqual(aExpected))
    {
      ++aNbWrong;
    }
    const TCollection_AsciiString aFile = aName + ".step";
    const TCollection_AsciiString aRoot = aSession->GiveFileRoot(aFile.ToCString());
    if (!aRoot.IsEqual(aName))
    {
      ++aNbWrong;
    }
  });
  EXPECT_EQ(aNbWrong.load(), 0);
}
