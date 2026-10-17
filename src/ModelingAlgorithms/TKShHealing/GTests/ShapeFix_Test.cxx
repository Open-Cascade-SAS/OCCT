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

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Message_ProgressRange.hxx>
#include <Message_ProgressIndicator.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeFix.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

namespace
{
//! Cancels an operation at its first progress check.
class ShapeFix_CancelProgress : public Message_ProgressIndicator
{
protected:
  //! Requests cancellation.
  bool UserBreak() override;
  //! Ignores progress display.
  void Show(const Message_ProgressScope&, bool) override;
};

//==================================================================================================

bool ShapeFix_CancelProgress::UserBreak()
{
  return true;
}

//==================================================================================================

void ShapeFix_CancelProgress::Show(const Message_ProgressScope&, bool) {}
} // namespace

TEST(ShapeFixTest, SameParameterPreservesSharedEdgeMessages)
{
  for (const int aNbOccurrences : {1, 2, 4})
  {
    for (const bool hasMovedFace : {false, true})
    {
      for (const bool toEnforce : {false, true})
      {
        SCOPED_TRACE(aNbOccurrences);
        SCOPED_TRACE(hasMovedFace);
        SCOPED_TRACE(toEnforce);
        BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp_Pnt(), gp_Dir(0.0, 0.0, 1.0)),
                                             0.0,
                                             1.0,
                                             0.0,
                                             1.0);
        ASSERT_TRUE(aFaceBuilder.IsDone());
        const TopoDS_Face       aFace = aFaceBuilder.Face();
        BRepBuilderAPI_MakeEdge anEdgeBuilder(gp_Pnt(0.0, 0.0, 2.0), gp_Pnt(1.0, 0.0, 2.0));
        ASSERT_TRUE(anEdgeBuilder.IsDone());
        BRep_Builder    aBuilder;
        TopoDS_Compound aShape;
        aBuilder.MakeCompound(aShape);
        for (int anOccurrence = 0; anOccurrence < aNbOccurrences; ++anOccurrence)
        {
          aBuilder.Add(aShape, anOccurrence % 2 == 0 ? TopoDS_Shape(aFace) : aFace.Reversed());
          aBuilder.Add(aShape, anEdgeBuilder.Edge());
        }
        if (hasMovedFace)
        {
          gp_Trsf aPlacement;
          aPlacement.SetTranslation(gp_Vec(2.0, 0.0, 0.0));
          aBuilder.Add(aShape, aFace.Moved(TopLoc_Location(aPlacement)));
        }
        const occ::handle<ShapeExtend_MsgRegistrator> aMessages = new ShapeExtend_MsgRegistrator;
        EXPECT_TRUE(
          ShapeFix::SameParameter(aShape, toEnforce, 0.0, Message_ProgressRange(), aMessages));
        EXPECT_EQ(aMessages->MapShape().Size(), toEnforce ? (hasMovedFace ? 9u : 5u) : 0u);
        for (const auto& aList : aMessages->MapShape())
        {
          EXPECT_EQ(aList.Size(), 1u);
        }
        EXPECT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
        EXPECT_TRUE(BRep_Tool::SameParameter(anEdgeBuilder.Edge()));
      }
    }
  }
}

TEST(ShapeFixTest, SameParameterHandlesEmptyShapeAndCancellation)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aShape;
  aBuilder.MakeCompound(aShape);
  EXPECT_TRUE(ShapeFix::SameParameter(aShape, false));
  const occ::handle<ShapeFix_CancelProgress> aProgress = new ShapeFix_CancelProgress;
  EXPECT_FALSE(ShapeFix::SameParameter(aShape, false, 0.0, aProgress->Start()));
  BRepBuilderAPI_MakeEdge anEdgeBuilder(gp_Pnt(), gp_Pnt(1.0, 0.0, 0.0));
  ASSERT_TRUE(anEdgeBuilder.IsDone());
  aBuilder.Add(aShape, anEdgeBuilder.Edge());
  EXPECT_FALSE(ShapeFix::SameParameter(aShape, true, 0.0, aProgress->Start()));
  EXPECT_TRUE(BRep_Tool::SameParameter(anEdgeBuilder.Edge()));
}
