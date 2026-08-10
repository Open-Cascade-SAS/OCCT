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

#include <BRepAlgoAPI_Section.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

// Test OCCN2: BRepAlgoAPI_Section of a cylinder intersecting a sphere.
// Migrated from QABugs_17.cxx OCCN2
TEST(BRepAlgoAPI_SectionTest, OCCN2_CylinderSphereSectionIsDone)
{
  BRepPrimAPI_MakeCylinder aCylMaker(50., 200.);
  const TopoDS_Shape&      aCylinder = aCylMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(gp_Pnt(60., 0., 100.), 50.);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepAlgoAPI_Section aSection(aCylinder, aSphere);
  EXPECT_TRUE(aSection.IsDone());
  EXPECT_FALSE(aSection.Shape().IsNull());
}
