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

#include <DE_ConfigurationContext.hxx>
#include <DE_ConfigurationNode.hxx>
#include <DE_Wrapper.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <gtest/gtest.h>

namespace
{
class DE_FailedLoadNode : public DESTEP_ConfigurationNode
{
public:
  bool Load(const occ::handle<DE_ConfigurationContext>&) override;
};

//==================================================================================================

bool DE_FailedLoadNode::Load(const occ::handle<DE_ConfigurationContext>&)
{
  return false;
}
} // namespace

TEST(DE_ConfigurationContextTest, BlankAndCommentLines)
{
  DE_ConfigurationContext aContext;
  ASSERT_TRUE(aContext.LoadStr("\n  \t\r\n ! comment\n first : one\n\t\nsecond: two\n"));
  EXPECT_EQ(aContext.StringVal("first", ""), "one");
  EXPECT_EQ(aContext.StringVal("second", ""), "two");
  EXPECT_EQ(aContext.GetInternalMap().Size(), size_t(2));
}

//==================================================================================================

TEST(DE_ConfigurationContextTest, SyntaxFailure)
{
  DE_ConfigurationContext aContext;
  for (const char* aText : {"no separator", ":", " : value", "first: one\ninvalid\n"})
  {
    EXPECT_FALSE(aContext.LoadStr(aText));
  }
  ASSERT_TRUE(aContext.LoadStr("empty:\nescaped: \\ value\n"));
  EXPECT_TRUE(aContext.StringVal("empty", "").IsEmpty());
  EXPECT_EQ(aContext.StringVal("escaped", ""), " value");
}

//==================================================================================================

TEST(DE_ConfigurationContextTest, MissingFileDoesNotLoadDefaults)
{
  OSD_File aFile;
  aFile.BuildTemporary();
  ASSERT_FALSE(aFile.Failed());
  OSD_Path aPath;
  aFile.Path(aPath);
  TCollection_AsciiString aName;
  aPath.SystemName(aName);
  aFile.Close();
  aFile.Remove();
  ASSERT_FALSE(aFile.Exists());

  DE_ConfigurationContext aContext;
  EXPECT_FALSE(aContext.LoadFile(aName));
  DESTEP_ConfigurationNode aNode;
  aNode.GlobalParameters.LengthUnit = 25.4;
  DE_ConfigurationNode& aBase       = aNode;
  EXPECT_FALSE(aBase.Load(aName));
  EXPECT_EQ(aNode.GlobalParameters.LengthUnit, 25.4);
}

//==================================================================================================

TEST(DE_WrapperTest, HandleCopyPreservesSourceFlag)
{
  const occ::handle<DE_Wrapper> aSource = new DE_Wrapper;
  for (const bool aKeep : {false, true})
  {
    aSource->SetKeepUpdates(aKeep);
    const DE_Wrapper aCopy(aSource);
    EXPECT_EQ(aSource->KeepUpdates(), aKeep);
    EXPECT_EQ(aCopy.KeepUpdates(), aKeep);
  }
}

//==================================================================================================

TEST(DE_WrapperTest, PropagatesLoadFailures)
{
  DE_Wrapper aWrapper;
  EXPECT_FALSE(aWrapper.Load("invalid configuration"));
  const occ::handle<DE_ConfigurationContext> aContext = new DE_ConfigurationContext;
  ASSERT_TRUE(aWrapper.Bind(new DE_FailedLoadNode));
  EXPECT_FALSE(aWrapper.Load(aContext));
  EXPECT_TRUE(aWrapper.Load(aContext, false));
}

//==================================================================================================

TEST(DE_WrapperTest, DisabledVendorSurvivesRoundTrip)
{
  DE_Wrapper                                  aSource;
  const occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode;
  ASSERT_TRUE(aSource.Bind(aNode));
  aNode->SetEnabled(false);
  const TCollection_AsciiString               aSaved = aSource.Save();
  DE_Wrapper                                  aTarget;
  const occ::handle<DESTEP_ConfigurationNode> aRestored = new DESTEP_ConfigurationNode;
  ASSERT_TRUE(aTarget.Bind(aRestored));
  ASSERT_TRUE(aTarget.Load(aSaved));
  EXPECT_FALSE(aRestored->IsEnabled());
}

//==================================================================================================

TEST(DE_ShapeFixConfigurationNodeTest, FractionalLimitsAndTopologyRoundTrip)
{
  const occ::handle<DE_ConfigurationContext> aContext = new DE_ConfigurationContext;
  ASSERT_TRUE(aContext->LoadStr("provider.STEP.OCC.healing.max.tail.width : 0.125\n"
                                "provider.STEP.OCC.healing.max.tail.angle : 12.5\n"
                                "provider.STEP.OCC.healing.detalization.level : 6\n"
                                "provider.STEP.OCC.healing.non.manifold : 1\n"));
  DESTEP_ConfigurationNode aNode;
  ASSERT_TRUE(aNode.Load(aContext));
  EXPECT_EQ(aNode.ShapeFixParameters.MaxTailWidth, 0.125);
  EXPECT_EQ(aNode.ShapeFixParameters.MaxTailAngle, 12.5);
  EXPECT_EQ(aNode.ShapeFixParameters.DetalizationLevel, TopAbs_EDGE);
  EXPECT_TRUE(aNode.ShapeFixParameters.NonManifold);
  ASSERT_TRUE(aContext->LoadStr(aNode.Save()));
  DESTEP_ConfigurationNode aRestored;
  ASSERT_TRUE(aRestored.Load(aContext));
  EXPECT_EQ(aRestored.ShapeFixParameters.MaxTailWidth, 0.125);
  EXPECT_EQ(aRestored.ShapeFixParameters.MaxTailAngle, 12.5);
  EXPECT_EQ(aRestored.ShapeFixParameters.DetalizationLevel, TopAbs_EDGE);
  EXPECT_TRUE(aRestored.ShapeFixParameters.NonManifold);

  DESTEP_ConfigurationNode aDefaults;
  EXPECT_EQ(aDefaults.ShapeFixParameters.MaxTailAngle, 0.0);
  EXPECT_EQ(aDefaults.ShapeFixParameters.MaxTailWidth, -1.0);
}
