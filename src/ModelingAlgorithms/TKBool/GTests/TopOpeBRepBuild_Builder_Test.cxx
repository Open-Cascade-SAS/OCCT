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
// Alternatively, this file may be used under the terms of the Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>

namespace
{
class BuilderProbe : public TopOpeBRepBuild_Builder
{
public:
  explicit BuilderProbe(const TopOpeBRepDS_BuildTool& theBuildTool)
      : TopOpeBRepBuild_Builder(theBuildTool)
  {
  }

  bool IsFaces2d() const { return myFaces2d; }

  void SetFaces2d(const bool theIsFaces2d) { myFaces2d = theIsFaces2d; }

  bool IsClassifySplitEdge() const { return myClassifySplitEdge; }

  void SetClassifySplitEdge(const bool theClassifySplitEdge)
  {
    myClassifySplitEdge = theClassifySplitEdge;
  }

  bool IsReverseOwnSplitFaceOrientation() const
  {
    return myReverseOwnSplitFaceOrientation;
  }

  void SetReverseOwnSplitFaceOrientation(const bool theReverseOwnSplitFaceOrientation)
  {
    myReverseOwnSplitFaceOrientation = theReverseOwnSplitFaceOrientation;
  }

  int SplitSectionState() const { return mySplitSectionState; }

  void SetSplitSectionState(const int theSplitSectionState)
  {
    mySplitSectionState = theSplitSectionState;
  }

  bool IsProcessingFaces() const { return myProcessFaces; }

  void SetProcessingFaces(const bool theProcessFaces) { myProcessFaces = theProcessFaces; }

  int Nb2dShapes() const { return myDS2d.NbShapes(); }

  void Add2dShape(const TopoDS_Shape& theShape, const int theRank)
  {
    myDS2d.AddShape(theShape, theRank);
  }

  void AddSplitFaceAncestor(const TopoDS_Shape& theFace, const int theAncestor)
  {
    mySplitFaceAncestors.Bind(theFace, theAncestor);
  }

  bool HasSplitFaceAncestor(const TopoDS_Shape& theFace) const
  {
    return mySplitFaceAncestors.IsBound(theFace);
  }

  void AddFaceToProcess(const TopoDS_Shape& theFace) { myFacesToProcess.Append(theFace); }

  int NbFacesToProcess() const { return myFacesToProcess.Extent(); }
};
} // namespace

TEST(TopOpeBRepBuild_BuilderTest, Faces2dModeIsLocalToBuilder)
{
  TopOpeBRepDS_BuildTool aBuildTool;
  BuilderProbe           aFirstBuilder(aBuildTool);
  BuilderProbe           aSecondBuilder(aBuildTool);

  EXPECT_FALSE(aFirstBuilder.IsFaces2d());
  EXPECT_FALSE(aSecondBuilder.IsFaces2d());

  aFirstBuilder.SetFaces2d(true);

  EXPECT_TRUE(aFirstBuilder.IsFaces2d());
  EXPECT_FALSE(aSecondBuilder.IsFaces2d());
}

TEST(TopOpeBRepBuild_BuilderTest, ClassifySplitEdgeModeIsLocalToBuilder)
{
  TopOpeBRepDS_BuildTool aBuildTool;
  BuilderProbe           aFirstBuilder(aBuildTool);
  BuilderProbe           aSecondBuilder(aBuildTool);

  EXPECT_FALSE(aFirstBuilder.IsClassifySplitEdge());
  EXPECT_FALSE(aSecondBuilder.IsClassifySplitEdge());

  aFirstBuilder.SetClassifySplitEdge(true);

  EXPECT_TRUE(aFirstBuilder.IsClassifySplitEdge());
  EXPECT_FALSE(aSecondBuilder.IsClassifySplitEdge());
}

TEST(TopOpeBRepBuild_BuilderTest, ReverseOwnSplitFaceOrientationModeIsLocalToBuilder)
{
  TopOpeBRepDS_BuildTool aBuildTool;
  BuilderProbe           aFirstBuilder(aBuildTool);
  BuilderProbe           aSecondBuilder(aBuildTool);

  EXPECT_FALSE(aFirstBuilder.IsReverseOwnSplitFaceOrientation());
  EXPECT_FALSE(aSecondBuilder.IsReverseOwnSplitFaceOrientation());

  aFirstBuilder.SetReverseOwnSplitFaceOrientation(true);

  EXPECT_TRUE(aFirstBuilder.IsReverseOwnSplitFaceOrientation());
  EXPECT_FALSE(aSecondBuilder.IsReverseOwnSplitFaceOrientation());
}

TEST(TopOpeBRepBuild_BuilderTest, TemporarySplitStateAnd2dDataAreLocalToBuilder)
{
  TopOpeBRepDS_BuildTool aBuildTool;
  BuilderProbe           aFirstBuilder(aBuildTool);
  BuilderProbe           aSecondBuilder(aBuildTool);

  EXPECT_EQ(aFirstBuilder.SplitSectionState(), 0);
  EXPECT_EQ(aSecondBuilder.SplitSectionState(), 0);
  EXPECT_FALSE(aFirstBuilder.IsProcessingFaces());
  EXPECT_FALSE(aSecondBuilder.IsProcessingFaces());
  EXPECT_EQ(aFirstBuilder.Nb2dShapes(), 0);
  EXPECT_EQ(aSecondBuilder.Nb2dShapes(), 0);

  aFirstBuilder.SetSplitSectionState(TopAbs_IN);
  aFirstBuilder.SetProcessingFaces(true);
  const TopoDS_Shape aShape = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  aFirstBuilder.Add2dShape(aShape, 1);
  aFirstBuilder.AddSplitFaceAncestor(aShape, 1);
  aFirstBuilder.AddFaceToProcess(aShape);

  EXPECT_EQ(aFirstBuilder.SplitSectionState(), TopAbs_IN);
  EXPECT_TRUE(aFirstBuilder.IsProcessingFaces());
  EXPECT_EQ(aFirstBuilder.Nb2dShapes(), 1);
  EXPECT_TRUE(aFirstBuilder.HasSplitFaceAncestor(aShape));
  EXPECT_EQ(aFirstBuilder.NbFacesToProcess(), 1);
  EXPECT_EQ(aSecondBuilder.SplitSectionState(), 0);
  EXPECT_FALSE(aSecondBuilder.IsProcessingFaces());
  EXPECT_EQ(aSecondBuilder.Nb2dShapes(), 0);
  EXPECT_FALSE(aSecondBuilder.HasSplitFaceAncestor(aShape));
  EXPECT_EQ(aSecondBuilder.NbFacesToProcess(), 0);
}
