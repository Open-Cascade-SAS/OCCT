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

#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRep_Tool.hxx>
#include <Geom_SphericalSurface.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

// Migrated from tests/bugs/modalg_5/bug25591.  The second unique edge of a
// sphere shell created from a surface must not acquire a spurious 3D curve.
TEST(BRepBuilderAPI_MakeShellTest, ModalgBug_25591_SphereSeamHasNo3dCurve)
{
  const occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 100.0);
  BRepBuilderAPI_MakeShell aMaker(aSphere, false);
  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shell aShell = aMaker.Shell();
  ASSERT_FALSE(aShell.IsNull());

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(aShell, TopAbs_EDGE, anEdges);
  ASSERT_GE(anEdges.Extent(), 2);

  TopLoc_Location              aLocation;
  double                       aFirst = 0.0, aLast = 0.0;
  const occ::handle<Geom_Curve> aCurve =
    BRep_Tool::Curve(TopoDS::Edge(anEdges(2)), aLocation, aFirst, aLast);
  EXPECT_TRUE(aCurve.IsNull());
}
