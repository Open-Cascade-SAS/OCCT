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
#include <TopOpeBRepBuild_Builder1.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

TEST(TopOpeBRepBuild_Builder1Test, SolidBuildCanBeRepeatedAfterClear)
{
  const TopoDS_Shape aFirstSolid  = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  const TopoDS_Shape aSecondSolid = BRepPrimAPI_MakeBox(5.0, 6.0, 7.0).Shape();

  const occ::handle<TopOpeBRepDS_HDataStructure> aHDS = new TopOpeBRepDS_HDataStructure();
  aHDS->ChangeDS().AddShape(aFirstSolid, 1);
  aHDS->ChangeDS().AddShape(aSecondSolid, 2);
  aHDS->AddAncestors(aFirstSolid);
  aHDS->AddAncestors(aSecondSolid);

  TopOpeBRepDS_BuildTool  aBuildTool;
  TopOpeBRepBuild_Builder1 aBuilder(aBuildTool);

  EXPECT_NO_THROW(aBuilder.Perform(aHDS, aFirstSolid, aSecondSolid));
  EXPECT_TRUE(aBuilder.DataStructure() == aHDS);

  aBuilder.Clear();

  EXPECT_NO_THROW(aBuilder.Perform(aHDS, aFirstSolid, aSecondSolid));
  EXPECT_TRUE(aBuilder.DataStructure() == aHDS);
}
