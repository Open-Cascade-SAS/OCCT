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

#include <StdSelect_BRepSelectionTool.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>

#include <Precision.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <AIS_Shape.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitiveCylinder.hxx>

#include <gtest/gtest.h>

TEST(StdSelect_BRepSelectionToolTest, Mirrored_Cylinder_From_Extrusion)
{
  const double radius = 10.0;
  const double height = 30.0;

  gp_Circ circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), radius);
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circle);
  TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);
  gp_Pln plane(gp::XOY());
  TopoDS_Face face = BRepBuilderAPI_MakeFace(plane, wire);
  gp_Vec prismVec(0.0, 0.0, height);
  TopoDS_Shape prism = BRepPrimAPI_MakePrism(face, prismVec);
  gp_Trsf mirrorTrsf;
  mirrorTrsf.SetMirror(gp::XOY());
  TopoDS_Shape mirroredPrism = BRepBuilderAPI_Transform(prism, mirrorTrsf).Shape();

  occ::handle<AIS_Shape>                anObject = new AIS_Shape(mirroredPrism);
  occ::handle<SelectMgr_ViewerSelector> aSelector = new SelectMgr_ViewerSelector;
  SelectMgr_SelectionManager            aManager(aSelector);
  aManager.Load(anObject);
  aManager.Activate(anObject, 6); // mode 6 : selection of the constituent solids

  const occ::handle<SelectMgr_Selection> aSelection = anObject->Selection(6);
  ASSERT_FALSE(aSelection.IsNull());
  ASSERT_FALSE(aSelection->IsEmpty());

  const NCollection_DynamicArray<occ::handle<SelectMgr_SensitiveEntity>>& anEntities = aSelection->Entities();
  ASSERT_TRUE(anEntities.Size() == 1);

  NCollection_DynamicArray<occ::handle<SelectMgr_SensitiveEntity>>::Iterator anEntityIter(anEntities);
  ASSERT_TRUE(anEntityIter.More());

  const occ::handle<SelectMgr_SensitiveEntity>& anEnt = anEntityIter.Value();
  ASSERT_FALSE(anEnt.IsNull());

  const occ::handle<Select3D_SensitiveEntity>& anEnt3D = anEnt->BaseSensitive();
  ASSERT_FALSE(anEnt3D.IsNull());

  ASSERT_TRUE(anEnt3D->IsKind(STANDARD_TYPE(Select3D_SensitiveCylinder)));

  occ::handle<Select3D_SensitiveCylinder> aCyl = occ::down_cast<Select3D_SensitiveCylinder>(anEnt3D);
  ASSERT_FALSE(aCyl.IsNull());

  gp_Pnt aCG = aCyl->CenterOfGeometry();
  EXPECT_NEAR(aCG.Z(), -15.0, Precision::Confusion());
}
