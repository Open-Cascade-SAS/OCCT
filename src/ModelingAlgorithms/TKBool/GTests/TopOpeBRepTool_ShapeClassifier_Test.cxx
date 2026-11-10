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
// Alternatively, this file may be distributed under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopAbs_State.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopoDS_Face.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>

TEST(TopOpeBRepTool_ShapeClassifierTest, DefaultStateIsInitialized)
{
  TopOpeBRepTool_ShapeClassifier aClassifier;

  EXPECT_EQ(aClassifier.SameDomain(), -1);
  EXPECT_EQ(aClassifier.State(), TopAbs_UNKNOWN);
}

TEST(TopOpeBRepTool_ShapeClassifierTest, ClassifiesPointUsingConstructorReference)
{
  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), 0.0, 10.0, 0.0, 10.0).Face();
  TopOpeBRepTool_ShapeClassifier aClassifier(aFace);

  aClassifier.StateP2DReference(gp_Pnt2d(5.0, 5.0));

  EXPECT_EQ(aClassifier.State(), TopAbs_IN);
}
