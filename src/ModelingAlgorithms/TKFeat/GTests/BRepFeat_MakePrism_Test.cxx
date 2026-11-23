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

#include <BRepFeat_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gtest/gtest.h>

TEST(BRepFeat_MakePrism, UntilHeightDelegatesOptionalLimits)
{
  for (const bool hasLimit : {false, true})
  {
    for (const int aMode : {0, 1, 2})
    {
      SCOPED_TRACE(hasLimit);
      SCOPED_TRACE(aMode);
      const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(20, 20, 10).Shape();
      const double       aZ     = aMode == 0 ? 0.0 : 10.0;
      const TopoDS_Face  aProfile =
        BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), 3, 9, 3, 9).Face();
      const TopoDS_Face aLimit =
        BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, aZ + 5.0), gp_Dir(0, 0, 1))).Face();
      BRepFeat_MakePrism aFeature(aStock, aProfile, TopoDS_Face(), gp_Dir(0, 0, 1), aMode, false);
      ASSERT_NO_THROW(
        aFeature.PerformUntilHeight(hasLimit ? aLimit : TopoDS_Face(), hasLimit ? 0.0 : 5.0));
      ASSERT_TRUE(aFeature.IsDone());
      EXPECT_TRUE(BRepCheck_Analyzer(aFeature.Shape()).IsValid());
      GProp_GProps aProps;
      BRepGProp::VolumeProperties(aFeature.Shape(), aProps);
      EXPECT_NEAR(aProps.Mass(), aMode == 0 ? 3820.0 : aMode == 1 ? 4180.0 : 180.0, 1.e-6);
      BRepGProp::VolumeProperties(aStock, aProps);
      EXPECT_NEAR(aProps.Mass(), 4000.0, 1.e-6);
    }
  }
}
